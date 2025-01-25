/*
 * Simple Bootloader which can be run from synthesized RAM during SoC startup
 *
 * Copyright (C) 2024-2025 Fabmicro, LLC. Tyumen, Russia.
 *
 * Written by Ruslan Zalata <rz@fabmicro.ru>
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
#include "qspi.h"

#define APPS_NUM	17		// How many applications can be in NOR flash to look for
#define	MAGIC		0x12300013	// Magic number to check for

// Where in NOR flash to look for applications, defines offsets relative to QSPI_MEMORY_ADDRESS 
uint32_t app_offsets[] = {0xe0000, 0x100000, 0x200000, 0x300000, 0x400000, 0x500000,
			 0x600000, 0x700000, 0x800000, 0x900000, 0xa00000, 0xb00000,
			 0xc00000, 0xd00000, 0xe00000, 0xf00000};

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

int main(void) {

	PLIC->ENABLE = 0; /* Disable all PLIC IRQ lines, jsut in case */

	/* Wait for CPU clocks to settle, needed only if running stand-alone. */
	delay_us(2000000);

	/* Save current stack pointer for trap handling. */
	asm volatile ("sw sp, (%0)" :  : "r"(&context.sp));

	printk("\r\nKarnix SoC Bootloader, build #%d on %s at %s\r\n", BUILD_NUMBER, __DATE__, __TIME__);


	for(int i = 0; i < APPS_NUM; i++) {
		uint32_t *app = (uint32_t*)(QSPI_MEMORY_ADDRESS + app_offsets[i]);
		uint32_t (*long_jump)(void) = (uint32_t (*)(void)) app;

		if(*app == MAGIC) {
			printk("Calling application at %p\r\n\r\n", app);
			return long_jump();
		}
	}

	printk("No apps found in Flash!\r\n");

	return -1; // Do we need ot return anything to nowhere ? 
}

void timerInterrupt(void) {
	/* Not supported on this machine */
}

void externalInterrupt(void){

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

	printk("\r\n*** BOOT TRAP: %p at %p = %p, mtval = %p\r\n"
	       "*** BOOT CONTEXT: sp = %p, gp = %p, tp = %p, heap_end = %p\r\n",
		cause, context.mepc, *(uint32_t*)(context.mepc & 0xfffffffc),
		context.mtval, context.sp, context.gp, context.tp,
		(uint32_t)sbrk_heap_end);

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

