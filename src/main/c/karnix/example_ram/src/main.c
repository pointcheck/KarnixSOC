/*
 * Simple program that can be stored in RAM/SRAM at any location.
 *
 * Copyright (C) 2024-2025 Fabmicro, LLC. Tyumen, Russia.
 *
 * Written by Ruslan Zalata <rz@fabmicro.ru>
 *
 * Uses Software Model #1 that is implemented by befault crt.S and linker.ld
 * from karnix_soc HAL collection.
 *
 * This example shows how to set your own interrupt handler, handle CPU traps,
 * init heap for malloc and other LIBC functions. Restoring trap handler on return
 * is not necessary, monitor will do it on itself.
 *
 * To compile:
 * 1. Just call "make"
 *
 * To run from Monitor:
 * 1. Connect your terminal to the Monitor
 * 2. Send "rz 0x90000000" to receive binary file on local machine into SRAM on Karnix
 * 3. Start sending "example_ram" binary using ZModem protocol
 * 4. Once uploaded, check CRC32 using "cksum -o 3" (FreeBSD only)
 * 5. Send "call 0x90000000" command to execute. 
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/reent.h>
#include "soc.h"
#include "riscv.h"
#include "plic.h"
#include "uart.h"
#include "utils.h"


extern unsigned int trap_entry; /* Trap entry point provided by crt.S */

/* Context saving structure */

struct _context {
	char crash_str[16];
	uint32_t sp;
	uint32_t gp;
	uint32_t tp;
	uint32_t mepc;	/* Trap happened at this instruction */
	uint32_t mtval; /* access address causing the trap   */
} context;

/* Some global data used by test fuction. */
char buf[16];

/* Some test function, tell compiler to not optimize it. */
void __attribute__((optimize("O0"))) test(int i) {

	buf[0] = 'A';
	buf[1] = 'B';
	buf[2] = 'C';
	buf[3] = i + '0';
	buf[4] = '\r';
	buf[5] = '\n';
	buf[6] = 0;

	print_uart0(buf);
}


extern const struct __sFILE_fake __sf_fake_stdin;
extern const struct __sFILE_fake __sf_fake_stdout;
extern const struct __sFILE_fake __sf_fake_stderr;
extern unsigned int _IMPURE_DATA; /* reference to .data.impure_data section */
//struct _reent impure_data;
//struct __sFILE fake_stdin;
//struct __sFILE fake_stdout;
//struct __sFILE fake_stderr;
void __sinit(struct _reent*);

int main(void) {

	/* Disable Machine interrupts during tests */ 
	//csr_clear(mstatus, MSTATUS_MIE);
	
	/*
	   Note, when running from Monitor there are many IRQ sources already
	   enabled and configured, including exhausting TIMER0 and TIMER1.
	   So we might be needing to disable interrupts from PLIC, else may hang.
	*/
	//PLIC->ENABLE = 0; /* Disable all PLIC IRQ lines */


	/* Wait for CPU clocks to settle, needed only if running stand-alone. */
	delay_us(2000000);

	/* Save current stack pointer for trap handling. */
	asm volatile ("sw sp, (%0)" :  : "r"(&context.sp));

	print_uart0("\r\n= = = HELLO WORLD = = =\r\n");

	print_uart0("*** Use global read-write data:\r\n");

	for(int i = 0; i < 10; i++)
		test(i);

	print_uart0("\r\n*** Print current context:\r\n");

	print_uart0("SP: ");
	to_hex(buf, (unsigned int)context.sp);
	print_uart0(buf);
	print_uart0("\r\n");

	print_uart0("_stack_start: ");
	to_hex(buf, (unsigned int)& _stack_start);
	print_uart0(buf);
	print_uart0("\r\n");

	print_uart0("_bss_start: ");
	to_hex(buf, (unsigned int)& _bss_start);
	print_uart0(buf);
	print_uart0("\r\n");

	print_uart0("_bss_end: ");
	to_hex(buf, (unsigned int)& _bss_end);
	print_uart0(buf);
	print_uart0("\r\n");

	print_uart0("_ram_heap_start: ");
	to_hex(buf, (unsigned int)& _ram_heap_start);
	print_uart0(buf);
	print_uart0("\r\n");

	print_uart0("_ram_heap_end: ");
	to_hex(buf, (unsigned int)& _ram_heap_end);
	print_uart0(buf);
	print_uart0("\r\n");

	/* Initialize heap for malloc to use free RAM right above the stack */
	print_uart0("\r\n*** Init heap:\r\n");
	init_sbrk(NULL, 0);
	print_uart0("init_sbrk done!\r\n");

	print_uart0("heap_start: ");
	to_hex(buf, (unsigned int)heap_start);
	print_uart0(buf);
	print_uart0("\r\n");

	print_uart0("heap_end: ");
	to_hex(buf, (unsigned int)heap_end);
	print_uart0(buf);
	print_uart0("\r\n");

	print_uart0("sbrk_heap_end: ");
	to_hex(buf, (unsigned int)sbrk_heap_end);
	print_uart0(buf);
	print_uart0("\r\n");

	print_uart0("\r\n*** Adjusting global REENT structure:\r\n");

	*(uint32_t*)&_impure_ptr = (uint32_t)&_IMPURE_DATA;
	*(uint32_t*)&_global_impure_ptr = (uint32_t)_impure_ptr;
	_impure_ptr->_stdin = (__FILE *)&__sf_fake_stdin;
	_impure_ptr->_stdout = (__FILE *)&__sf_fake_stdout;
	_impure_ptr->_stderr = (__FILE *)&__sf_fake_stderr;

	print_uart0("_impure_ptr: ");
	to_hex(buf, (unsigned int)_impure_ptr);
	print_uart0(buf);
	print_uart0("\r\n");

	print_uart0("_global_impure_ptr: ");
	to_hex(buf, (unsigned int)_global_impure_ptr);
	print_uart0(buf);
	print_uart0("\r\n");

	print_uart0("fake_stdout: ");
	to_hex(buf, (unsigned int)_impure_ptr->_stdout);
	print_uart0(buf);
	print_uart0("\r\n");


	print_uart0("\r\n*** Checking malloc:\r\n");


	for(int i = 0; i < 10; i++) {
		char *mem = malloc(1024);
		print_uart0("allocated mem at: ");
		to_hex(buf, (unsigned int)mem);
		print_uart0(buf);
		print_uart0("\r\n");
	}


	print_uart0("\r\n*** Calling LIBC function...\r\n");

	printf("Hello from LIBC! Yoohoo!\r\n");

	print_uart0("\r\n= = = Returning to Monitor = = =\r\n\r\n");

	delay_us(2000000);

	/* return result code to Monitor */
	return 0x11223344;
}

void timerInterrupt(void) {
	/* Not supported on this machine */
}

void externalInterrupt(void){

	if(PLIC->PENDING & PLIC_IRQ_TIMER0) {
		print_uart0("TIMER0 IRQ\r\n");
		timer_run(TIMER0, 1000000); // 1 s timer
		PLIC->PENDING &= ~PLIC_IRQ_TIMER0;
	}

	if(PLIC->PENDING & PLIC_IRQ_TIMER1) {
		print_uart0("TIMER1 IRQ\r\n");
		timer_run(TIMER1, 100000); // 100 ms timer
		PLIC->PENDING &= ~PLIC_IRQ_TIMER1;
	}

	PLIC->PENDING = 0; /* Clear all pending IRQ lines */
}

void crash(int cause) {
	
	context.mepc = csr_read(mepc);
	context.mtval = csr_read(mtval);

	asm volatile ("sw gp, (%0)" :  : "r"(&context.gp));

	asm volatile ("sw tp, (%0)" :  : "r"(&context.tp));

	/* Add 16 to SP to reference upper level */
	asm volatile ("mv t1, sp; \
		      addi t1,t1,16; \
	      	      sw t1, (%0)" :  : "r"(&context.sp)); 

	print_uart0("\r\n*** TRAP: ");
	to_hex(context.crash_str, cause);
	print_uart0(context.crash_str);
	print_uart0(" at ");

	to_hex(context.crash_str, context.mepc);
	print(context.crash_str);
	print(" = ");
	to_hex(context.crash_str, *(uint32_t*)(context.mepc & 0xfffffffc));
	print(context.crash_str);

	print(", mtval = ");
	to_hex(context.crash_str, context.mtval);
	print(context.crash_str);

	print(", sp = ");
	to_hex(context.crash_str, context.sp);
	print(context.crash_str);

	print(", gp = ");
	to_hex(context.crash_str, context.gp);
	print(context.crash_str);

	print(", tp = ");
	to_hex(context.crash_str, context.tp);
	print(context.crash_str);

	print(", heap_end = ");
	to_hex(context.crash_str, (uint32_t)sbrk_heap_end);
	print(context.crash_str);

	print("\r\n");

	for(;;);
}


void irqCallback() {

	//print_uart0("!");

	int32_t mcause = csr_read(mcause);
	int32_t interrupt = mcause < 0;    /* Interrupt if true, exception if false */
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
}

