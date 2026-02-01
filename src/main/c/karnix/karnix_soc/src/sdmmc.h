/*
	Implementation of simple driver for accessing SD/MMC cards over SPI.

	Depends on SPI controller/driver from SpinalHDL.

	Written by Ruslan Zalata <rz@fabmicro.ru>.

	Copyright (C) 2026, Fabmicro, LLC. Tyumen, Russia.

	SPDX-License-Identifier: BSD-2-Clause
*/

#ifndef __SDMMC_H__
#define __SDMMC_H__

#include "soc.h"
#include "spi.h"

#define SDMMC_SPI_BITRATE_INIT          400000		// Bitrate while Init
#define SDMMC_SPI_BITRATE_NORMAL        20000000	// Bitrate for normal operations (20MHz - MMC, 25MHZ - SDC)
//#define SDMMC_SPI_BITRATE_NORMAL        31000000	// Speed higher 25MHz does not work on some cards
#define	SDMMC_IFACES			2		// Number of available SD/MMC interfaces
#define	SDMMC_TIMEOUT			0x07ffff	// Wait cycles
#define	SDMMC_RETRIES			3		// Number of attempts to perform init
#define	SDMMC_BLOCK_SIZE		512		// Should always be 512 bytes

#define	SDMMC_ERROR_OK			0		// OK
#define	SDMMC_ERROR_PARAMS		-1		// Bogus parameters
#define	SDMMC_ERROR_TIMEOUT		-2		// Timtout
#define	SDMMC_ERROR_NO_RESP		-3		// No response received
#define	SDMMC_ERROR_NO_TOKEN		-4		// No token received	
#define	SDMMC_ERROR_INIT_FAIL		-5		// Initialization sequence failed	
#define	SDMMC_ERROR_NOT_READY		-6		// SD/MMC interface is not ready
#define	SDMMC_ERROR_CRC			-7		// CRC16 mismatch on block read 
#define	SDMMC_ERROR_WRITE		-8		// Block write error 

#define	SDMMC_TOKEN_DATA1		0xfe		// Data initiation token for CMD17/18/24
#define	SDMMC_TOKEN_DATA2		0xfc		// Data initiation token for CMD25
#define	SDMMC_TOKEN_STOP		0xfd		// Stoptransaction for CMD25

#define	SDMMC_RESP_DATA_ACK		0xf5		// XXX00101 - data accepted
#define	SDMMC_RESP_DATA_CRC		0xfb		// XXX01011 - data rejected due CRC error 
#define	SDMMC_RESP_DATA_WE		0xfd		// XXX01101 - data rejected due write error 

#define	SDMMC_CMD_CMD0			0 // Soft reset 
#define	SDMMC_CMD_CMD1			1 // Start init process 
#define	SDMMC_CMD_CMD8			8 // Check voltage range (SDC_V2)
#define	SDMMC_CMD_CMD9			9 // Read CSD data
#define	SDMMC_CMD_CMD10			10 // Read CID data
#define	SDMMC_CMD_CMD12			12 // Stop data transmission
#define	SDMMC_CMD_CMD16			16 // Set R/W block size, should return 0x00
#define	SDMMC_CMD_CMD17			17 // Read block 
#define	SDMMC_CMD_CMD18			18 // Read multiple blocks 
#define	SDMMC_CMD_CMD23			23 // SDC - Set N block to be erased before writing multiple sections
					   // MMC - Num blocks to transfer in multi mode
#define	SDMMC_CMD_CMD24			24 // Write block
#define	SDMMC_CMD_CMD25			25 // write Multi sector
#define	SDMMC_CMD_CMD41			41 // Start init process (SDC_V2), should return 0x00
#define	SDMMC_CMD_CMD55			55 // App command (ACMD), should return 0x01
#define	SDMMC_CMD_CMD58			58 // Read OCR information
#define	SDMMC_CMD_CMD59			59 // Enable / disable CRC, return 0x00

#define	SDMMC_TYPE_NONE			0
#define	SDMMC_TYPE_SDC_V1		1
#define	SDMMC_TYPE_SDC_V2		2
#define	SDMMC_TYPE_SDC_V2HC		3
#define	SDMMC_TYPE_MMC_V3		4


struct sdmmc_iface_info {
	SPI_Reg* reg;
	int ss;		
};

struct sdmmc_card_info {
	uint32_t type;
	uint32_t ocr;
	uint32_t blocks;	// Number of blocks (sectors)
	uint8_t cid_data[16];	// CID data: manufacturer info
	uint8_t csd_data[16];	// CSD data: capacity, etc 
};

extern const struct sdmmc_iface_info sdmmc_ifaces[];
extern struct sdmmc_card_info sdmmc_cards[];
extern const char* sdmmc_types[];

int sdmmc_init(int);
uint32_t sdmmc_get_num_blocks(uint8_t csd[]);
int sdmmc_read_block(int iface, int block_num, int count, uint8_t* buf);
int sdmmc_write_block(int iface, int block_num, int count, uint8_t* buf);

#endif // __SDMMC_H__

