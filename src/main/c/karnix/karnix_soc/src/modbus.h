#ifndef _MODBUS_H_
#define _MODBUS_H_

#include <stdio.h>

#define MODBUS_RX_BUF_SIZE 		128
#define MODBUS_TX_BUF_SIZE		128
#define	MODBUS_RX_DELAY_US		10000		// 10ms delay before RX complete

#define FUNC_READ_COILS                 0x01            // Read one or more coils' status
#define FUNC_READ_DISC_INPUT            0x02            // Read one or more discrete inputes
#define FUNC_READ_HOLD_REGS             0x03            // Read one or more holding registers (current PWM)
#define FUNC_READ_INPUT_REGS            0x04            // Read one or more input registers (inpud ADC)
#define FUNC_WRITE_ONE_COIL_REG         0x05            // Write one coil (ON/OFF)
#define FUNC_WRITE_ONE_HOLD_REG         0x06            // Write one holding register (PWM)
#define FUNC_WRITE_MANY_COIL_REG        0x0F            // Write many coils (ON/OFF)
#define FUNC_WRITE_MANY_HOLD_REG        0x10            // Write many holding registers (PWM)
#define FUNC_ID                         0x11            // Read device ID

#define ERR_FUNC_NOT_IMPLEMENTED        0x01            // Error code: func not implemented
#define ERR_ADDR_NOT_AVAILABLE          0x02            // Error code: register address not available
#define ERR_WRONG_ARGS                  0x03            // Error code: invalid arguments
#define ERR_SYS_ERROR			0x07            // Error code: system failure 

#define	REG_FRAMEBUFFER			20		// Command to write directly to framebuffer with given offset (first two bytes)
#define REG_SYS_COUNTER			30              // Register holding system counter value (32 bits)
#define	REG_IRQ_COUNTER			31		// Register holding IRQ counter (32 bits)
#define	REG_SCRATCH			32		// Register holding temporary data (32 bits)

#define	REG_IP_ADDR			41		// Register holding current IP address (32 bits)
#define	REG_GW_ADDR			42		// Register holding current IP gatwway (32 bits)
#define	REG_NETMASK			43		// Register holding current IP netmask (32 bits)
#define	REG_MBUS_ADDR			44		// Register holding current Modbus address (32 bits, only lower 8 bits are used)
#define	REG_MBUS_BAUD			45		// Register holding current Modbus serial baud rate (32 bits)
#define	REG_MAC_ADDR			47		// Register holding current MAC HW address (6 bytes)
#define	REG_HUB_TYPE			48		// Currently used HUB type (32 bits, only 8 bits used) 
#define	REG_CONFIG_OPTIONS		49		// Misc configuration options (32 bits, only 8 bits used) 

#define	REG_PRINT_TEXT			50		// Print text on the LED screen (X: 16 bits, Y: 16 bits, FontID: 16 bits, remaining is TEXT)
#define	REG_CLEAR_TEXT			51		// Clear screen
#define	REG_COLOR			52		// Current text color 
#define	REG_VIDEO_MODE			53		// Choose between CGA (1) and HUB (2) 

#define	REG_AUDIODAC0_BUFFER		60		// Command to write to AUDIODAC0 tx buffer 


#define	REG_CONFIG_WRITE		126		// Config write Operations: (32 bit signed) - any positive value greater than 0 allows config to be written 
#define	REG_REBOOT			127		// Writing to this register causes hard reboot

#define REG_VIDEO_MODE_CGA  		1
#define REG_VIDEO_MODE_HUB		2

extern volatile uint32_t reg_sys_counter;
extern volatile uint32_t reg_irq_counter;
extern volatile uint32_t reg_scratch;
extern volatile uint32_t reg_config_write;

uint32_t modbus_error(uint8_t f, uint8_t err_code, uint8_t *txbuf);
int modbus_recv(uint8_t *rx_buf, int rx_len, uint8_t *tx_buf);

#endif // _MODBUS_COMMON_H

