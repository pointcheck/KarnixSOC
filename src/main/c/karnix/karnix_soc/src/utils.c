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

#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

void init_sbrk(unsigned int* heap, int size) {

	if(heap == NULL) {
		heap_start = (unsigned int*)& _ram_heap_start;
		heap_end = (unsigned int*)& _ram_heap_end;
	} else {
		heap_start = heap;
		heap_end = heap_start + size;
	}

	sbrk_heap_end = (char*) heap_start;

	#if(DEBUG_SBRK)
	printk("init_sbrk() sbrk_heap_end: %p\r\n", (unsigned int)sbrk_heap_end);
	#endif
}


void * _sbrk (unsigned int incr) {

	#if(DEBUG_SBRK)
	printk("_sbrk() request, incr: %d\r\n", (unsigned int)incr);
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
		printk("_sbrk() out of mem, sbrk_heap_end: %p, heap_end: %p, incr: %d\r\n",
			(unsigned int)sbrk_heap_end, (unsigned int)heap_end, (unsigned int)incr);
		#endif

		return ((void*)-1); // error - no more memory
	}

	sbrk_heap_end += incr;

	#if(DEBUG_SBRK)
	printk("_sbrk() prev_heap_end: %p\r\n", (unsigned int)prev_heap_end);
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


void printk(const char *fmt, ...) {
	va_list vargs;
	va_start(vargs, fmt);

	while (*fmt) {
		if (*fmt == '%') {
			fmt++; // Skip '%'
			switch (*fmt) { // Read the next character
				case '\0': // '%' at the end of the format string
					uart_write(UART0, '%');
					goto end;
				case '%': // Print '%'
					uart_write(UART0, '%');
					break;
				case 's': { // Print a NULL-terminated string.
					const char *s = va_arg(vargs, const char *);
					while (*s) {
						uart_write(UART0, *s);
						s++;
					}
					break;
				}
				case 'd': { // Print an integer in decimal.
					int value = va_arg(vargs, int);
					if (value < 0) {
						uart_write(UART0, '-');
						value = -value;
					}

					int divisor = 1;
					while (value / divisor > 9)
						divisor *= 10;

					while (divisor > 0) {
						uart_write(UART0, '0' + value / divisor);
						value %= divisor;
						divisor /= 10;
					}

					break;
				}

				case 'p': {
					uart_write(UART0, '0');
					uart_write(UART0, 'x');
				};
				case 'x': { // Print an integer in hexadecimal.
					int value = va_arg(vargs, int);
					for (int i = 7; i >= 0; i--) {
						int nibble = (value >> (i * 4)) & 0xf;
						uart_write(UART0, "0123456789abcdef"[nibble]);
					}
					break;
				}
			}
		} else
			uart_write(UART0, *fmt);

		fmt++;
	}

end:
	va_end(vargs);
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
	printk("*** GOING HARD RESET ***\r\n");
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

