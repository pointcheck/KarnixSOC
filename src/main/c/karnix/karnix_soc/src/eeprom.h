#ifndef	_EEPROM_H_
#define _EEPROM_H_

#include "i2c.h"

#define EEPROM_I2C_ADDRESS      ((0xA << 3) | (0b000))  // Lower three bits represend device configurable address: A2, A1 and A0 pins accordingly

int eeprom_probe(I2C_Reg* i2c);
int eeprom_write_page(I2C_Reg* i2c, uint8_t device_addr, uint8_t mem_addr, uint8_t bytes, uint8_t* data);
int eeprom_poll(I2C_Reg* i2c, uint8_t device_addr);
int eeprom_read_page(I2C_Reg* i2c, uint8_t device_addr, uint8_t mem_addr, uint8_t bytes, uint8_t* data);


#endif // _EEPROM_H_

