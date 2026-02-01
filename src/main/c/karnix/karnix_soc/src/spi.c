#include <stdio.h>
#include "soc.h"
#include "spi.h"
#include "utils.h"
#include "riscv.h"

int spi_wait_tx_avail(SPI_Reg* reg, int min_avail, int timeout) {
	int i;
	int avail;

	for(i = 0; i < timeout; i++) {
		if((avail = spi_get_tx_avail(reg)) >= min_avail)
			break;
	}

	if(i == timeout)
		return 0;

	return avail;
}

int spi_xmit(SPI_Reg* reg, int ss, unsigned short* txbuf, unsigned short* rxbuf, int len) {

	int avail, sent = 0;

	while(len > 0) {
		if((avail = spi_get_tx_avail(reg)) < 3)
			break;

		for(int i = 0; i < MIN(avail/3, len); i++) {
			reg->data = SPI_CMD_ENABLE_SS | ss;
			reg->data = (unsigned int)(*txbuf++);
			reg->data = SPI_CMD_DISABLE_SS | ss;
			*rxbuf++ = (reg->data) & 0xffff;
			len--;
			sent++;
		}
	}

	return sent;
}


int spi_clear_rx_fifo(SPI_Reg* reg) {
	volatile int tmp;
	while(reg->rxoccupancy)
		tmp = reg->data;
}

