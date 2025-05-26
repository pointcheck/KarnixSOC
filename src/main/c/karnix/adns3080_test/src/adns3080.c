#include "utils.h"
#include "soc.h"
#include "adns3080.h"

int adns3080_init(SPI_Reg* reg, int ss, int gpio_pdn, int gpio_rst, int sample_rate) {
	SPI_Config spi_cfg;
	
	spi_cfg.config = SPI_CONFIG_CPHA | SPI_CONFIG_CPOL;
	//spi_cfg.config = SPI_CONFIG_CPHA;
	spi_cfg.ssSetup = 16;
	spi_cfg.ssHold = 16;
	spi_cfg.ssDisable = 16;
	//spi_cfg.divider = SYSTEM_CLOCK_HZ/(2*sample_rate*(16 + 1));
	spi_cfg.divider = SYSTEM_CLOCK_HZ/(2*sample_rate);
	spi_applyConfig(reg, &spi_cfg);

	printk("adns3080_init: divider = %d\r\n", spi_cfg.divider);
	
	GPIO1->OUTPUT_ENABLE |= (gpio_rst | gpio_pdn);
	GPIO1->OUTPUT |= gpio_rst; // Reset active
	delay_us(10);
	GPIO1->OUTPUT &= ~gpio_rst; // Reset in-active 
	delay_us(500); // Wait for sensor to get ready
	GPIO1->OUTPUT |= gpio_pdn; // Power UP
	delay_us(75000); // Wait for return from Power Down mode

	// Read Device ID and version

	reg->data = SPI_CMD_ENABLE_SS | ss;
	reg->data = SPI_CMD_SEND | 0x00; // Read reg 0x00 - Device ID
	delay_us(75);
	reg->data = SPI_CMD_SEND_RECEIVE | 0x00;
	reg->data = SPI_CMD_DISABLE_SS | ss;
	delay_us(75);
	unsigned int id = reg->data & 0xff;

	reg->data = SPI_CMD_ENABLE_SS | ss;
	reg->data = SPI_CMD_SEND | 0x01; // Read reg 0x01 - Version
	delay_us(75);
	reg->data = SPI_CMD_SEND_RECEIVE | 0x00;
	reg->data = SPI_CMD_DISABLE_SS | ss;
	delay_us(75);
	unsigned int ver = reg->data & 0xff;

	printk("adns3080_init: id = 0x%x, ver = 0x%x\r\n", id, ver);
	
	return (id << 8) | ver; 
}    

unsigned char adns3080_read_reg(SPI_Reg* reg, int ss, int addr) {
	reg->data = SPI_CMD_ENABLE_SS | ss;
	reg->data = SPI_CMD_SEND | (addr & 0x7F);
	delay_us(75);
	reg->data = SPI_CMD_SEND_RECEIVE | 0x00;
	reg->data = SPI_CMD_DISABLE_SS | ss;
	delay_us(75);
	return reg->data & 0xff;
}


void adns3080_capture_frame(SPI_Reg* reg, int ss, unsigned char *buf) {
	if(buf == NULL)
		return;

	// Write to Frame Capture reg to force capture
	reg->data = SPI_CMD_ENABLE_SS | ss;
	reg->data = SPI_CMD_SEND | 0x80 | 0x13; // bit7 is write flag
	reg->data = SPI_CMD_SEND | 0x83;
	reg->data = SPI_CMD_DISABLE_SS | ss;
	delay_us(32*2 + 50); // 2*tTX + tSWW

	// Wait 3 frame periods + 10 nanoseconds for frame to be copied to SROM buffer
	// Minimum frame speed is 2000 frames/second so 1 frame = 500 nano seconds. So 500 x 3 + 10 = 1510
	delay_us(1510);

	reg->data = SPI_CMD_ENABLE_SS | ss;
	reg->data = SPI_CMD_SEND | 0x40; // Read Pixel Burst reg

	delay_us(32 + 75); // tTX + tSRAD_MOT

	// Read till bit6 is 1 to find first pixel of the frame
	for(int i = 0; i < 600; i++) {
		reg->data = SPI_CMD_SEND_RECEIVE | 0x00; // Receive next pixel
		delay_us(32 + 10); // tRX + tLOAD
		uint8_t pixel = reg->data;
		if(pixel & 0b11000000 == 0b11000000) { // bit7 is always 1, bit6 is 1 for first pixel
			*buf++ = pixel; // Store first pixel data
			printk("adns3080: frame found at i = %d\r\n", i);
			break;
		}
	}

	// Read consequent 899 pixels 
	for(int i = 0; i < 900-1; i++) {
		reg->data = SPI_CMD_SEND_RECEIVE | 0x00;
		delay_us(32 + 10); // tRX + tLOAD
		*buf++ = reg->data; // Store next pixel data
	}
		
	reg->data = SPI_CMD_DISABLE_SS | ss;

	delay_us(4+10); // tLOAD + tBEXIT 
}

