#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef	CONFIG_HAS_MAC
#include "lwip/inet.h"
#define	CONFIG_OPTION_USE_DHCP	0x01
#endif
#include "eeprom.h"
#include "utils.h"

#ifdef	CONFIG_HAS_USERDATA
#include "config_userdata.h" // should be provided by user in its source code tree
#endif

//#pragma pack(1)
typedef struct {
	#ifdef	CONFIG_HAS_MAC
	ip4_addr_t	ip_addr;
	ip4_addr_t	gw_addr;
	ip4_addr_t	netmask;
	uint8_t		options;
	uint8_t		mac_addr[6];
	#endif
	#ifdef	CONFIG_HAS_HUB
	uint8_t		hub_type;
	#endif
	#ifdef	CONFIG_HAS_MODBUS
	uint8_t		modbus_addr;
	uint32_t	modbus_baud;
	#endif
	#ifdef	CONFIG_HAS_USERDATA
	ConfigUserData	userdata;
	#endif
	uint16_t	reserved;
	uint16_t	crc16;
} Config;
//#pragma pack(0)
 

extern Config active_config;
extern Config default_config;

int config_load(Config* config);
int config_save(Config* config);

#endif // _CONFIG_H_
