#ifndef _CRC16_H_
#define _CRC16_H_

#include <stdint.h>

uint16_t crc16(uint8_t *buf, uint16_t len);
void add_crc(uint8_t *buf, uint16_t len);

#endif // _CRC16_H_
