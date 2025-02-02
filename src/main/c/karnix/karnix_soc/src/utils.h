#ifndef	_UTILS_H_
#define _UTILS_H_
#include <stdint.h>
#include <stdio.h>
#include <uart.h>

#define SATURATE8(X)    (X > 255 ? 255 : X < 0 ? 0: X)
#define ABS(X)          ((X) > 0 ? (X) : (-1 * (X)))
#define MIN(X,Y)        ((X) < (Y) ? (X) : (Y))
#define	SWAP32(X)	__builtin_bswap32((X))
#define	CHAR_BIT	8
#define BitsCount( val ) ( sizeof( val ) * CHAR_BIT )
#define Shift( val, steps ) ( steps % BitsCount( val ) )
#define ROL( val, steps ) ( ( val << Shift( val, steps ) ) | ( val >> ( BitsCount( val ) - Shift( val, steps ) ) ) )
#define ROR( val, steps ) ( ( val >> Shift( val, steps ) ) | ( val << ( BitsCount( val ) - Shift( val, steps ) ) ) )


extern unsigned char* sbrk_heap_end; /* Set by init_sbrk() and maintained by _sbrk() */
extern unsigned int end; /* Set by linker.  */
extern unsigned int _bss_start; /* Set by linker.  */
extern unsigned int _bss_end; /* Set by linker.  */
extern unsigned int _ram_heap_start; /* Set by linker.  */
extern unsigned int _ram_heap_end; /* Set by linker.  */
extern unsigned int _stack_start; /* Set by linker.  */
extern unsigned int _stack_size; /* Set by linker.  */
extern unsigned int trap_entry;
extern unsigned int* heap_start; /* programmer defined heap start */
extern unsigned int* heap_end; /* programmer defined heap end */


void init_sbrk(unsigned int* heap, int size);
void delay(uint32_t loops);
void delay_us(uint32_t us);
void print_uart0(const char*str);
void print_uart1(const char*str);
void hard_reboot(void);
void memcpy_rev(void* dst, void* src, uint32_t count);
uint32_t strntoul(const char * buf, int size, int base); // string to unsigned long with size
void printk(const char *fmt, ...);

#define print print_uart0

#endif // _UTILS_H_

