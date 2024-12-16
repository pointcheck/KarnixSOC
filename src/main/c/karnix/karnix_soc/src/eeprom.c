#include <stdio.h>
#include "i2c.h"
#include "utils.h"
#include "eeprom.h"



int eeprom_write_page(I2C_Reg* i2c, uint8_t device_addr, uint8_t mem_addr, uint8_t bytes, uint8_t* data) {

	//printf("eeprom_write_page(%p) begin: device_addr = 0x%02x, mem_addr = %p, bytes = %d\r\n", i2c, device_addr, mem_addr, bytes);

	// Write device address

	i2c_xmitData(i2c, (device_addr << 1) | I2C_WRITE, I2C_STATUS_ENABLE | I2C_STATUS_TX_DRIVE | I2C_STATUS_NEED_START);

	if(!i2c_wait_for_busy(i2c, 100000)) {
		i2c_disable(i2c);
		printf("eeprom_write_page(%p) data xmit timeout #%d\r\n", i2c, 1);
		return -1;
	}

	if(!i2c_wait_for_release(i2c, 100000)) {
		i2c_disable(i2c);
		printf("eeprom_write_page(%p) data xmit timeout #%d\r\n", i2c, 2);
		return -1;
	}

	if(i2c->status & I2C_STATUS_RX_ACK) {
		i2c_disable(i2c);
		printf("eeprom_write_page(%p) data xmit no-ack #%d\r\n", i2c, 1);
		return -1;
	}

	delay_us(2); // cosmetique delay to let waveform look good on the scope, can be deleted


	// Write memory address

	i2c_xmitData(i2c,  (mem_addr & 0xff), I2C_STATUS_ENABLE | I2C_STATUS_TX_DRIVE);

	if(!i2c_wait_for_busy(i2c, 100000)) {
		i2c_disable(i2c);
		printf("eeprom_write_page(%p) data xmit timeout #%d\r\n", i2c, 3);
		return -1;
	}

	if(!i2c_wait_for_release(i2c, 100000)) {
		i2c_disable(i2c);
		printf("eeprom_write_page(%p) data xmit timeout #%d\r\n", i2c, 4);
		return -1;
	}

	if(i2c->status & I2C_STATUS_RX_ACK) {
		i2c_disable(i2c);
		printf("eeprom_write_page(%p) data xmit no-ack #%d\r\n", i2c, 2);
		return -1;
	}


	for(int i = 0; i < bytes; i++) {

		delay_us(2); // cosmetique delay to let waveform look good on the scope, can be deleted

		uint32_t flags = I2C_STATUS_ENABLE | I2C_STATUS_TX_DRIVE;

		if(i  == bytes - 1) // Is this the last byte ?
			flags = I2C_STATUS_ENABLE | I2C_STATUS_TX_DRIVE | I2C_STATUS_NEED_STOP;

		i2c_xmitData(i2c,  data[i], flags);


		if(!i2c_wait_for_busy(i2c, 100000)) {
			i2c_disable(i2c);
			printf("eeprom_write_page(%p) data xmit timeout #%d\r\n", i2c, 5);
			return -1;
		}

		if(!i2c_wait_for_release(i2c, 100000)) {
			i2c_disable(i2c);
			printf("eeprom_write_page(%p) data xmit timeout #%d\r\n", i2c, 6);
			return -1;
		}

		if(i2c->status & I2C_STATUS_RX_ACK) {
			i2c_disable(i2c);
			printf("eeprom_write_page(%p) data xmit no-ack #%d\r\n", i2c, 3);
			return -1;
		}

	}

	return bytes;

}


int eeprom_poll(I2C_Reg* i2c, uint8_t device_addr) {

	i2c_xmitData(i2c, (device_addr << 1) | I2C_WRITE, I2C_STATUS_ENABLE | I2C_STATUS_TX_DRIVE | I2C_STATUS_NEED_START | I2C_STATUS_NEED_STOP);

	if(!i2c_wait_for_busy(i2c, 100000)) {
		printf("eeprom_poll(%p) data xmit timeout #%d\r\n", i2c, 1);
		return -1;
	}

	if(!i2c_wait_for_release(i2c, 100000)) {
		printf("eeprom_poll(%p) data xmit timeout #%d\r\n", i2c, 2);
		return -1;
	}

	return (i2c->status & I2C_STATUS_RX_ACK) > 0;
}


int eeprom_read_page(I2C_Reg* i2c, uint8_t device_addr, uint8_t mem_addr, uint8_t bytes, uint8_t* data) {

	//printf("eeprom_read_page(%p) begin: device_addr = 0x%02x, mem_addr = %p, bytes = %d\r\n", i2c, device_addr, mem_addr, bytes);

	// Write device address for WRITE

	i2c_xmitData(i2c, (device_addr << 1) | I2C_WRITE, I2C_STATUS_ENABLE | I2C_STATUS_TX_DRIVE | I2C_STATUS_NEED_START);

	if(!i2c_wait_for_busy(i2c, 100000)) {
		i2c_disable(i2c);
		printf("eeprom_read_page(%p) data xmit timeout #%d\r\n", i2c, 1);
		return -1;
	}

	if(!i2c_wait_for_release(i2c, 100000)) {
		i2c_disable(i2c);
		printf("eeprom_read_page(%p) data xmit timeout #%d\r\n", i2c, 2);
		return -1;
	}

	if(i2c->status & I2C_STATUS_RX_ACK) {
		i2c_disable(i2c);
		printf("eeprom_read_page(%p) data xmit no-ack #%d\r\n", i2c, 1);
		return -1;
	}

	delay_us(2); // cosmetique delay to let waveform look good on the scope, can be deleted


	// Write memory address

	i2c_xmitData(i2c,  (mem_addr & 0xff), I2C_STATUS_ENABLE | I2C_STATUS_TX_DRIVE | I2C_STATUS_NEED_STOP);

	if(!i2c_wait_for_busy(i2c, 100000)) {
		i2c_disable(i2c);
		printf("eeprom_read_page(%p) data xmit timeout #%d\r\n", i2c, 3);
		return -1;
	}

	if(!i2c_wait_for_release(i2c, 100000)) {
		i2c_disable(i2c);
		printf("eeprom_read_page(%p) data xmit timeout #%d\r\n", i2c, 4);
		return -1;
	}

	if(i2c->status & I2C_STATUS_RX_ACK) {
		i2c_disable(i2c);
		printf("eeprom_read_page(%p) data xmit no-ack #%d\r\n", i2c, 2);
		return -1;
	}


	delay_us(2); // cosmetique delay to let waveform look good on the scope, can be deleted


	// Write device address for READ

	i2c_xmitData(i2c, (device_addr << 1) | I2C_READ, I2C_STATUS_ENABLE | I2C_STATUS_TX_DRIVE | I2C_STATUS_NEED_START);

	if(!i2c_wait_for_busy(i2c, 100000)) {
		i2c_disable(i2c);
		printf("eeprom_read_page(%p) data xmit timeout #%d\r\n", i2c, 5);
		return -1;
	}

	if(!i2c_wait_for_release(i2c, 100000)) {
		i2c_disable(i2c);
		printf("eeprom_read_page(%p) data xmit timeout #%d\r\n", i2c, 6);
		return -1;
	}

	if(i2c->status & I2C_STATUS_RX_ACK) {
		i2c_disable(i2c);
		printf("eeprom_read_page(%p) data xmit no-ack #%d\r\n", i2c, 3);
		return -1;
	}


	for(int i = 0; i < bytes; i++) {

		delay_us(2); // cosmetique delay to let waveform look good on the scope, can be deleted

		uint32_t flags = I2C_STATUS_ENABLE | I2C_STATUS_TX_ACK;

		if(i  == bytes - 1) // Is this the last byte ?
			flags = I2C_STATUS_ENABLE | I2C_STATUS_NEED_STOP;

		i2c_xmitData(i2c, 0xFF, flags);

		if(!i2c_wait_for_busy(i2c, 100000)) {
			i2c_disable(i2c);
			printf("eeprom_read_page(%p) data xmit timeout #%d\r\n", i2c, 7);
			return -1;
		}	

		if(!i2c_wait_for_release(i2c, 100000)) {
			i2c_disable(i2c);
			printf("eeprom_read_page(%p) data xmit timeout #%d\r\n", i2c, 8);
			return -1;
		}

		data[i] = i2c->rx_data;
	}

	return bytes;
}



int eeprom_probe(I2C_Reg* i2c) {

	printf("eeprom_probe(%p) begin\r\n", EEPROM_I2C_ADDRESS);


	uint32_t buf = 0xDEADBEEF;

	if(eeprom_write_page(i2c, EEPROM_I2C_ADDRESS, 128-4, 4, (uint8_t*)&buf) != 4) {
		printf("eeprom_probe(%p) failed while test-write\r\n", EEPROM_I2C_ADDRESS);
		return -1;
	}
	

	int i;
	for(i = 0; i < 10; i++) {
		if(eeprom_poll(i2c, EEPROM_I2C_ADDRESS) == 0)
			break;
		delay_us(1000); // 5ms is max write delay, as per datasheet
	}

	if(i == 10) {
		printf("eeprom_probe(%p) failed to recover from write\r\n", EEPROM_I2C_ADDRESS);
		return -1;
	}


	buf = 0;

	if(eeprom_read_page(i2c, EEPROM_I2C_ADDRESS, 128-4, 4, (uint8_t*)&buf) != 4) {
		printf("eeprom_probe(%p) failed while test-read\r\n", EEPROM_I2C_ADDRESS);
		return -1;
	}
	
	if(buf != 0xDEADBEEF) {
		printf("eeprom_probe(%p) failed read-write mismatch: %p\r\n", EEPROM_I2C_ADDRESS, buf);
		return -1;
	}


	printf("eeprom_probe(%p) done\r\n", EEPROM_I2C_ADDRESS);

	return 0;
}

