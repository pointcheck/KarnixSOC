/*
	I2C driver for SpinalHDL 1.4.4 I2C controller

	Refactored by Ruslan Zalata <rz@fabmicro.ru>

	Based on SaxonSoc 

*/

#ifndef _I2C_H_
#define _I2C_H_

typedef struct {
	volatile uint32_t tx_data; 		// 0x00
	volatile uint32_t rx_data;		// 0x04
	volatile uint32_t clock_div;		// 0x08
	volatile uint32_t status;		// 0x0C
	volatile uint32_t state;		// 0x10
	volatile uint32_t bit_counter;		// 0x14
} I2C_Reg;


typedef struct {
	uint32_t clock_div; //Number of cycle - 1 between each SDA/SCL sample
} I2C_Config;



#define I2C_STATUS_NONE		0x00000000
#define I2C_STATUS_ENABLE	0x00000001
#define I2C_STATUS_BUSY		0x00000002
#define I2C_STATUS_NEED_START	0x00000004
#define I2C_STATUS_NEED_STOP	0x00000008
#define I2C_STATUS_TX_DRIVE	0x00000010
#define I2C_STATUS_TX_ACK	0x00000020
#define I2C_STATUS_RX_ACK	0x00000040
#define I2C_READ		0x00000001
#define I2C_WRITE		0x00000000


static inline void i2c_applyConfig(I2C_Reg* reg, I2C_Config *config) {
	reg->clock_div = config->clock_div;
}


static inline void i2c_disable(I2C_Reg* reg) {
	reg->status = 0;
}


static inline void i2c_xmitData(I2C_Reg* reg, uint32_t data, uint32_t flags) {
        reg->status = flags; // It's important to set flags first !!!
        reg->tx_data = data;
}


void i2c_init(I2C_Reg* reg);
int i2c_wait_for_busy(I2C_Reg* reg, int timeout);
int i2c_wait_for_release(I2C_Reg* reg, int timeout);
void i2c_xmitData(I2C_Reg* reg, uint32_t data, uint32_t flags);

#endif // _I2C_H_

