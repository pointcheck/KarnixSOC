/*
	SPI driver based on SpinalHDL 1.4.4 SpiMaster controller

	Refactored by Ruslan Zalata <rz@fabmicro.ru>

	Based on SaxonSoc

*/

#ifndef _SPI_H_
#define _SPI_H_

typedef struct {
	volatile uint32_t data; 	// 0x00
	volatile uint32_t status;	// 0x04
	volatile uint32_t config;	// 0x08
	volatile uint32_t divider;	// 0x0C - SPI frequency = FCLK / (2 * clockDivider)
	volatile uint32_t ssSetup;	// 0x10 - time between chip select enable and the next byte
	volatile uint32_t ssHold;	// 0x14 - time between the last byte transmission and the chip select disable
	volatile uint32_t ssDisable;	// 0x18 - time between chip select disable and chip select enable
	volatile uint32_t rxoccupancy;	// 0x1c - num of bytes pending in RX fifo
} SPI_Reg;

typedef struct {
	uint32_t config;
	uint32_t divider;
	uint32_t ssSetup;
	uint32_t ssHold;
	uint32_t ssDisable;
} SPI_Config;

#define SPI0	((SPI_Reg*)(0xF00C1000))

#define SPI_STATUS_NONE			0x00000000
#define SPI_STATUS_CMD_INT_ENABLE	0x00000001	// Command fifo empty interrupt enable (R/W)
#define SPI_STATUS_RSP_INT_ENABLE	0x00000002	// Read fifo not empty interrupt enable (R/W)
#define SPI_STATUS_CMD_INT_HALF_ENABLE	0x00000004	// Command fifo is half-empty interrupt enable (R/W)
#define SPI_STATUS_RSP_INT_HALF_ENABLE	0x00000008	// Read fifo falf-full interrupt enable (R/W)
#define SPI_STATUS_CMD_INT		0x00000100	// Command fifo empty interrupt pending (R/W)
#define SPI_STATUS_RSP_INT		0x00000200	// Read fifo not empty interrupt pending (R/W)
#define SPI_STATUS_CMD_HALF_EMPTY_INT	0x00000400	// Command fifo is half-empty interrupt pending (R/W)
#define SPI_STATUS_RSP_HALF_FULL_INT	0x00000800	// Read fifo is half-full interrupt pending (R/W)
#define SPI_STATUS_TX_AVAIL		0xFFFF0000	// Command fifo space availability (SS commands + send byte commands)
#define SPI_DATA_RX			0x0000000F	// R[7:0] rxData read bits in DATA REG
#define	SPI_DATA_RX_OCUPANCY		0x7FFF0000	// R[30:16] rx fifo occupancy (include the rxData in the amount)
#define	SPI_DATA_RX_VALID		0x80000000	// R[31] Inform that read rxData is valid
#define	SPI_CONFIG_CPOL			0x00000001	// CPOL
#define	SPI_CONFIG_CPHA			0x00000002	// CPHA
#define SPI_CONFIG_SS			0xFFFFFFF0	// For each ss, the corresponding bit specify 
							// if that's a active high one.
#define SPI_CMD_SEND			0x00000000
#define	SPI_CMD_SEND_RECEIVE		0x01000000
#define	SPI_CMD_ENABLE_SS		0x11000000
#define	SPI_CMD_DISABLE_SS		0x10000000
#define	SPI_CMD_ENABLE_SS0		0x11000001
#define	SPI_CMD_DISABLE_SS0		0x10000001
#define	SPI_CMD_ENABLE_SS1		0x11000002
#define	SPI_CMD_DISABLE_SS1		0x10000002

#define	SPI_SS0				0
#define	SPI_SS1				1
#define	SPI_SS2				2
#define	SPI_SS3				3

/*
     *   When you read DATA register it pop an byte of the rx fifo and provide its value (via rxData)
     *   When you write DATA register, it push a command into the fifo. There is the commands that you can use:
     *     0x000000xx =>  Send byte xx
     *     0x010000xx =>  Send byte xx and also push the read data into the FIFO
     *     0x1100000X =>  Enable the SS line X
     *     0x1000000X =>  Disable the SS line X
*/


static inline void spi_applyConfig(SPI_Reg* reg, SPI_Config *config) {
	reg->config = config->config;
	reg->divider = config->divider;
	reg->ssSetup = config->ssSetup;
	reg->ssHold = config->ssHold;
	reg->ssDisable = config->ssDisable;
}

static inline int spi_get_tx_avail(SPI_Reg* reg) {
        return (reg->status & SPI_STATUS_TX_AVAIL) >> 16;
}

static inline int spi_get_rx_occupancy(SPI_Reg* reg) {
        return reg->rxoccupancy;
}



int spi_wait_tx_avail(SPI_Reg* reg, int min_avail, int timeout);
int spi_xmit(SPI_Reg* reg, int ss, unsigned short* txbuf, unsigned short* rxbuf, int len);


#endif // _SPI_H_

