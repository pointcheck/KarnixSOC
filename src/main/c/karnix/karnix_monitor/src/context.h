/*
 * Contains context structure and context handling routines
 *
 */ 

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <stdint.h>
#include "riscv.h"
#include "plic.h"

extern unsigned int   trap_entry;

extern struct _context {
	uint32_t gp;
	uint32_t tp;
	uint32_t sp;
	uint32_t ra;
	uint32_t pc;
	uint32_t plic_enable;
	uint32_t plic_polarity;
	uint32_t plic_edge;
	uint32_t cur_pc;
	uint32_t mtval;
	volatile uint32_t trap_flag;
} context;

void context_save(void);


// This inline function restores current context (SP, GP, RA)
// Tell compiler to never make it as function call (should be defined "inline")
//
#define context_restore() \
	csr_clear(mstatus, MSTATUS_MIE); \
	context.trap_flag = 0; \
	asm volatile ("lw ra, (%0)" :  : "r"(&context.ra)); \
	asm volatile ("lw gp, (%0)" :  : "r"(&context.gp)); \
	asm volatile ("lw sp, (%0)" :  : "r"(&context.sp)); \
	PLIC->ENABLE = context.plic_enable; \
	PLIC->EDGE = context.plic_edge; \
	PLIC->POLARITY = context.plic_polarity; \
	csr_write(mtvec, &trap_entry); \
	csr_set(mstatus, MSTATUS_MIE);

#endif

