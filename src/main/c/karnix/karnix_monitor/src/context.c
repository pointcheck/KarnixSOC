
/*
 * Contains context structure and context handling rutines
 *
 */ 

#include "context.h"

struct _context context = {0};


// This function saves current execution context (pc, ra, sp, gp, tp)
// Tell compiler to do NOT optimize this function call
void __attribute__((optimize("O0"))) context_save(void) {
       
	context.trap_flag = 0;

	asm volatile ("sw gp, (%0)" :  : "r"(&context.gp));

	asm volatile ("sw tp, (%0)" :  : "r"(&context.tp));

	asm volatile ("sw ra, (%0)" :  : "r"(&context.ra));

	asm volatile ("auipc t1,0; \
		       sw t1, (%0)" :  : "r"(&context.pc));

	// We need parent's stack frame, reference it by adding size of current (16 bytes)
	asm volatile ("mv t1, sp; \
		addi t1,t1,16; \
		sw t1, (%0)" :  : "r"(&context.sp)); 
}

