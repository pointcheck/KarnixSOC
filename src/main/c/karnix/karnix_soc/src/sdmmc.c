/*
	Implementation of simple driver for accessing SD/MMC cards over SPI.

	Depends on SPI controller/driver from SpinalHDL.

	Written by Ruslan Zalata <rz@fabmicro.ru>.

	Copyright (C) 2026, Fabmicro, LLC. Tyumen, Russia.

	SPDX-License-Identifier: BSD-2-Clause
*/

#include "sdmmc.h"
#include "utils.h"

//#define	SDMMC_NO_DEBUG		1	// Disabled debug output (reduces code size)
#define	SDMMC_SUPPORT_CRC16	1	// Calculate CRC16 for in and out data
//#define	SDMMC_ENABLE_TX_AVAIL	1	// Check TX FIFO each time byte is written
//#define	SDMMC_ENABLE_PREERASE		1	// Enable Pre-erase blocks on write

#ifndef sdmmc_printf
#ifdef SDMMC_NO_DEBUG
#define	sdmmc_printf(...) { } 
#else
#define	sdmmc_printf(...) printf(__VA_ARGS__)
#endif
#endif

const struct sdmmc_iface_info sdmmc_ifaces[SDMMC_IFACES] = {
	{
		.ss = 0,
		.reg = SPI0,
	},
	{
		.ss = 0,
		.reg = SPI1,
	},
};

struct sdmmc_card_info sdmmc_cards[SDMMC_IFACES] = { 0 };

const char* sdmmc_types[5] = { "NONE", "SDC_V1", "SDC_V2", "SDC_V2HC", "MMC_V3" };

#ifdef SDMMC_SUPPORT_CRC16
const uint16_t sdmmc_crc16_table[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};


//Not used, CRC calculation is inlined in I/O
uint16_t sdmmc_crc16(uint8_t* buf, int len) {
	uint16_t crc16 = 0;
	for (int i = 0; i < len; i++)
		crc16 = sdmmc_crc16_table[((crc16 >> 8) ^ buf[i]) & 0xff] ^ (crc16 << 8);
	return crc16;
}


// Not used, just for reference
void sdmmc_crc16_init_table(void) {
	uint16_t polynomial = 0x1021;
	uint16_t mask = (1 << (16 - 1));

	for(int i = 0; i < 256; i++) {
		uint16_t crc = i << 8;
		for(uint8_t bit = 0; bit < 8; bit++) {
		if(crc & mask)
			crc = (crc << 1) ^ polynomial;
		else
			crc <<= 1;
        }
        ((uint16_t*)sdmmc_crc16_table)[i] = crc;
    }
}
#endif


uint32_t sdmmc_get_num_blocks(uint8_t csd[]) {
	uint8_t n;
	uint16_t csize;  					    
	if((csd[0] & 0xc0) == 0x40)	{ //V2.00 card	
		csize = ((uint16_t)csd[8] << 8) + csd[9] + 1;
		return csize << 10; // get the number of blocks	 		   
	} else {//V1.XX card
		n = (csd[5] & 0x0f) + ((csd[10] & 0x80) >> 7) + ((csd[9] & 0x03) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 0x03) << 10) + 1;
		return (uint32_t)csize << (n - 9); // get the number of blocks   
	}
}


int sdmmc_wait_ready(SPI_Reg* reg) {
	int rx;

	// Make first transmission
	reg->data = SPI_CMD_SEND_RECEIVE | 0xff;

	for(int i = 0; i < SDMMC_TIMEOUT; i++) {
		rx = reg->data;

		// Wait for data valid
		if(rx & SPI_DATA_RX_VALID) {
			// Some data received 
			if((rx & 0xff) == 0xff)
				return SDMMC_ERROR_OK;
			// Make another transmission
			reg->data = SPI_CMD_SEND_RECEIVE | 0xff;
		}
	}

	return SDMMC_ERROR_TIMEOUT;
}


// On error retuns negative value, otherwise returns positive read byte value
int sdmmc_readwrite_byte(SPI_Reg* reg, uint8_t tx_byte) {
	
	reg->data = SPI_CMD_SEND_RECEIVE | tx_byte;

	for(int i = 0; i < SDMMC_TIMEOUT; i++) {
		int rx = reg->data;
		if(rx & SPI_DATA_RX_VALID)
			return rx & 0xff;
	}

	return SDMMC_ERROR_TIMEOUT;
}


static inline void sdmmc_select(SPI_Reg* reg, int ss) {
	reg->data = SPI_CMD_ENABLE_SS | ss; // make CS=0
}


static inline void sdmmc_deselect(SPI_Reg* reg, int ss) {
	reg->data = SPI_CMD_DISABLE_SS | ss; // make CS=1
	sdmmc_readwrite_byte(reg, 0xff);
}


// Get response from the card
int sdmmc_get_r1_response(SPI_Reg* reg) {

	int rx;

	for(int i = 0; i < SDMMC_TIMEOUT; i++) {

		reg->data = SPI_CMD_SEND_RECEIVE | 0xff; // transmit one byte

		// Wait for RX byte. This may hang if SPI controller is broken !!!
		while(((rx = reg->data) & SPI_DATA_RX_VALID) == 0);

		if((rx & 0x80) == 0) // 7 bit should be zero for response byte
			return rx & 0xff;

	}

	return SDMMC_ERROR_NO_RESP;
}


// Get response from the card
int sdmmc_wait_r1_response(SPI_Reg* reg, uint8_t r1, int retry) {

	int rx;

	while(retry--) {

		for(int i = 0; i < SDMMC_TIMEOUT; i++) {

			reg->data = SPI_CMD_SEND_RECEIVE | 0xff; // transmit one byte

			// Wait for RX byte. This may hang if SPI controller is broken !!!
			while(((rx = reg->data) & SPI_DATA_RX_VALID) == 0);

			if((rx & 0xff) == r1)
				return SDMMC_ERROR_OK;

		}
	}

	return SDMMC_ERROR_NO_RESP;
}


// Read len bytes from card into buf
int sdmmc_rx_buf(SPI_Reg* reg, uint8_t *buf, uint16_t len) {

	int ret = sdmmc_wait_r1_response(reg, SDMMC_TOKEN_DATA1, 1000);

	if(ret < 0) {
		sdmmc_printf("%s: wait token fail, ret = %d\r\n", "sdmmc_rx_buf", ret);  
		return ret;
	}

	int count = len + 2; // include two CRC16 bytes
	uint8_t *b = buf;

	uint16_t crc16_my = 0; // accumulated CRC16

	// transmit first byte
	reg->data = SPI_CMD_SEND_RECEIVE | 0xff;

	while(count--) { 
		
		// This loop can possible hang if SPI controller is broken 

		while(1) {

			int rx = reg->data; // read from RX FIFO

			if(rx & SPI_DATA_RX_VALID) { // if we have valid data

				// transmit next byte upfront
				reg->data = SPI_CMD_SEND_RECEIVE | 0xff;

				// save current data byte 
				*(b++) = rx;

				#ifdef SDMMC_SUPPORT_CRC16
				// calculate intermediate CRC16 value
				crc16_my = sdmmc_crc16_table[((crc16_my >> 8) ^ rx) & 0xff] ^ (crc16_my << 8);
				#endif
				break;
			}

		}
	}

	// read one extra byte from RX FIFO
	volatile int tmp = reg->data;

	#ifdef SDMMC_SUPPORT_CRC16
	// Note that we accumulated CRC including two extra bytes of his CRC16 data,
	// as a result accumulated sum should always be equal to zero!!!
	if(crc16_my != 0x0000) {
		uint16_t crc16_his = (b[-2] << 8) | b[-1];

		sdmmc_printf("%s: accumulated CRC is not zero: 0x%04X (his: 0x%04X)\r\n",
			"sdmmc_rx_buf", crc16_my, crc16_his);

		return SDMMC_ERROR_CRC;
	}
	#endif

	return SDMMC_ERROR_OK;
}


// Write len bytes to card from buf using cmd
int sdmmc_tx_buf(SPI_Reg* reg, uint8_t *buf, uint16_t len, uint8_t cmd) {

	int ret = sdmmc_wait_ready(reg);

	if(ret < 0) {
		sdmmc_printf("%s: wait ready fail, ret = %d\r\n", "sdmmc_tx_buf", ret);  
		return ret;
	}

	int count = len; // usually 512 bytes
	uint8_t *b = buf;

	volatile uint16_t crc16_my = 0; // accumulated CRC16

 	sdmmc_readwrite_byte(reg, cmd); // Write command provided by user 

	while(count--) { 

		// We assume that transmitting a single byte over SPI takes less time than
		// calculating CRC16 word, so we do not need to wait for TX FIFO readiness.
		// Otherwise, we need to call spi_wait_tx_avail() to check if there's space
		// for another byte to put in. If that is the case, enable SDMMC_ENABLE_TX_AVAIL.
		// Note, checking FIFO takes time, hence effects writing throughput significantly!
 
		#ifdef SDMMC_ENABLE_TX_AVAIL
		if(spi_wait_tx_avail(reg, 1, 1000) == 0) {
			sdmmc_printf("%s: tx avail fail, ret = %d\r\n", "sdmmc_tx_buf", ret);  
			return SDMMC_ERROR_WRITE;
		}
		#endif

		uint8_t tx = *b++;

		reg->data = SPI_CMD_SEND | tx;

		#ifdef SDMMC_SUPPORT_CRC16
		// calculate intermediate CRC16 value
		crc16_my = sdmmc_crc16_table[((crc16_my >> 8) ^ tx) & 0xff] ^ (crc16_my << 8);
		#endif
	}

	// Send CRC16

	if(spi_wait_tx_avail(reg, 2, 1000) == 0) {
		sdmmc_printf("%s: tx avail fail, ret = %d\r\n", "sdmmc_tx_buf", ret);  
		return SDMMC_ERROR_WRITE;
	}

	reg->data = SPI_CMD_SEND | (crc16_my >> 8);
	reg->data = SPI_CMD_SEND | (crc16_my & 0xff);
 
	// Wait for response (may take very long time !!!) 
	for(int i = 0; i < SDMMC_TIMEOUT << 6; i++) {

		ret = sdmmc_readwrite_byte(reg, 0xff);
		if(ret < 0) {
			sdmmc_printf("%s: wait resp fail, ret = %d\r\n", "sdmmc_tx_buf", ret);  
			return ret;
		}

		if((ret & 0x1f) == 0x05) {
			return SDMMC_ERROR_OK;
		}
	}

	sdmmc_printf("%s: wait 0x05 timeout, ret = %d\r\n", "sdmmc_tx_buf", ret);  

	return SDMMC_ERROR_TIMEOUT;
}


int sdmmc_send_cmd(SPI_Reg* reg, uint8_t cmd, uint32_t arg, uint8_t crc, uint32_t resp_delay_us) {

	int ret;

	sdmmc_readwrite_byte(reg, cmd | 0x40); // Write CMD
	sdmmc_readwrite_byte(reg, (arg >> 24) & 0xff); // Write arg 
	sdmmc_readwrite_byte(reg, (arg >> 16) & 0xff);
	sdmmc_readwrite_byte(reg, (arg >>  8) & 0xff);
	sdmmc_readwrite_byte(reg, (arg >>  0) & 0xff);
	sdmmc_readwrite_byte(reg, crc);  
	
	// Some implementations use below hack for CMD12 without explanations.
	/*
	if(cmd == SDMMC_CMD_CMD12) // Special case for CMD12
		sdmmc_readwrite_byte(reg, 0xff); // one more dummy byte 
	*/

	// Perform delay before reading response
	if(resp_delay_us)
		delay_us(resp_delay_us);

	// Wait for responses
	ret = sdmmc_get_r1_response(reg);

	return ret;
}


// Send command wrapping in select/deselect
int sdmmc_send_cmd_sel(SPI_Reg* reg, int ss, uint8_t cmd, uint32_t arg, uint8_t crc, uint32_t resp_delay_us) {

	int ret;

	sdmmc_select(reg, ss);

	ret = sdmmc_send_cmd(reg, cmd, arg, crc, resp_delay_us);

	sdmmc_deselect(reg, ss);

	return ret;
}


// Read 16 bytes of SD/MMC information block (CID, CSD) into info_buf16
int sdmmc_get_info(SPI_Reg* reg, int ss, uint8_t cmd, uint8_t *info_buf16) {
	int ret;

	sdmmc_select(reg, ss);
	
	if((ret = sdmmc_send_cmd(reg, cmd, 0, 0x01, 0)) < 0) {
		sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_get_info", cmd, ret);
		goto end;
	}

	sdmmc_printf("%s: cmd %d resp = %d\r\n", "sdmmc_get_info", cmd, ret);

	if((ret = sdmmc_rx_buf(reg, info_buf16, 16)) < 0) {
		sdmmc_printf("%s: rx_buf failed, ret = %d\r\n", "sdmmc_get_info", ret);
		goto end;
	}

	end:

	sdmmc_deselect(reg, ss);

	return ret;
}


int sdmmc_init(int iface) {
	int ret;

	if(iface >= SDMMC_IFACES) {
		sdmmc_printf("%s: iface %d should be < %d\r\n", "sdmmc_init",
			iface, SDMMC_IFACES);
		return SDMMC_ERROR_PARAMS;
	}

	SPI_Reg* reg = sdmmc_ifaces[iface].reg;
	int ss = sdmmc_ifaces[iface].ss;

	// Clear old card data

	memset(&sdmmc_cards[iface], 0, sizeof(sdmmc_cards[0]));

	SPI_Config spi_cfg;
	
	spi_cfg.config = SPI_CONFIG_CPHA | SPI_CONFIG_CPOL;
	spi_cfg.ssSetup = 16;
	spi_cfg.ssHold = 16;
	spi_cfg.ssDisable = 16;
	spi_cfg.divider = SYSTEM_CLOCK_HZ/(2*SDMMC_SPI_BITRATE_INIT);
	spi_applyConfig(reg, &spi_cfg);

	sdmmc_printf("%s: set divider for %dHz = %d\r\n", "sdmmc_init",
		SDMMC_SPI_BITRATE_INIT, spi_cfg.divider);

	spi_clear_rx_fifo(reg);

	// Send at least 74 clocks with CS=1 for SD/MMC card to Power ON 

	sdmmc_deselect(reg, ss);

	for(int i = 0; i < 10; i++)
		sdmmc_readwrite_byte(reg, 0xff);

	// Send CMD0 - Card Reset command

	for(int retry = 0; retry < SDMMC_RETRIES; retry++) {

		if((ret = sdmmc_send_cmd_sel(reg, ss, SDMMC_CMD_CMD0, 0, 0x95, 100000)) < 0) { // hard-coded CRC=0x95
			sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_init", SDMMC_CMD_CMD0, ret);
			goto end;
		}
	
		if(ret == 0x01) // r1 == 0x01 - card is in idle state
			break;
	}

	if(ret != 0x01) {
		sdmmc_printf("%s: cmd %d resp = %x\r\n", "sdmmc_init", SDMMC_CMD_CMD0, ret);
		ret = SDMMC_ERROR_INIT_FAIL;
		goto end;
	}
	

	// Send CMD8 - Check for SDC V2

	for(int retry = 0; retry < SDMMC_RETRIES; retry++) {

		sdmmc_select(reg, ss);

		if((ret = sdmmc_send_cmd(reg, SDMMC_CMD_CMD8, 0x01aa, 0x87, 0)) < 0) { // hard-coded CRC=0x87
			sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_init", SDMMC_CMD_CMD0, ret);
			goto end;
		}

		sdmmc_printf("%s: cmd %d resp = %x\r\n", "sdmmc_init", SDMMC_CMD_CMD8, ret);
		
		if(ret < 0x7f)
			break;

		sdmmc_deselect(reg, ss);

		delay_us(100000); // Let card complete initialization
	}

	if(ret == 0x01) {  // SDC V2 init
		// Read trainling bytes of R7 response
		sdmmc_readwrite_byte(reg, 0xff);
		sdmmc_readwrite_byte(reg, 0xff);
		ret  = sdmmc_readwrite_byte(reg, 0xff) << 8;
		ret |= sdmmc_readwrite_byte(reg, 0xff);


		sdmmc_deselect(reg, ss);

		sdmmc_printf("%s: r7 = %x\r\n", "sdmmc_init", ret);
		

		// Init V2 card

		for(int retry = 0; retry < SDMMC_RETRIES*10; retry++) {

			// Send CMD55
	
			if((ret = sdmmc_send_cmd_sel(reg, ss, SDMMC_CMD_CMD55, 0x0, 0x01, 100000)) < 0) {
				sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_init", SDMMC_CMD_CMD55, ret);
				goto end;
			}
	
			sdmmc_printf("%s: cmd %d resp = %x\r\n", "sdmmc_init", SDMMC_CMD_CMD55, ret);
	
	
			// Send CMD41
	
			if((ret = sdmmc_send_cmd_sel(reg, ss, SDMMC_CMD_CMD41, 0x40000000, 0x01, 0)) < 0) { // HCS bit set
				sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_init", SDMMC_CMD_CMD41, ret);
				goto end;
			}
	
			sdmmc_printf("%s: cmd %d resp = %x\r\n", "sdmmc_init", SDMMC_CMD_CMD41, ret);

			if(ret == 0)
				break;
		}

		if(ret != 0) {
			sdmmc_printf("%s: V2 init failed\r\n", "sdmmc_init");
			goto end;
		}

		// Send CMD58 - Get OCR value

		sdmmc_select(reg, ss);
	
		if((ret = sdmmc_send_cmd(reg, SDMMC_CMD_CMD58, 0x0, 0x01, 0)) < 0) {
			sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_init", SDMMC_CMD_CMD58, ret);
			goto end;
		}
	
		sdmmc_printf("%s: cmd %d resp = %x\r\n", "sdmmc_init", SDMMC_CMD_CMD58, ret);
	
		if(ret != 0) {
			sdmmc_printf("%s: %s init failed\r\n", "sdmmc_init", sdmmc_types[SDMMC_TYPE_SDC_V2]);
			goto end;
		}

		// Read trailing OCR
		ret  = sdmmc_readwrite_byte(reg, 0xff) << 24;
		ret |= sdmmc_readwrite_byte(reg, 0xff) << 16;
		ret |= sdmmc_readwrite_byte(reg, 0xff) << 8;
		ret |= sdmmc_readwrite_byte(reg, 0xff);

		if(ret & 0x40000000)
			sdmmc_cards[iface].type = SDMMC_TYPE_SDC_V2HC;
		else
			sdmmc_cards[iface].type = SDMMC_TYPE_SDC_V2;

		sdmmc_cards[iface].ocr = ret;

		sdmmc_deselect(reg, ss);
	
		sdmmc_printf("%s: %s OCR = 0x%x\r\n", "sdmmc_init", sdmmc_types[SDMMC_TYPE_SDC_V2], ret);

	} else { // SDC V1.x or MMC V3

		sdmmc_deselect(reg, ss);


		// Send CMD55

		if((ret = sdmmc_send_cmd_sel(reg, ss, SDMMC_CMD_CMD55, 0x0, 0x01, 0)) < 0) {
			sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_init", SDMMC_CMD_CMD55, ret);
			goto end;
		}

		sdmmc_printf("%s: cmd %d resp = %x\r\n", "sdmmc_init", SDMMC_CMD_CMD55, ret);
	
		// Send CMD41

		if((ret = sdmmc_send_cmd_sel(reg, ss, SDMMC_CMD_CMD41, 0x0, 0x01, 0)) < 0) { // HCS bit set
			sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_init", SDMMC_CMD_CMD41, ret);
			goto end;
		}

		sdmmc_printf("%s: cmd %d resp = %x\r\n", "sdmmc_init", SDMMC_CMD_CMD41, ret);

		if(ret == 0) {
			// Init V1

			for(int retry = 0; retry < SDMMC_RETRIES; retry++) {
	
				// Send CMD55
		
				if((ret = sdmmc_send_cmd_sel(reg, ss, SDMMC_CMD_CMD55, 0x0, 0x01, 0)) < 0) {
					sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_init", SDMMC_CMD_CMD55, ret);
					goto end;
				}
		
				sdmmc_printf("%s: cmd %d resp = %x\r\n", "sdmmc_init", SDMMC_CMD_CMD55, ret);
		
		
				// Send CMD41
		
				if((ret = sdmmc_send_cmd_sel(reg, ss, SDMMC_CMD_CMD41, 0x0, 0x01, 0)) < 0) { // HCS bit set
					sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_init", SDMMC_CMD_CMD41, ret);
					goto end;
				}
		
				sdmmc_printf("%s: cmd %d resp = %x\r\n", "sdmmc_init", SDMMC_CMD_CMD41, ret);
	
				if(ret == 0)
					break;
			}

			if(ret == 0) {
				sdmmc_cards[iface].type = SDMMC_TYPE_SDC_V1;
			} else {
				sdmmc_printf("%s: %s init failed\r\n", "sdmmc_init", sdmmc_types[SDMMC_TYPE_SDC_V1]);
			}

		} else {
			// Init MMC V3

			for(int retry = 0; retry < SDMMC_RETRIES; retry++) {
	
				// Send CMD1
		
				if((ret = sdmmc_send_cmd_sel(reg, ss, SDMMC_CMD_CMD1, 0x0, 0x01, 0)) < 0) {
					sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_init", SDMMC_CMD_CMD1, ret);
					goto end;
				}
		
				sdmmc_printf("%s: cmd %d resp = %x\r\n", "sdmmc_init", SDMMC_CMD_CMD1, ret);

				if(ret == 0)
					break;
			}

			if(ret == 0) {
				sdmmc_cards[iface].type = SDMMC_TYPE_MMC_V3;
			} else {
				sdmmc_printf("%s: %s init failed\r\n", "sdmmc_init", sdmmc_types[SDMMC_TYPE_MMC_V3]);
			}
		}
	}


	// Send CMD16 - Set sector size to SDMMC_BLOCK_SIZE bytes

	sdmmc_send_cmd_sel(reg, ss, SDMMC_CMD_CMD16, SDMMC_BLOCK_SIZE, 0x01, 0);

	// Get CID (manufacturer)
	if((ret = sdmmc_get_info(reg, ss, SDMMC_CMD_CMD10, sdmmc_cards[iface].cid_data)) < 0) {
		sdmmc_printf("%s: cid failed, ret = %d\r\n", "sdmmc_init", ret);
		goto end;
	}

	// Get CSD (geometry)
	if((ret = sdmmc_get_info(reg, ss, SDMMC_CMD_CMD9, sdmmc_cards[iface].csd_data)) < 0) {
		sdmmc_printf("%s: csd failed, ret = %d\r\n", "sdmmc_init", ret);
		goto end;
	}

	sdmmc_cards[iface].blocks = sdmmc_get_num_blocks(sdmmc_cards[iface].csd_data);

	// Initialization completed OK, switch to higher bitrate

	spi_cfg.config = SPI_CONFIG_CPHA | SPI_CONFIG_CPOL;
	//spi_cfg.config = SPI_CONFIG_CPHA;
	spi_cfg.ssSetup = 16;
	spi_cfg.ssHold = 16;
	spi_cfg.ssDisable = 16;
	spi_cfg.divider = SYSTEM_CLOCK_HZ/(2*SDMMC_SPI_BITRATE_NORMAL);
	spi_applyConfig(reg, &spi_cfg);

	sdmmc_printf("%s: set divider for %dHz = %d\r\n", "sdmmc_init",
		SDMMC_SPI_BITRATE_NORMAL, spi_cfg.divider);

	end:

	sdmmc_deselect(reg, ss);

	sdmmc_printf("%s: card type = %d (%s), blocks = %d (%d MiB)\r\n", "sdmmc_init",
		sdmmc_cards[iface].type, sdmmc_types[sdmmc_cards[iface].type],
		sdmmc_cards[iface].blocks, (sdmmc_cards[iface].blocks / 1024) * 512 / 1024
	);

	return ret;
}    


int sdmmc_read_block(int iface, int block_num, int count, uint8_t* buf) {
	int ret = SDMMC_ERROR_OK;

	if(iface >= SDMMC_IFACES) {
		sdmmc_printf("%s: iface %d should be < %d\r\n", "sdmmc_read_block",
			iface, SDMMC_IFACES);
		return SDMMC_ERROR_PARAMS;
	}

	if(count < 1) {
		sdmmc_printf("%s: count %d should be > 0\r\n", "sdmmc_read_block",
			count, SDMMC_IFACES);
		return SDMMC_ERROR_PARAMS;
	}

	if(buf == NULL) {
		sdmmc_printf("%s: buf is NULL!\r\n", "sdmmc_read_block",
			SDMMC_IFACES);
		return SDMMC_ERROR_PARAMS;
	}

	if(sdmmc_cards[iface].type == SDMMC_TYPE_NONE) {
		sdmmc_printf("%s: iface %d is not ready!\r\n", "sdmmc_read_block", iface);
		return SDMMC_ERROR_NOT_READY;
	}

	SPI_Reg* reg = sdmmc_ifaces[iface].reg;
	int ss = sdmmc_ifaces[iface].ss;

	if(sdmmc_cards[iface].type != SDMMC_TYPE_SDC_V2HC)
		block_num *= SDMMC_BLOCK_SIZE; // convert to address = block * SDMMC_BLOCK_SIZE 

	spi_clear_rx_fifo(reg);

	int c;
	uint8_t* b;

	for(int retry = 0; retry < SDMMC_RETRIES; retry++) {

		c = count;
		b = buf;
 
		sdmmc_select(reg, ss);

		if(c < 2) {

			// Send CMD17 - Read One Sector
		
			if((ret = sdmmc_send_cmd(reg, SDMMC_CMD_CMD17, block_num, 0x01, 0)) < 0) {
				sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_read_block", SDMMC_CMD_CMD17, ret);
				goto again;
			}

			if(ret != 0x00) {
				sdmmc_printf("%s: cmd %d resp = %d\r\n", "sdmmc_read_block", SDMMC_CMD_CMD17, ret);
				goto again;
			}

			if((ret = sdmmc_rx_buf(reg, b, SDMMC_BLOCK_SIZE)) != 0) {
				sdmmc_printf("%s: rx_buf failed at %d, retry = %d, ret = %d\r\n", "sdmmc_read_block", count - c - 1, retry, ret);
				goto again;
			}

		} else {
			// Send MD18 - Read Many Sectors (Continuous Read)
		
			if((ret = sdmmc_send_cmd(reg, SDMMC_CMD_CMD18, block_num, 0x01, 0)) < 0) {
				sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_read_block", SDMMC_CMD_CMD18, ret);
				goto again;
			}

			if(ret != 0x00) {
				sdmmc_printf("%s: cmd %d resp = %d\r\n", "sdmmc_read_block", SDMMC_CMD_CMD18, ret);
				goto again;
			}

			while(c--) {
				if((ret = sdmmc_rx_buf(reg, b, SDMMC_BLOCK_SIZE)) != 0) {
					sdmmc_printf("%s: rx_buf failed at %d, retry = %d, ret = %d\r\n", "sdmmc_read_block", count - c - 1, retry, ret);
					goto again;
				}
				b += SDMMC_BLOCK_SIZE;
			}

			// Send CMD12 - Stop Data Transaction 
		
			if((ret = sdmmc_send_cmd(reg, SDMMC_CMD_CMD12, block_num, 0x01, 0)) < 0) {
				sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_read_block", SDMMC_CMD_CMD12, ret);
				goto again;
			}

			// Wait card ready (busy: MISO = 0, ready: MISO = 1)
			ret = sdmmc_wait_ready(reg);
		}

		sdmmc_deselect(reg, ss);

		break;

		again:

		// Send CMD12 - Stop Data Transaction 
		
		if((ret = sdmmc_send_cmd(reg, SDMMC_CMD_CMD12, block_num, 0x01, 0)) < 0) {
			sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_read_block", SDMMC_CMD_CMD12, ret);
			break;
		}

		sdmmc_printf("%s: retry = %d of %d failed\r\n", "sdmmc_read_block", retry, SDMMC_RETRIES);

		sdmmc_deselect(reg, ss);

	} // retry

	return ret;
}


int sdmmc_write_block(int iface, int block_num, int count, uint8_t* buf) {
	int ret = SDMMC_ERROR_OK;

	if(iface >= SDMMC_IFACES) {
		sdmmc_printf("%s: iface %d should be < %d\r\n", "sdmmc_write_block",
			iface, SDMMC_IFACES);
		return SDMMC_ERROR_PARAMS;
	}

	if(count < 1) {
		sdmmc_printf("%s: count %d should be > 0\r\n", "sdmmc_write_block",
			iface, SDMMC_IFACES);
		return SDMMC_ERROR_PARAMS;
	}

	if(buf == NULL) {
		sdmmc_printf("%s: buf is NULL!\r\n", "sdmmc_write_block",
			SDMMC_IFACES);
		return SDMMC_ERROR_PARAMS;
	}

	if(sdmmc_cards[iface].type == SDMMC_TYPE_NONE) {
		sdmmc_printf("%s: iface %d is not ready!\r\n", "sdmmc_write_block", iface);
		return SDMMC_ERROR_NOT_READY;
	}

	SPI_Reg* reg = sdmmc_ifaces[iface].reg;
	int ss = sdmmc_ifaces[iface].ss;


	if(sdmmc_cards[iface].type != SDMMC_TYPE_SDC_V2HC)
		block_num *= SDMMC_BLOCK_SIZE; // convert to address = block * SDMMC_BLOCK_SIZE 

	spi_clear_rx_fifo(reg);

	int c;
	uint8_t* b;

	for(int retry = 0; retry < SDMMC_RETRIES; retry++) {

		c = count;
		b = buf;

		sdmmc_select(reg, ss);

		if(c < 2) {

			// Send CMD24 - Write One Sector

			if((ret = sdmmc_send_cmd(reg, SDMMC_CMD_CMD24, block_num, 0x01, 0)) < 0) {
				sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_write_block", SDMMC_CMD_CMD24, ret);
				goto again;
			}

			if(ret != 0x00) {
				sdmmc_printf("%s: cmd %d resp = %d\r\n", "sdmmc_write_block", SDMMC_CMD_CMD24, ret);
				goto again;
			}

			sdmmc_readwrite_byte(reg, 0xFF); // write 1 dummy byte after Write command

			if((ret = sdmmc_tx_buf(reg, b, SDMMC_BLOCK_SIZE, SDMMC_TOKEN_DATA1)) != 0) {
				sdmmc_printf("%s: tx_buf failed at %d, retry = %d, ret = %d\r\n", "sdmmc_write_block", count - c - 1, retry, ret);
				goto again;
			}

			// Wait till data are written
			sdmmc_wait_ready(reg);

		} else {

			#ifdef SDMMC_ENABLE_PREERASE
			if(sdmmc_cards[iface].type != SDMMC_TYPE_MMC_V3) { // SDC can do pre-formatting

				// Send CMD55 - Leading ACMD (application command follows) 

				if((ret = sdmmc_send_cmd(reg, SDMMC_CMD_CMD55, 0x0, 0x01, 0)) != 0) {
					sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_write_block", SDMMC_CMD_CMD55, ret);
					goto again;
				}

				// Send ACMD23 - Pre-erase blocks 

				if((ret = sdmmc_send_cmd(reg, SDMMC_CMD_CMD23, count, 0x01, 0)) != 0) {
					sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_write_block", SDMMC_CMD_CMD23, ret);
					goto again;
				}
			}
			#endif

			// Send CMD25 - Write multiple blocks at block_num 

			if((ret = sdmmc_send_cmd(reg, SDMMC_CMD_CMD25, block_num, 0x01, 0)) != 0) {
				sdmmc_printf("%s: cmd %d failed, ret = %d\r\n", "sdmmc_write_block", SDMMC_CMD_CMD25, ret);
				goto again;
			}

			sdmmc_readwrite_byte(reg, 0xFF); // write 1 dummy byte after Write command

			while(c--) {

				if((ret = sdmmc_tx_buf(reg, b, SDMMC_BLOCK_SIZE, 0xfc)) != 0) {
					sdmmc_printf("%s: tx_buf failed at %d, retry = %d, ret = %d\r\n", "sdmmc_write_block", count - c - 1, retry, ret);
					goto again;
				}
				b += SDMMC_BLOCK_SIZE;

				// Wait till data are written
				sdmmc_wait_ready(reg);
			}


			// Send Stop Tran Token
			sdmmc_readwrite_byte(reg, SDMMC_TOKEN_STOP);
			sdmmc_readwrite_byte(reg, 0xFF); // write 1 dummy byte after Write command

		}
 
		// Wait till data are written
		sdmmc_wait_ready(reg);

		sdmmc_deselect(reg, ss);

		break;

		again:

		// Send Stop Tran Token
		sdmmc_readwrite_byte(reg, SDMMC_TOKEN_STOP);
		sdmmc_readwrite_byte(reg, 0xFF); // write 1 dummy byte after Write command

		sdmmc_deselect(reg, ss);
	} // retry

	return ret;
}
