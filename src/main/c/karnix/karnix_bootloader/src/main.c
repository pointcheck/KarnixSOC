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

#define APPS_NUM	16		// How many applications can be in NOR flash to look for
#define	MAGIC		0x12300013	// Magic number to check for

// Where in NOR flash to look for applications, defines offsets relative to QSPI_MEMORY_ADDRESS 
const uint32_t app_offsets[] = {0xe0000, 0x100000, 0x200000, 0x300000, 0x400000, 0x500000,
			 0x600000, 0x700000, 0x800000, 0x900000, 0xa00000, 0xb00000,
			 0xc00000, 0xd00000, 0xe00000, 0xf00000
};

// Default RGB palette for text mode
const uint32_t rgb_palette[16] = {
			0x00000000, 0x000000f0, 0x0000f000, 0x00f00000,
			0x0000f0f0, 0x00f000f0, 0x00f0f000, 0x00f0f0f0,
			0x000f0f0f, 0x000f0fff, 0x000fff0f, 0x00ff0f0f,
			0x000fffff, 0x00ff0fff, 0x00ffff0f, 0x00ffffff,
};

extern void __sinit(void *); /* LIBC init function */
extern unsigned int _IMPURE_DATA; /* LIBC context structure */
extern unsigned int trap_entry; /* Trap entry point provided by crt.S */

/* Context saving structure */

struct _context {
	uint32_t sp;
	uint32_t gp;
	uint32_t tp;
	uint32_t mepc;	/* Trap happened at this instruction */
	uint32_t mtval; /* access address causing the trap   */
} context;

char *BOOTLOADER = "Bootloader";

int main(void) {

begin:

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init
	PLIC->ENABLE = 0; /* Disable all PLIC IRQ lines, jsut in case */

	/* Wait for CPU clocks to settle, needed only if running stand-alone. */
	delay_us(2000000);

	// Initialize CGA video

	// Set text mode and load color palette
	cga_set_video_mode(CGA_MODE_TEXT);

	// Load default color palette
	cga_set_palette((uint32_t*)rgb_palette);

	// Clear video framebuffer
	memset(CGA->FB, 0, CGA_FRAMEBUFFER_SIZE);
	cga_set_cursor_xy(0, 0);
	cga_set_scroll(0);

	/* Save current stack pointer for trap handling. */
	asm volatile ("sw sp, (%0)" :  : "r"(&context.sp));

	// Initialize LIBC
	init_sbrk(NULL, 0); // Initialize heap for malloc to use on-chip RAM
	__sinit(&_IMPURE_DATA); // Init LIBC impure_data structure

	xprintf("[%s] Karnix SoC Bootloader, build #%d on %s at %s\r\n",
		BOOTLOADER, BUILD_NUMBER, __DATE__, __TIME__);


	for(int i = 0; i < APPS_NUM; i++) {
		uint32_t *app = (uint32_t*)(QSPI_MEMORY_ADDRESS + app_offsets[i]);
		uint32_t (*long_jump)(void) = (uint32_t (*)(void)) app;

		if(*app == MAGIC) {
			xprintf("[%s] calling application at %p\r\n\r\n", BOOTLOADER, app);
			delay_us(2000000);
			long_jump();
		}
	}

	xprintf("[%s] No more apps found in Flash!\r\n\r\n", BOOTLOADER);

	goto begin;

	return -1; // Do we need to return anything to nowhere ? 
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

	xprintf("\r\n*** BOOT TRAP: %p at %p = %p, mtval = %p\r\n"
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

