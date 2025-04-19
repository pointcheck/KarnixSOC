#include "utils.h" 
#include "sram.h"

int sram_test_write_random_ints(int interations) {
       volatile unsigned int *mem;
       unsigned int fill;
       int fails = 0;

       for(int i = 0; i < interations; i++) {
	       fill = 0xdeadbeef + i;
	       mem = (unsigned int*) SRAM_ADDR_BEGIN;

	       xprintf("Filling SRAM at: %p, size: %d bytes...\r\n", mem, SRAM_SIZE);

	       while((unsigned int)mem < SRAM_ADDR_END) {
		       *mem++ = fill;
		       fill += 0xdeadbeef; // generate pseudo-random data
	       }

	       fill = 0xdeadbeef + i;
	       mem = (unsigned int*) SRAM_ADDR_BEGIN;

	       xprintf("Checking SRAM at: %p, size: %d bytes...\r\n", mem, SRAM_SIZE);

	       while((unsigned int)mem < SRAM_ADDR_END) {
		       unsigned int tmp = *mem;
		       if(tmp != fill) {
			       xprintf("SRAM check failed at: %p, expected: %p, got: %p\r\n", mem, fill, tmp);
			       fails++;
		       } else {
			       //xprintf("\r\nMem check OK     at: %p, expected: %p, got: %p\r\n", mem, fill, *mem);
		       }
		       mem++;
		       fill += 0xdeadbeef; // generate pseudo-random data
		       break;
	       }

	       if((unsigned int)mem == SRAM_ADDR_END)
		       xprintf("SRAM Fails: %d\r\n", fails);

	       if(fails)
		       break;
       }

       return fails++;
}

