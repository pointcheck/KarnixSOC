#ifndef __USB10_H__
#define __USB10_H__

#include <stdint.h>
#include <string.h>

#define	USB10_STATUS_ERROR_S	30
#define	USB10_STATUS_ERROR_M	0x01
#define	USB10_STATUS_ERROR(X)	(((X) >> USB10_STATUS_ERROR_S) & USB10_STATUS_ERROR_M)
#define	USB10_STATUS_ERROR_BIT	(USB10_STATUS_ERROR_M << USB10_STATUS_ERROR_S)	

#define	USB10_STATUS_REPORT_S	29	
#define	USB10_STATUS_REPORT_M	0x01
#define	USB10_STATUS_REPORT(X)	(((X) >> USB10_STATUS_REPORT_S) & USB10_STATUS_REPORT_M)
#define	USB10_STATUS_REPORT_BIT	(USB10_STATUS_REPORT_M << USB10_STATUS_REPORT_S)	

#define	USB10_STATUS_BUSY_S	28	
#define	USB10_STATUS_BUSY_M	0x01
#define	USB10_STATUS_BUSY(X)	(((X) >> USB10_STATUS_BUSY_S) & USB10_STATUS_BUSY_M)
#define	USB10_STATUS_BUSY_BIT	(USB10_STATUS_BUSY_M << USB10_STATUS_BUSY_S)

#define	USB10_STATUS_RECEIVED_S		27
#define	USB10_STATUS_RECEIVED_M		0x01
#define	USB10_STATUS_RECEIVED(X)	(((X) >> USB10_STATUS_RECEIVED_S) & USB10_STATUS_RECEIVED_M)
#define	USB10_STATUS_RECEIVED_BIT	(USB10_STATUS_RECEIVED_M << USB10_STATUS_RECEIVED_S)

#define	USB10_STATUS_PID_S	16
#define	USB10_STATUS_PID_M	0xff
#define	USB10_STATUS_PID(X)	(((X) >> USB10_STATUS_PID_S) & USB10_STATUS_PID_M)

#define	USB10_STATUS_STATE_S	0	
#define	USB10_STATUS_STATE_M	0xff
#define	USB10_STATUS_STATE(X)	(((X) >> USB10_STATUS_PID_S) & USB10_STATUS_PID_M)

#define	USB10_CMD_START_S	31
#define	USB10_CMD_START_M	0x01
#define	USB10_CMD_START(X)	(((X) >> USB10_CMD_START_S) & USB10_CMD_START_M)
#define	USB10_CMD_START_BIT	(USB10_CMD_START_M << USB10_CMD_START_S)

#define	USB10_CMD_ADDR_S	24
#define	USB10_CMD_ADDR_M	0x7f
#define	USB10_CMD_ADDR(X)	(((X) >> USB10_CMD_ADDR_S) & USB10_CMD_ADDR_M)
#define	USB10_CMD_SET_ADDR(X)	(((X) & USB10_CMD_ADDR_M) << USB10_CMD_ADDR_S)

#define	USB10_CMD_ENDP_S	20	
#define	USB10_CMD_ENDP_M	0x0f
#define	USB10_CMD_ENDP(X)	(((X) >> USB10_CMD_ENDP_S) & USB10_CMD_ENDP_M)
#define	USB10_CMD_SET_ENDP(X)	(((X) & USB10_CMD_ENDP_M) << USB10_CMD_ENDP_S)

#define	USB10_CMD_LEN_S		8
#define	USB10_CMD_LEN_M		0xfff
#define	USB10_CMD_LEN(X)	(((X) >> USB10_CMD_LEN_S) & USB10_CMD_LEN_M)
#define	USB10_CMD_SET_LEN(X)	(((X) & USB10_CMD_LEN_M) << USB10_CMD_LEN_S)

#define	USB10_CMD_PID_S		4
#define	USB10_CMD_PID_M		0x0f
#define	USB10_CMD_PID(X)	(((X) >> USB10_CMD_PID_S) & USB10_CMD_PID_M)
#define	USB10_CMD_SET_PID(X)	(((X) & USB10_CMD_PID_M) << USB10_CMD_PID_S)

#define	USB10_CMD_S		0
#define	USB10_CMD_M		0x0f
#define	USB10_CMD(X)		(((X) >> USB10_CMD_S) >> USB10_CMD_M)
#define	USB10_CMD_SET(X)	(((X) & USB10_CMD_M) << USB10_CMD_S)

#define	USB10_CMD_NONE			0x00
#define	USB10_CMD_SEND_TOKEN		0x01	// Send arbitrary token
#define	USB10_CMD_SEND_SHORT_TOKEN	0x02	// Send arbitrary token
#define	USB10_CMD_SEND_DATA		0x03	// Send data packet
#define	USB10_CMD_BUS_RESET		0x04	// Initiate Bus Reset state

#define	USB10_STATE_UNCONNECTED	0
#define	USB10_STATE_IDLE	1
#define	USB10_STATE_TOKEN_SEND	2

#define	USB10_CONTROL_ENABLE_S	31
#define	USB10_CONTROL_ENABLE_M	0x01
#define	USB10_CONTROL_ENABLE(X)	(((X) >> USB10_CONTROL_ENABLE_S) & USB10_CONTROL_ENABLE_M)
#define	USB10_CONTROL_ENABLE_BIT	(USB10_CONTROL_ENABLE_M << USB10_CONTROL_ENABLE_S)

#define	USB10_CONTROL_KEEPALIVE_S	30
#define	USB10_CONTROL_KEEPALIVE_M	0x01
#define	USB10_CONTROL_KEEPALIVE(X)	(((X) >> USB10_CONTROL_KEEPALIVE_S) & USB10_CONTROL_KEEPALIVE_M)
#define	USB10_CONTROL_KEEPALIVE_BIT	(USB10_CONTROL_KEEPALIVE_M << USB10_CONTROL_KEEPALIVE_S)

#define	USB10_CONTROL_RESET_DELAY_S		0	
#define	USB10_CONTROL_RESET_DELAY_M		0xffff
#define	USB10_CONTROL_RESET_DELAY(X)		(((X) >> USB10_CONTROL_RESET_DELAY_S) & USB10_CONTROL_RESET_DELAY_M)
#define	USB10_CONTROL_RESET_DELAY_SET(X)	(((X) & USB10_CONTROL_RESET_DELAY_M) << USB10_CONTROL_RESET_DELAY_S)

#define	USB10_RX_STATUS_LEN_S	0
#define	USB10_RX_STATUS_LEN_M	0xffff
#define	USB10_RX_STATUS_LEN(X)	(((X) >> USB10_RX_STATUS_LEN_S) & USB10_RX_STATUS_LEN_M)

#define	USB10_LOW_SPEED_PACKET_SIZE	(8 + 64 + 16)	// PID + DATA + CRC16

#define	USB10_PID_SETUP		0b00101101	// "1011 0100"	SETUP 	Address for host-to-device control transfer
#define	USB10_PID_DATA0		0b11000011	// "1100 0011"	DATA0 	Even-numbered data packet
#define	USB10_PID_DATA1		0b01001011	// "1101 0010"	DATA1  	Odd-numbered data packet
#define	USB10_PID_IN		0b01101001	// "1001 0110"	IN	Address for device-to-host transfer
#define	USB10_PID_ACK		0b11010010	// "0100 1011"	ACK	Data packet accepted	
#define	USB10_PID_NACK		0b01011010	// "0101 1010"	NACK	Data packet not accepted; please retransmit 


#pragma pack(1)
typedef struct {
	volatile uint32_t STATUS;
	volatile uint32_t COMMAND;
	volatile uint32_t RECV_DATA_LOW;
	volatile uint32_t RECV_DATA_HIGH;
	volatile uint32_t SEND_DATA_LOW;
	volatile uint32_t SEND_DATA_HIGH;
	volatile uint32_t RX_STATUS;
	volatile uint32_t CONTROL;
} USB10_Reg;

typedef struct {
	uint8_t bLength;		// 1 Length of this descriptor = 18 bytes
	uint8_t bDescriptorType;	// 1 Descriptor type = DEVICE (01h)
	uint16_t bcdUSB;		// 2 USB specification version (BCD)
	uint8_t bDeviceClass;		// 1 Device class
	uint8_t bDeviceSubClass;	// 1 Device subclass
	uint8_t bDeviceProtocol;	// 1 Device Protocol
	uint8_t bMaxPacketSize0;	// 1 Max Packet size for endpoint 0
	uint16_t idVendor;		// 2 Vendor ID (or VID, assigned by USB-IF)
	uint16_t idProduct;		// 2 Product ID (or PID, assigned by the manufacturer)
	uint16_t bcdDevice;		// 2 Device release number (BCD)
	uint8_t iManufacturer;		// 1 Index of manufacturer string
	uint8_t iProduct;		// 1 Index of product string
	uint8_t iSerialNumber;		// 1 Index of serial number string
	uint8_t bNumConfigurations;	// 1 Number of configurations supported 
} USB10_Description;

typedef union {
	USB10_Description descr;
	uint32_t data[6];	
} USB10_DescriptionUnion;

#pragma pack(0)


int usb10_wait_cmd_complete(USB10_Reg* reg, int timeout); 
int usb10_bus_reset(USB10_Reg* reg, int wait_us);
int usb10_device_set_address(USB10_Reg* reg, int address);
int usb10_device_get_description(USB10_Reg* reg, int address);


#endif /* __USB10_H__ */

