#ifndef __SOC_H__
#define __SOC_H__

#include "timer.h"
#include "prescaler.h"
#include "interrupt.h"
#include "gpio.h"
#include "uart.h"
#include "pwm.h"
#include "hub.h"
#include "mac.h"
#include "i2c.h"
#include "wd.h"
#include "audiodac.h"
#include "cga.h"

#define TIMER_PRESCALER	((Prescaler_Reg*)0xF0020000)
#define TIMER_INTERRUPT	((InterruptCtrl_Reg*)0xF0020010)
#define TIMER0		((Timer_Reg*)0xF0020000)
#define TIMER1		((Timer_Reg*)0xF0021000)
#define UART0		((Uart_Reg*)(0xF0010000))
#define UART1		((Uart_Reg*)(0xF0011000))
#define PWM		((Pwm_Reg*)0xF0030000)
#define HUB		((Hub_Reg*)(0xF0050000))
#define PLIC		((PLIC_Reg*)(0xF0060000))
#define GPIO		((Gpio_Reg*)(0xF0000000))
#define MAC		((MAC_Reg*)(0xF0070000))
#define I2C0		((I2C_Reg*)(0xF0090000))
#define WD		((WD_Reg*)(0xF00A0000))
#define MTIME		(*(volatile unsigned long long*)(0xF00B0000))
#define AUDIODAC0	((AUDIODAC_Reg*)(0xF00C0000))

inline volatile unsigned long long get_mtime(void) { return MTIME; }

#define	SYSTEM_CLOCK_HZ	58333000
//#define	SYSTEM_CLOCK_HZ	50000000
//#define	SYSTEM_CLOCK_HZ	65000000
//#define	SYSTEM_CLOCK_HZ	62000000
//#define	SYSTEM_CLOCK_HZ	60000000


#endif /* __SOC_H__ */
