#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdint.h>

uint32_t crc32(const void* data, uint32_t length, uint32_t previousCrc32, uint32_t polynomial);

#endif
