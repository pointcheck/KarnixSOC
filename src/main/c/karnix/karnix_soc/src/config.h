#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "lwip/inet.h"
#include "eeprom.h"
#include "utils.h"

//#pragma pack(1)
typedef struct {
	ip4_addr_t	ip_addr;
	ip4_addr_t	gw_addr;
	ip4_addr_t	netmask;
	uint8_t		options;
	uint8_t		mac_addr[6];
	uint8_t		hub_type;
	uint8_t		modbus_addr;
	uint32_t	modbus_baud;
	uint16_t	reserved;
	uint16_t	crc16;
} Config;
//#pragma pack(0)
 
#define	CONFIG_OPTION_USE_DHCP	0x01

extern Config active_config;
extern Config default_config;

int config_load(Config* config);
int config_save(Config* config);

#endif // _CONFIG_H_
