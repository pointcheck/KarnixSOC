#ifndef __QSPI_H__
#define __QSPI_H__

#include <stdint.h>
#include <string.h>

#define	QSPI_MEMORY_ADDRESS	0xA0000000
#define	QSPI_MEMORY_SIZE	0x01000000
#define	QSPI_CTRL_WE		(1<<31)
#define	QSPI_CTRL_CMD_ERASE	(1<<30)
#define	QSPI_CTRL_CMD_STATUS	(1<<29)
#define	QSPI_CTRL_CMD_PROGRAM	(1<<28)
#define	QSPI_CTRL_PROGRESS	(1<<16)
#define	QSPI_CTRL_STATUS_M	(0xff)
#define	QSPI_DEVICE_STATUS_BUSY	(1<<0)
#define	QSPI_DEVICE_STATUS_WE	(1<<1)

#pragma pack(1)
typedef struct
{
	volatile uint32_t CTRL;
	volatile uint32_t ERASE_SECTOR;	// Write sector number to be erased, set CTRL[31] = 1 before that. 
} QSPI_Reg;
#pragma pack(0)

#define	QSPI	((QSPI_Reg*)(0xF00C2000))

/* Erase one 4K sector. sector is linear byte address within the sector to be erased */
inline static void qspi_erase_sector(uint32_t sector) {
	QSPI->ERASE_SECTOR = sector;
	QSPI->CTRL |= QSPI_CTRL_WE | QSPI_CTRL_CMD_ERASE;
}


/* Wait while QSPI CMD is being sent to device */
inline static void qspi_wait_progress(void) {
	while(QSPI->CTRL & QSPI_CTRL_PROGRESS);
}

/* Request device Status Req 1 by sending Status CMD */ 
static uint8_t qspi_get_status(void) {
	qspi_wait_progress();
	QSPI->CTRL |= QSPI_CTRL_CMD_STATUS;
	qspi_wait_progress();
	return QSPI->CTRL & QSPI_CTRL_STATUS_M;
}

inline static void qspi_write_enable(void) {
	QSPI->CTRL |= QSPI_CTRL_WE;
}

inline static void qspi_write_disable(void) {
	QSPI->CTRL &= ~QSPI_CTRL_WE;
}

#endif // __QSPI_H__

