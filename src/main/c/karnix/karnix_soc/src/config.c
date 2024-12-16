#include "soc.h"
#include "eeprom.h"
#include "config.h"
#include "crc16.h"

#include <string.h>

Config default_config = {
	IPADDR4_INIT_BYTES(192,168,174,199),	// IP address
	IPADDR4_INIT_BYTES(192,168,174,1),	// IP gateway
	IPADDR4_INIT_BYTES(255,255,255,0),	// IP netmask
	0x01,					// Option: Use DHCP is ON
	{0x02,0x0A,0x0B,0x0C,0x15,0x94},	// MAC addr
	75,					// HUB type (12 - HUB12, 75 - HUB75)
	1,					// modbus add (RTU and UDP)
	115200,					// modbus RTU baud rate
	0xffff					// CRC16 check-sum, will be computed automagically
};


Config active_config;

int config_load(Config* config) {

	if(!config)
		return -1;

	//printf("config_load() loading %d bytes from EEPROM\r\n", sizeof(Config));

	if(sizeof(Config) > 128-4) {
		printf("config_load() config is too big to fit in EEPROM (%d)\r\n", sizeof(Config));
		return -1;
	}

	for(int i = 0; i < (sizeof(Config) + 3) / 4; i++) {

		int len = 4;

		if(sizeof(Config) - i*4 < 4)
			len = sizeof(Config) - i*4;

		if(eeprom_read_page(I2C0, EEPROM_I2C_ADDRESS, i*4, len, (uint8_t*)config + i*4) != len) {
			printf("config_load() error reading %d bytes from EEPROM!\r\n", len);
			return -2;
		}
	}

	uint16_t my_crc16 = crc16((uint8_t*)config, sizeof(Config) - 2);

	if(my_crc16 != config->crc16) {
		printf("config_load() CRC16 mismatch: 0x%04X != 0x%04X\r\n", my_crc16, config->crc16); 
		return -3;
	}

	printf("config_load() loaded %d bytes from EEPROM, CRC16 = %p\r\n", sizeof(Config), config->crc16);

	return 0;
}


int config_save(Config* config) {
	
	if(!config)
		return -1;

	if(sizeof(Config) > 128-4) {
		printf("config_save(%p) config is too big to fit in EEPROM (%d)\r\n", EEPROM_I2C_ADDRESS, sizeof(Config));
		return -1;
	}

	config->crc16 = crc16((uint8_t*)config, sizeof(Config) - 2);

	for(int i = 0; i < (sizeof(Config) + 3) / 4; i++) {

		int len = 4;

		if(sizeof(Config) - i*4 < 4)
			len = sizeof(Config) - i*4;

		if(eeprom_write_page(I2C0, EEPROM_I2C_ADDRESS, i*4, len, (uint8_t*)config + i*4) != len) {
			printf("config_save(%p) error writing %d bytes of page %d to EEPROM at %p!\r\n", EEPROM_I2C_ADDRESS, len, i, i*4);
			return -2;
		}

		int i;
		for(i = 0; i < 10; i++) {
			if(eeprom_poll(I2C0, EEPROM_I2C_ADDRESS) == 0)
				break;
			delay_us(1000); // 5ms is max write delay, as per datasheet
		}

		if(i == 10) {
			printf("config_save(%p) failed to recover from write\r\n", EEPROM_I2C_ADDRESS);
			return -3;
		}

	}

	printf("config_save() saved %d bytes to EEPROM, CRC16 = %p\r\n", sizeof(Config), config->crc16);

	return 0;
}



