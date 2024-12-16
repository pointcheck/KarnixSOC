/*
	AUDIO DAC driver based on SpinalHDL 1.4.4 SpiMaster controller

	Refactored by Ruslan Zalata <rz@fabmicro.ru>

	Based on SaxonSoc

*/

#ifndef _AUDIODAC_H_
#define _AUDIODAC_H_

typedef struct {
	volatile uint32_t data; 	// 0x00
	volatile uint32_t status;	// 0x04
	volatile uint32_t config;	// 0x08
	volatile uint32_t divider;	// 0x0C - SPI frequency = FCLK / (2 * clockDivider)
	volatile uint32_t ssSetup;	// 0x10 - time between chip select enable and the next byte
	volatile uint32_t ssHold;	// 0x14 - time between the last byte transmission and the chip select disable
	volatile uint32_t ssDisable;	// 0x18 - time between chip select disable and chip select enable
} AUDIODAC_Reg;


typedef struct {
	uint32_t config;
	uint32_t divider;
	uint32_t ssSetup;
	uint32_t ssHold;
	uint32_t ssDisable;
} AUDIODAC_Config;


#define AUDIO_DAC_STATUS_NONE			0x00000000
#define AUDIO_DAC_STATUS_CMD_INT_ENABLE		0x00000001	// Command fifo empty interrupt enable (R/W)
#define AUDIO_DAC_STATUS_RSP_INT_ENABLE		0x00000002	// Read fifo not empty interrupt enable (R/W)
#define AUDIO_DAC_STATUS_CMD_INT_HALF_ENABLE	0x00000004	// Command fifo is half-empty interrupt enable (R/W)
#define AUDIO_DAC_STATUS_RSP_INT_HALF_ENABLE	0x00000008	// Read fifo falf-full interrupt enable (R/W)
#define AUDIO_DAC_STATUS_CMD_INT		0x00000100	// Command fifo empty interrupt pending (R/W)
#define AUDIO_DAC_STATUS_RSP_INT		0x00000200	// Read fifo not empty interrupt pending (R/W)
#define AUDIO_DAC_STATUS_CMD_HALF_EMPTY_INT	0x00000400	// Command fifo is half-empty interrupt pending (R/W)
#define AUDIO_DAC_STATUS_RSP_HALF_FULL_INT	0x00000800	// Read fifo is half-full interrupt pending (R/W)
#define AUDIO_DAC_STATUS_TX_AVAIL		0xFFFF0000	// Command fifo space availability (SS commands + send byte commands)
#define AUDIO_DAC_DATA_RX			0x0000000F	// R[7:0] rxData read bits in DATA REG
#define	AUDIO_DAC_DATA_RX_OCUPANCY		0x7FFF0000	// R[30:16] rx fifo occupancy (include the rxData in the amount)
#define	AUDIO_DAC_DATA_RX_VALID			0x80000000	// R[31] Inform that read rxData is valid
#define	AUDIO_DAC_CONFIG_CPOL			0x00000001	// CPOL
#define	AUDIO_DAC_CONFIG_CPHA			0x00000002	// CPHA
#define AUDIO_DAC_CONFIG_SS			0xFFFFFFF0	// For each ss, the corresponding bit specify 
								// if that's a active high one.
#define AUDIO_DAC_CMD_SEND			0x00000000
#define	AUDIO_DAC_CMD_SEND_RECEIVE		0x01000000
#define	AUDIO_DAC_CMD_ENABLE_SS0		0x11000001
#define	AUDIO_DAC_CMD_DISABLE_SS0		0x10000001

#define	AUDIODAC0_CHANNEL			0b0010000000000000	// DAC0 is Channel 2

#define	DAC_NOT_ISR	0	// API called from outside of ISR
#define	DAC_ISR		1	// API called from ISR

/*
     *   When you read DATA register it pop an byte of the rx fifo and provide its value (via rxData)
     *   When you write DATA register, it push a command into the fifo. There is the commands that you can use:
     *     0x000000xx =>  Send byte xx
     *     0x010000xx =>  Send byte xx and also push the read data into the FIFO
     *     0x1100000X =>  Enable the SS line X
     *     0x1000000X =>  Disable the SS line X
*/


static inline void audiodac_applyConfig(AUDIODAC_Reg* reg, AUDIODAC_Config *config) {
	reg->config = config->config;
	reg->divider = config->divider;
	reg->ssSetup = config->ssSetup;
	reg->ssHold = config->ssHold;
	reg->ssDisable = config->ssDisable;
}

inline int audiodac_get_tx_avail(AUDIODAC_Reg* reg) {
        return (reg->status & AUDIO_DAC_STATUS_TX_AVAIL) >> 16;
}


void audiodac_init(AUDIODAC_Reg* reg, int sample_rate);
int audiodac_wait_tx_avail(AUDIODAC_Reg* reg, int min_avail, int timeout);
int audiodac_xmit(AUDIODAC_Reg* reg, short* buf, int len, int timeout);
int audiodac0_isr(void);
void audiodac0_start_playback(short *ring_buffer, int ring_buffer_size);
int audiodac0_submit_buffer(short *audio_buffer, int audio_buffer_fill, int isr_flag);

extern short *audiodac0_tx_ring_buffer; 
extern int audiodac0_tx_ring_buffer_size;
extern int audiodac0_tx_ring_buffer_playback_ptr;
extern int audiodac0_tx_ring_buffer_fill_ptr;

#endif // _AUDIODAC_H_

