#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "soc.h"
#include "riscv.h"
#include "uart.h"
#include "utils.h"
#include "crc32.h"
#include "qspi.h"

/* Below is some linker specific stuff */
extern unsigned int   _stack_start; /* Set by linker.  */
extern unsigned int   _stack_size; /* Set by linker.  */
extern unsigned int   trap_entry;
extern unsigned int   heap_start; /* programmer defined heap start */
extern unsigned int   heap_end; /* programmer defined heap end */

void println(const char*str){
	print_uart0(str);
	print_uart0("\r\n");
}

char to_hex_nibble(char n)
{
	n &= 0x0f;

	if(n > 0x09)
		return n + 'A' - 0x0A;
	else
		return n + '0';
}

void to_hex(char*s , unsigned int n)
{
	for(int i = 0; i < 8; i++) {
		s[i] = to_hex_nibble(n >> (28 - i*4));
	}
	s[8] = 0;
}


void test_nor_xip(void) {
	uint32_t t0, t1;
	uint32_t nor_addr = 0x00800000; // Somewhere in the middle of NOR flash
	uint32_t nor_size = 0x7f0000; // 8 MB should be enough
	volatile uint32_t* mem_addr = (uint32_t*)(QSPI_MEMORY_ADDRESS + nor_addr);

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during test 

	// Phase I: Erasing

	t0 = get_mtime();
	for(uint32_t addr = (uint32_t) mem_addr; addr < (uint32_t)mem_addr + nor_size; addr += 4096) {
		printf("Erasing NOR sector at: 0x%08x (%d)... ", addr, (addr & 0xffffff) >> 12);
		qspi_erase_sector(addr); 
		while(qspi_get_status() & QSPI_DEVICE_STATUS_BUSY);
		printf("Done! NOR Status Reg 1 = 0x%08x\r\n", qspi_get_status());
	}
	t1 = get_mtime();

	printf("NOR erase of %d bytes took %d us\r\n", nor_size, t1-t0);

	delay_us(1000000);

	// Phase II: Programming

	printf("Programming NOR at: 0x%08x, size: 0x%08x\r\n", mem_addr, nor_size);

	t0 = get_mtime();
	for(uint32_t addr = (uint32_t) mem_addr; addr < (uint32_t)mem_addr + nor_size; addr += 4) {
		qspi_write_enable();
		if(addr == (uint32_t)mem_addr + nor_size - 4)
			*(uint32_t*) addr = 0x80828082; // JALR X0, X1 (RET)
		else
			*(uint32_t*) addr = 0x00000013; // ADDI x0, x0, 0 (NOP)

		while(qspi_get_status() & QSPI_DEVICE_STATUS_BUSY);
		//qspi_write_disable();
	}
	t1 = get_mtime();

	printf("NOR write of %d bytes took %d us\r\n", nor_size, t1-t0);

	// Phase III: Checking

	printf("Checking what has been written... ");

	uint32_t sum0 = 0, sum1 = 0;

	t0 = get_mtime();
	for(uint32_t addr = (uint32_t) mem_addr; addr < (uint32_t)mem_addr + nor_size; addr += 4) {
		sum0 += *(uint32_t*) addr;

		if(addr == (uint32_t)mem_addr + nor_size - 4)
			sum1 += 0x80828082; // JALR X0, X1 (RET)
		else
			sum1 += 0x00000013; // ADDI x0, x0, 0 (NOP)
	}
	t1 = get_mtime();

	printf("%s. sum0 = 0x%08x, sum1 = 0x%08x\r\n",
			sum0 == sum1 ? "Matched" : "Mismatch", sum0, sum1);

	if(sum0 != sum1) {
		printf("Hang!\r\n");
		for(;;);
	}

	delay_us(1000000);

	// Phase IV: Executing code from NOR

	printf("Running XiP test: jump to 0x%08x (data: 0x%08x)\r\n", mem_addr, *mem_addr);

	while(1) {
		void (*long_jump)(void) = (void (*)(void)) mem_addr;
		t0 = get_mtime();
		long_jump();
		t1 = get_mtime();
		printf("NOR test executed jump to 0x%08x, time = %d\r\n", long_jump, t1-t0);
	}
}


void main(void) {


	delay_us(100000); // Wait for FCO to settle

	init_sbrk(NULL, 0); // Initialize heap for malloc to use on-chip RAM

	printf("QSPI/NOR flash controller test utility.\r\n\r\n");


	test_nor_xip();
}



void timerInterrupt(void) {
	// Not supported on this machine
}


void externalInterrupt(void){
	// Not implemente for this application
}


static char crash_str[16];
static uint32_t pc;

void crash(int cause) {
	
	print("\r\n*** TRAP: ");
	to_hex(crash_str, cause);
	print(crash_str);
	print(" at ");

	pc = csr_read(mepc);

	to_hex(crash_str, pc);
	print(crash_str);
	print(" = ");

	to_hex(crash_str, *(uint32_t*)pc);
	print(crash_str);
	print("\r\n");

}

void irqCallback() {

	// Interrupts are already disabled by machine

	int32_t mcause = csr_read(mcause);
	int32_t interrupt = mcause < 0;    //Interrupt if true, exception if false
	int32_t cause     = mcause & 0xF;
	if(interrupt){
		switch(cause) {
			case CAUSE_MACHINE_TIMER: timerInterrupt(); break;
			case CAUSE_MACHINE_EXTERNAL: externalInterrupt(); break;
			default: crash(2); break;
		}
	} else {
		crash(1);
	}

	// Interrupt state will be restored by machine on MRET
}

