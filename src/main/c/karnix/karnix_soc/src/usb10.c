#include "usb10.h"
#include "utils.h"

#if(USB10_DEBUG)
	#define	usb10_printf(format, ...)	{printf(format, __VA_ARGS__);}
#else
	#define	usb10_printf(format, ...)	{ }
#endif

USB10_DescriptionUnion usb_dev_desc = {0};

int usb10_wait_cmd_complete(USB10_Reg* reg, int timeout) {

	int i;

	// Make sure command execution has begun
	for(i = 0; i < 100; i++)
	       if(reg->COMMAND & USB10_CMD_START_BIT)
		       break;

	// Wait for execution to complete
	for(i = 0; i < timeout; i++)
	       if(!(reg->STATUS & USB10_STATUS_BUSY_BIT))
			return 1;

	return 0;
}


/*
getdesc:                        ; get device descriptor of (0,0)
        outb 0x80               ; SYNC
        outb 0x2d               ; PID
        outb 0x00               ; ADDR:ENDP = 0:0
        outb 0x10               ; + CRC5
        out4 0x03               ; EOP
        ; outb 0x01             ; ADDR:ENDP = 1:0
        ; outb 0xe8             ; + CRC5
        ; out4 0x03             ; EOP

        outb 0x80               ; SYNC
        outb 0xc3               ; PID=DATA0
        outb 0x80               ; bmRequestType: 80
        outb 0x06               ; bRequest=6 Get_Descriptor
        outb 0x00               ; Desc Index: 0
        outb 0x01               ; Desc Type: 1 device
        outb 0x00               ; Language ID: 0
        outb 0x00               ; 
        outb 0x12               ; wLength = 18
        outb 0x00
        outb 0xE0               ; CRC16
        outb 0xF4
        out4 0x03               ; EOP


getconfig:                      ; get config descriptor of (0,0)
        outb 0x80               ; SYNC
        outb 0x2d               ; PID
        outb 0x00               ; ADDR:ENDP = 0:0
        outb 0x10               ; + CRC5
        out4 0x03               ; EOP

        outb 0x80               ; SYNC
        outb 0xc3               ; PID=DATA0
        outb 0x80               ; bmRequestType: 0
        outb 0x06               ; bRequest=6 Get_Descriptor
        outb 0x00               ; Desc Index: 0
        outb 0x02               ; Desc Type: 2 configuration
        outb 0x00               ; Language ID: 0
        outb 0x00               ; 
        outb 0x18               ; wLength = 24 (9 for config, 15 for first interface)
        outb 0x00
        outb 0xa2               ; CRC16
        outb 0x54
        out4 0x03               ; EOP
        ret


*/

#define	USB10_DEVICE_RESET_STR	"usb10_device_reset"

int usb10_device_reset(USB10_Reg* reg, int wait_us)
{

	if(reg->STATUS & USB10_STATUS_ERROR_BIT) {
		printf("%s: not device connected!\r\n", USB10_DEVICE_RESET_STR);
		return -1;
	}

	// RESET

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET(USB10_CMD_BUS_RESET);
			
	if(!usb10_wait_cmd_complete(reg, 300000)) { // ~30ms timeout

		printf("%s: hung in RESET!\r\n", USB10_DEVICE_RESET_STR);
		return -2;
	}


	if(wait_us)
		delay_us(wait_us); // Usually wait for 20 ms for device to settle 

	return 0;
}


#define	USB10_DEVICE_GET_DESCR_STR	"usb10_device_get_description"

int usb10_device_get_description(USB10_Reg* reg)
{

	// Get Description: SETUP
	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_SETUP) |
			USB10_CMD_SET_ADDR(0) |
			USB10_CMD_SET_ENDP(0) |
			USB10_CMD_SET(USB10_CMD_SEND_TOKEN);

	if(!usb10_wait_cmd_complete(reg, 20000)) { // ~2ms timeout
		printf("%s: hang after SETUP token!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -1;
	}

	// Send DATA0: Get Description

	reg->SEND_DATA_LOW = 0x01000680;
	reg->SEND_DATA_HIGH = 0x00120000;
	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_LEN(8*8-1) |
			USB10_CMD_SET_PID(USB10_PID_DATA0) |
			USB10_CMD_SET(USB10_CMD_SEND_DATA);

	if(!usb10_wait_cmd_complete(reg, 20000)) { // ~2ms timeout
		printf("%s: hang after DATA0 packet!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -2;
	}

	if(!wait_bit_set_timeout(&reg->STATUS, USB10_STATUS_RECEIVED_BIT, 20000)) { /// 2ms
		printf("%s: No response after SETUP/DATA0!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -3;
	}

	printf("%s: packet received (0), PID = 0x%02X, RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
		USB10_DEVICE_GET_DESCR_STR, USB10_STATUS_PID(reg->STATUS),
		reg->RX_STATUS, reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
	);

	if(USB10_STATUS_PID(reg->STATUS) != USB10_PID_ACK) {
		printf("%s: No ACK!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -4;
	}



	// Request First part of Descriptor

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_IN) |
			USB10_CMD_SET_ADDR(0) |
			USB10_CMD_SET_ENDP(0) |
			USB10_CMD_SET(USB10_CMD_SEND_TOKEN);
			
	if(!wait_bit_set_timeout(&reg->STATUS, USB10_STATUS_RECEIVED_BIT, 20000)) { /// 2ms
		printf("%s: No response after IN(0:0)!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -5;
	}

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_ACK) |
			USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
	if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
		printf("%s: hang after ACK!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -6;
	}

	printf("%s: packet received (1), PID = 0x%02X, RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
		USB10_DEVICE_GET_DESCR_STR, USB10_STATUS_PID(reg->STATUS), reg->RX_STATUS,
		reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
	);


	if(USB10_STATUS_PID(reg->STATUS) != USB10_PID_DATA0 && 
	   USB10_STATUS_PID(reg->STATUS) != USB10_PID_DATA1) {
		printf("%s: expected DATA0/1 packet instead!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -7;
	}

	if(USB10_RX_STATUS_LEN(reg->RX_STATUS) != USB10_LOW_SPEED_PACKET_SIZE) {
		printf("%s: received bogus data packet size: %d bits != %d\r\n",
			USB10_DEVICE_GET_DESCR_STR, USB10_RX_STATUS_LEN(reg->RX_STATUS),
			USB10_LOW_SPEED_PACKET_SIZE
		);
		return -8;
	}

	usb_dev_desc.data[0] = reg->RECV_DATA_LOW;
	usb_dev_desc.data[1] = reg->RECV_DATA_HIGH;


	// Request Second part of Descriptor

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_IN) |
			USB10_CMD_SET_ADDR(0) |
			USB10_CMD_SET_ENDP(0) |
			USB10_CMD_SET(USB10_CMD_SEND_TOKEN);
			
	if(!wait_bit_set_timeout(&reg->STATUS, USB10_STATUS_RECEIVED_BIT, 20000)) { /// 2ms
		printf("%s: No response after ACK!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -9;
	}

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_ACK) |
			USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
	if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
		printf("%s: hang after ACK!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -10;
	}

	printf("%s: packet received (2), PID = 0x%02X, RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
		USB10_DEVICE_GET_DESCR_STR, USB10_STATUS_PID(reg->STATUS),
		reg->RX_STATUS, reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
	);

	if(USB10_STATUS_PID(reg->STATUS) != USB10_PID_DATA0 && 
	   USB10_STATUS_PID(reg->STATUS) != USB10_PID_DATA1) {
		printf("%s: expected DATA0/1 packet instead!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -11;
	}

	if(USB10_RX_STATUS_LEN(reg->RX_STATUS) != USB10_LOW_SPEED_PACKET_SIZE) {
		printf("%s: received bogus data packet size: %d bits != %d\r\n",
			USB10_DEVICE_GET_DESCR_STR,
			USB10_RX_STATUS_LEN(reg->RX_STATUS), USB10_LOW_SPEED_PACKET_SIZE
		);
		return -12;
	}

	usb_dev_desc.data[2] = reg->RECV_DATA_LOW;
	usb_dev_desc.data[3] = reg->RECV_DATA_HIGH;

	// Do not request third part of Descriptor as many devices do not provide it during setup.


	printf("%s: Device description received: bLength = %d\r\n"
	       "	VID:PID:bcdDev = 0x%04X:0x%04X:0x%04X, Class:subClass = 0x%04X:0x%04X, Protocol = 0x%02X\r\n",
		USB10_DEVICE_GET_DESCR_STR,
		usb_dev_desc.descr.bLength,
		usb_dev_desc.descr.idVendor, usb_dev_desc.descr.idProduct,
		usb_dev_desc.descr.bcdDevice,
		usb_dev_desc.descr.bDeviceClass, usb_dev_desc.descr.bDeviceSubClass,
		usb_dev_desc.descr.bDeviceProtocol);

	return 0;
}

