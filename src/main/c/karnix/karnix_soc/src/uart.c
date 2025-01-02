#include <stdint.h>
#include "uart.h"
#include "soc.h"

int uart_read(Uart_Reg *reg, char *buf, int size, int timeout) {
	int bytes_read = 0;

	if(!buf)
		return -1;

	while(bytes_read < size) {
		uint64_t t0 = get_mtime();
		int avail;

		while((avail = uart_readOccupancy(reg)) == 0)
			if(get_mtime() - t0 >= 1000LL * timeout)
					return bytes_read;

		while(avail--)
			buf[bytes_read++] = reg->DATA;
	}

	return bytes_read;
}
