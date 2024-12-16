#ifndef _UART_H_
#define _UART_H_

typedef struct
{
  volatile uint32_t DATA;
  volatile uint32_t STATUS;
  volatile uint32_t CLOCK_DIVIDER;
  volatile uint32_t FRAME_CONFIG;
} Uart_Reg;

#define	UART_STATUS_TX_IRQ_EN	(1<<0)
#define	UART_STATUS_RX_IRQ_EN	(1<<1)
#define	UART_STATUS_TX_IRQ_PEN	(1<<8)
#define	UART_STATUS_RX_IRQ_PEN	(1<<9)
#define	UART_STATUS_TX_VALID	(1<<15)
#define	UART_STATUS_TX_FIFO_OCC	(0xff<<16)
#define	UART_STATUS_RX_FIFO_OCC	(0xff<<24)

#define	UART_PRE_SAMPLING_SIZE	1
#define	UART_SAMPLING_SIZE	3
#define	UART_POST_SAMPLING_SIZE	1


#define	UART_PARITY_NONE	0
#define	UART_PARITY_EVEN	1
#define	UART_PARITY_ODD		2

#define	UART_STOP_ONE		0
#define	UART_STOP_TWO		1

#define	UART_DATA_5		4
#define	UART_DATA_6		5
#define	UART_DATA_7		6
#define	UART_DATA_8		7
#define	UART_DATA_9		8


typedef struct {
	uint32_t dataLength;
	uint32_t clockDivider;
	uint8_t parity;
	uint8_t stop;
} Uart_Config;

static uint32_t uart_writeAvailability(Uart_Reg *reg){
	return (reg->STATUS >> 16) & 0xFF;
}
static uint32_t uart_readOccupancy(Uart_Reg *reg){
	return reg->STATUS >> 24;
}

static void uart_write(Uart_Reg *reg, uint32_t data){
	while(uart_writeAvailability(reg) == 0);
	reg->DATA = data;
}

static void uart_applyConfig(Uart_Reg *reg, Uart_Config *config){
	reg->CLOCK_DIVIDER = config->clockDivider;
	reg->FRAME_CONFIG = ((config->dataLength-1) << 0) | (config->parity << 8) | (config->stop << 16);
}


#endif /* _UART_H_ */


