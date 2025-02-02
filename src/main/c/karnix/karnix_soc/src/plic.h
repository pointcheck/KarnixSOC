#ifndef _PLIC_H_
#define _PLIC_H_

#include <stdio.h>
#include "soc.h"
#include "utils.h"

typedef struct
{
  volatile uint32_t ENABLE;	// If "1" - IRQ line is enabled
  volatile uint32_t PENDING;	// If "1" - Line is pending IRQ
  volatile uint32_t IRQLINE;	// Current IRQ lines state
  volatile uint32_t POLARITY;	// If "1" and edge control enabled - Raising Edge, "0" - Falling Edge 
  volatile uint32_t IRQLAST;	// IRQ lines state one clock cycle ago
  volatile uint32_t EDGE;	// If "1" - control edge change (falling/raising), "0" - control level state
} PLIC_Reg;


#define	PLIC_IRQ_UART0		(1 << 0)
#define	PLIC_IRQ_UART1		(1 << 1)
#define	PLIC_IRQ_MAC		(1 << 2)
#define	PLIC_IRQ_TIMER0		(1 << 3)
#define	PLIC_IRQ_TIMER1		(1 << 4)
#define	PLIC_IRQ_I2C		(1 << 5)
#define	PLIC_IRQ_AUDIODAC0	(1 << 6)	
#define	PLIC_IRQ_CGA_VBLANK	(1 << 7)

void plic_print_stats(void);

#endif /* _PLIC_H_ */

