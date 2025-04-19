#ifndef __USB10_H__
#define __USB10_H__

#include <stdint.h>
#include <string.h>

#define	USB10_EP0		0

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

#define	USB10_STATUS_CRC16_OK_S		26
#define	USB10_STATUS_CRC16_OK_M		0x01
#define	USB10_STATUS_CRC16_OK(X)	(((X) >> USB10_STATUS_CRC16_OK_S) & USB10_STATUS_CRC16_OK_M)
#define	USB10_STATUS_CRC16_OK_BIT	(USB10_STATUS_CRC16_OK_M << USB10_STATUS_CRC16_OK_S)

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
#define	USB10_RX_STATUS_LEN_M	0xff
#define	USB10_RX_STATUS_LEN(X)	(((X) >> USB10_RX_STATUS_LEN_S) & USB10_RX_STATUS_LEN_M)

#define	USB10_RX_STATUS_PID_S	8	
#define	USB10_RX_STATUS_PID_M	0xff
#define	USB10_RX_STATUS_PID(X)	(((X) >> USB10_RX_STATUS_PID_S) & USB10_RX_STATUS_PID_M)

#define	USB10_LOW_SPEED_PACKET_SIZE_BITS	(8 + 64 + 16)	// PID + DATA[64] + CRC16
#define	USB10_LOW_SPEED_DATA_SIZE		8	// max payload bytes in DATA packet

#define	USB10_PID_SETUP		0b00101101	// "1011 0100"	SETUP 	Address for host-to-device control transfer
#define	USB10_PID_DATA0		0b11000011	// "1100 0011"	DATA0 	Even-numbered data packet
#define	USB10_PID_DATA1		0b01001011	// "1101 0010"	DATA1  	Odd-numbered data packet
#define	USB10_PID_IN		0b01101001	// "1001 0110"	IN	Address for device-to-host transfer
#define	USB10_PID_OUT		0b11100001	// "1000 0111"	OUT	Address for host-to-device transfer 
#define	USB10_PID_ACK		0b11010010	// "0100 1011"	ACK	Data packet accepted	
#define	USB10_PID_NAK		0b01011010	// "0101 1010"	NAK	Data packet not accepted; please retransmit 
#define	USB10_PID_STALL		0b00011110	// "0111 1010"	STALL	Transfer impossible; do error recovery 

#define	MAX_ADDRESSES		4		// How many devices should be supported (consumes memory)
#define	MAX_ENDPOINTS		16		// How many endpoints per device

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
	volatile uint32_t RX_STATUS2;
} USB10_Reg;

typedef struct {
	uint8_t bmRequestType;	// D7: 0 - host->device, D6-5: 00 - standard, 01 - class, D4-0 - recipient: 0 - Device, 1 - Interface
	uint8_t bRequest;
	uint16_t wValue;	// Extra parameter to the request 
	uint16_t bIndex;	// Endpoint number, Config number, Interface number
	uint16_t wLength;
} USB10_SetupRequest;

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

typedef struct {
	uint8_t bLength;		// 1 Descriptor size in bytes
	uint8_t bDescriptorType;	// 1 Descriptor type = INTERFACE ASSOCIATION (0Bh)
	uint8_t bFirstInterface;	// 1 Number identifying the first interface associated with the function
	uint8_t bInterfaceCount;	// 1 The number of contiguous interfaces associated with the function
	uint8_t bFunctionClass;		// 1 Class code
	uint8_t bFunctionSubClass;	// 1 Subclass code
	uint8_t bFunctionProtocol;	// 1 Protocol code
	uint8_t iFunction;		// 1 Index of string descriptor for the function
} USB10_InterfaceAssociation;

typedef struct {
	uint8_t bLength;		// 1 Length of this descriptor = 9 bytes
	uint8_t bDescriptorType;	// 1 Descriptor type = INTERFACE (04h)
	uint8_t bInterfaceNumber;	// 1 Zero based index of this interface
	uint8_t bAlternateSetting;	// 1 Alternate setting value
	uint8_t bNumEndpoints;		// 1 Number of endpoints used by this interface (not including EP0)
	uint8_t bInterfaceClass;	// 1 Interface class
	uint8_t bInterfaceSubclass;	// 1 Interface subclass
	uint8_t bInterfaceProtocol;	// 1 Interface protocol
	uint8_t iInterface;		// 1 Index to string describing this interface
} USB10_Interface;

typedef struct {
	uint8_t bLength;		// 1 Length of this descriptor = 9 bytes
	uint8_t bDescriptorType;	// 1 Descriptor type = CONFIGURATION (02h)
	uint16_t wTotalLength;		// 2 Total length including interface and endpoint descriptors
	uint8_t bNumInterfaces;		// 1 Number of interfaces in this configuration
	uint8_t bConfigurationValue;	// 1 Configuration value used by SET_CONFIGURATION to select this configuration
	uint8_t iConfiguration;		// 1 Index of string that describes this configuration
	uint8_t bmAttributes;		// 1 Bit 7: Reserved (set to 1), Bit 6: Self-powered, Bit 5: Remote wakeup
	uint8_t bMaxPower;		// 1 Maximum power required for this configuration (in 2 mA units)
} USB10_Configuration;

typedef struct {
	uint8_t bLength;		// 1 Length of this descriptor = 7 bytes
	uint8_t bDescriptorType;	// 1 Descriptor type = ENDPOINT (05h)
	uint8_t bEndpointAddress;	// 1
	      // Bit 3...0: The endpoint number
	      // Bit 6...4: Reserved, reset to zero
	      // Bit 7: Direction. Ignored for Control
	      //	0 = OUT endpoint
	      //	1 = IN endpoint
	uint8_t bmAttributes;		// 1
	      // Bits 1..0: Transfer Type
	      //	00 = Control
	      //	01 = Isochronous
	      //	10 = Bulk
	      //	11 = Interrupt
	      // If not an isochronous endpoint, bits 5...2 are reserved and must be
	      // set to zero. If isochronous, they are defined as follows:
	      // Bits 3..2: Synchronization Type
	      //	00 = No Synchronization
	      //	01 = Asynchronous
	      //	10 = Adaptive
	      //	11 = Synchronous
	      // Bits 5..4: Usage Type
	      //	00 = Data endpoint
	      //	01 = Feedback endpoint
	      //	10 = Implicit feedback Data endpoint
	      //	11 = Reserved
	uint16_t wMaxPacketSize;	// 2 Maximum packet size for this endpoint
	uint8_t bInterval;		// 1 Polling interval in milliseconds for interrupt endpoints
		// (1 for isochronous endpoints, ignored for control or bulk)
} USB10_Endpoint;

typedef struct {
	USB10_Configuration conf;
	USB10_Interface iface;
	USB10_Endpoint endp;
} USB10_Config;

typedef union {
	USB10_Config conf;
	uint32_t data[6];	
} USB10_ConfigurationUnion;

#pragma pack(0)

extern uint8_t usb10_descr_req[];	// array of data for Description request
extern uint8_t usb10_config_req[];	// array of data for Config request
extern uint8_t usb10_setaddr_req[];	// array of data for Set Address request
extern uint8_t usb10_setconf_req[];	// array of data for Set Config request
extern uint8_t usb10_device_address;	// last used device address

extern USB10_DescriptionUnion usb10_descr_resp;
extern USB10_ConfigurationUnion usb10_config_resp;

int usb10_wait_cmd_complete(USB10_Reg* reg, int timeout); 
int usb10_bus_reset(USB10_Reg* reg, int wait_us);
int usb10_device_setup_request(USB10_Reg* reg, uint8_t address, uint8_t *request_data,
	uint8_t* response_data, uint32_t response_size);
int usb10_device_get_description(USB10_Reg* reg, uint8_t address, USB10_DescriptionUnion* descr_resp);
int usb10_device_get_config(USB10_Reg* reg, uint8_t address, USB10_ConfigurationUnion* config_resp);
int usb10_device_set_address(USB10_Reg* reg, uint8_t address_old, uint8_t address_new);
int usb10_device_set_address(USB10_Reg* reg, uint8_t address, uint8_t config_num);

/* NOTE: descr_resp and config_resp can be NULL pointers if response is unused. */
int usb10_scan(USB10_Reg* reg, uint8_t* new_device_address, USB10_DescriptionUnion **usb10_descr_resp,
	USB10_ConfigurationUnion **usb10_config_resp);

/* NOTE: request_data buffer should be at least 8 bytes long, even if request_size is zero !!! */
int usb10_device_in_request(USB10_Reg* reg, uint8_t address, uint8_t endpoint,
	uint8_t* response_data, uint32_t response_size);

int usb10_hid_set_led(USB10_Reg* reg, uint8_t address, uint8_t endpoint, uint8_t leds);

static inline void sub10_write_reg(volatile uint32_t* reg, uint32_t val) {
	asm volatile ("sw %0, (%1)" :  : "r"(val), "r"(reg));
}

static inline uint32_t usb10_read_reg(volatile uint32_t* reg) {
	return *reg;
}

#endif /* __USB10_H__ */

