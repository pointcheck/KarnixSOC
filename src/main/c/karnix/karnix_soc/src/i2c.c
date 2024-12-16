#include <stdio.h>
#include "soc.h"
#include "i2c.h"


void i2c_init(I2C_Reg* reg) {
	I2C_Config i2c;
	i2c.clock_div = SYSTEM_CLOCK_HZ/100000;
	i2c_applyConfig(reg, &i2c);
}


int i2c_wait_for_busy(I2C_Reg* reg, int timeout) {
	for(int i = 0; i < timeout; i++) {
		if(reg->status & I2C_STATUS_BUSY)
			return 1; // OK
	}
	return 0;
}


int i2c_wait_for_release(I2C_Reg* reg, int timeout) {
	for(int i = 0; i < timeout; i++) {
		if((reg->status & I2C_STATUS_BUSY) == 0)
			return 1; // OK
	}
	return 0;
}



