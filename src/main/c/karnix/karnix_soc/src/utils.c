#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <_syslist.h>
#include <reent.h>
#include "soc.h"
#include "uart.h"
#include "utils.h"
#include "wd.h"


// Below is some linker specific stuff 
extern unsigned int end; /* Set by linker.  */
extern unsigned int _ram_heap_start; /* Set by linker.  */
extern unsigned int _ram_heap_end; /* Set by linker.  */
extern unsigned int _stack_start; /* Set by linker.  */
extern unsigned int _stack_size; /* Set by linker.  */
extern unsigned int trap_entry;
unsigned char* sbrk_heap_end = 0; /* tracks heap usage */
unsigned int* heap_start = 0; /* programmer define heap start */
unsigned int* heap_end = 0; /* programmer defined heap end */


// Below is neede for malloc to work in non-threaded environment 
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

	//print("_sbrk() begin\r\n");

	unsigned char* prev_heap_end;

	if (sbrk_heap_end == 0) {
		heap_start = & _ram_heap_start;
		heap_end = & _ram_heap_end;
		sbrk_heap_end = (char*) heap_start;
	}

	prev_heap_end = sbrk_heap_end;

	if((unsigned int)(sbrk_heap_end + incr) >= (unsigned int)heap_end) {
		char str[128];
		snprintf(str, 128, "_sbrk() OUT OF MEM, sbrk_heap_end = %p, heap_end = %p, incr = %u\r\n", sbrk_heap_end, heap_end, incr);
		print(str);
		return ((void*)-1); // error - no more memory
	}

	sbrk_heap_end += incr;

	//char str[128];
	//snprintf(str, 128, "_sbrk() end prev_heap_end = %p, incr = %u, heap_end = %p\r\n", prev_heap_end, incr, heap_end);
	//print(str);

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



