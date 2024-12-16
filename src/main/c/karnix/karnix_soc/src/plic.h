#ifndef _PLIC_H_
#define _PLIC_H_

#include <stdio.h>
#include "soc.h"
#include "utils.h"

typedef struct
{
  volatile uint32_t ENABLE;
  volatile uint32_t PENDING;
  volatile uint32_t IRQLINE;
  volatile uint32_t POLARITY;
  volatile uint32_t IRQLAST;
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

