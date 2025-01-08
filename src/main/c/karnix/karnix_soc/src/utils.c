#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <_syslist.h>
#include <reent.h>
#include "soc.h"
#include "uart.h"
#include "utils.h"
#include "wd.h"

// Below is some linker specific stuff 
unsigned char* sbrk_heap_end = 0; /* tracks heap usage */
unsigned int* heap_start = 0; /* programmer defined heap start */
unsigned int* heap_end = 0; /* programmer defined heap end */

// Below is necessary for malloc to work in non-threaded environment 
void __malloc_lock(struct _reent *REENT) { /* print("__malloc_lock()\r\n"); */ }
void __malloc_unlock(struct _reent *REENT) { /* print("__malloc_unlock()\r\n"); */ }

void init_sbrk(unsigned int* heap, int size) {

	if(heap == NULL) {
		heap_start = (unsigned int*)& _ram_heap_start;
		heap_end = (unsigned int*)& _ram_heap_end;
	} else {
		heap_start = heap;
		heap_end = heap_start + size;
	}

	sbrk_heap_end = (char*) heap_start;
}


void * _sbrk (unsigned int incr) {

	#if(DEBUG_SBRK)
	char str[16];
	print("_sbrk() request, incr: ");
	to_hex(str, (unsigned int)incr);
	println(str);
	#endif

	unsigned char* prev_heap_end;

	if (sbrk_heap_end == 0) {
		heap_start = & _ram_heap_start;
		heap_end = & _ram_heap_end;
		sbrk_heap_end = (char*) heap_start;
	}

	prev_heap_end = sbrk_heap_end;

	if((unsigned int)(sbrk_heap_end + incr) >= (unsigned int)heap_end) {

		#if(DEBUG_SBRK)
		print("_sbrk() out of mem, sbrk_heap_end: ");
		to_hex(str, (unsigned int)sbrk_heap_end);
		print(str);
		print(", heap_end: ");
		to_hex(str, (unsigned int)heap_end);
		print(str);
		print(", incr: ");
		to_hex(str, (unsigned int)incr);
		println(str);
		#endif

		return ((void*)-1); // error - no more memory
	}

	sbrk_heap_end += incr;

	#if(DEBUG_SBRK)
	print("_sbrk() prev_heap_end: ");
	to_hex(str, (unsigned int)prev_heap_end);
	println(str);
	#endif

	return (void *) prev_heap_end;
}


void delay(uint32_t loops) {
	for(int i=0;i<loops;i++){
		asm("add a2,a2,0"); // NOP
	}
}


void delay_us(uint32_t us) {
	unsigned long long t0, t;

	t0 = get_mtime();

	while(1) {
		t = get_mtime();
		if(t - t0 >= us)
			break;
	}
}


void print_uart0(const char*str) {
	while(*str){
		uart_write(UART0,*str);
		str++;
	}
}

void print_uart1(const char*str) {
	while(*str){
		uart_write(UART1,*str);
		str++;
	}
}

int _write (int fd, const void *buf, size_t count) {
	int i;
	char* p = (char*) buf;
	for(i = 0; i < count; i++) { uart_write(UART0, *p++); }
	return count;
}

int _read (int fd, const void *buf, size_t count) { return 1; }
int _close(int fd) { return -1; }
int _lseek(int fd, int offset, int whence) { return 0 ;}
int _isatty(int fd) { return 1; }

int _fstat(int fd, struct stat *sb) { 
	sb->st_mode = S_IFCHR;
	return 0; 
}

void _kill(int pid, int sig) {
  return;
}

int _getpid(void) {
  return -1;
}


void hard_reboot(void) {
	//void (*begin)(void) = (void*) 0x80000000;
	//begin();
	print("*** GOING HARD RESET ***\r\n");
	delay_us(1000);
	WD->REBOOT = 1;
}


void memcpy_rev(void *dst, void *src, uint32_t count) {
	uint8_t* s = (uint8_t*) src;
	uint8_t* d = (uint8_t*) dst;
	while(count) {
		*d++ = *s--;
		count--;
	}
}


// Fast and simple conversion from BIN/OCT/HEX/DEC to unsigned long.
// Warning: it does not do any sanity checks, use with caution !!!
uint32_t strntoul(const char *buf, int size, int base) {

	uint32_t result = 0;

	for(int i = 0; i < size; i++) {
		uint8_t c = buf[i];
		uint32_t digit = 0;

		if(c >= 0x30 && c <= 0x39) {
			digit = c - 0x30;
		} else if(c >= 0x41 && c <= 0x46) {
			digit = c - 0x41 + 10;
		} else if(c >= 0x61 && c <= 0x64) {
			digit = c - 0x61 + 10;
		} else {
			return 0; // errornous character
		}

		result = result * base + digit;
	}

	return result;
}


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


