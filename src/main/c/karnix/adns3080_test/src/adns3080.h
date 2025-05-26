#ifndef __ADNS3080_H__
#define __ADNS3080_H__

#include "spi.h"

int adns3080_init(SPI_Reg* reg, int ss, int gpio_pdn, int gpio_rst, int sample_rate);
unsigned char adns3080_read_reg(SPI_Reg* reg, int ss, int addr);
void adns3080_capture_frame(SPI_Reg* reg, int ss, unsigned char *buf);

#endif // __ADNS3080_H__
