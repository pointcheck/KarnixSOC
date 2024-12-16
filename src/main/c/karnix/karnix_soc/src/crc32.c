#include <stdint.h>
/*
	This code is take from https://create.stephan-brumme.com/crc32/

	Copyright (c) 2011-2019 Stephan Brumme. All rights reserved.

	zlib's CRC32 polynomial is 0xEDB88320 which is bit-refversed 0x04C11DB7

 */
uint32_t crc32(const void* data, uint32_t length, uint32_t previousCrc32, uint32_t polynomial)
{
	//uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
	uint32_t crc = previousCrc32 ^ 0xFFFFFFFF;
	const uint8_t* current = (const uint8_t*) data;

	while (length-- != 0) {
		crc ^= *current++;

		for (int j = 0; j < 8; j++) {
			// branch-free
			crc = (crc >> 1) ^ (-(int32_t)(crc & 1) & polynomial);

			// branching, much slower:
			//if (crc & 1)
			//	crc = (crc >> 1) ^ polynomial;
			//else
			//	crc =	crc >> 1;
		}
	}

	//return ~crc; // same as crc ^ 0xFFFFFFFF
	return crc ^ 0xFFFFFFFF;
}

