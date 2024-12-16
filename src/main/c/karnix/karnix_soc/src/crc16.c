
#include "crc16.h"


uint16_t crc16(uint8_t *buf, uint16_t len) {
	uint16_t crc = 0xFFFF;
	for (int pos = 0; pos < len; pos++) {
		crc ^= (unsigned int)buf[pos];		// XOR byte into least sig. byte of crc

		for (int i = 8; i != 0; i--) {		// Loop over each bit
			if ((crc & 0x0001) != 0) {	// If the LSB is set
				crc >>= 1;		// Shift right and XOR 0xA001
				crc ^= 0xA001;
			}
			else				// Else LSB is not set
				crc >>= 1;		// Just shift right
		}
	}

	return crc;
}

void add_crc(uint8_t *buf, uint16_t len) {
        uint16_t crc = crc16(buf, len);
        buf[len] = (crc & 0xff);
        buf[len+1] = (crc >> 8) & 0xff;
}

