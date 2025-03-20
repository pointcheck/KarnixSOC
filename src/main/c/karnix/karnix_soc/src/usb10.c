#include "usb10.h"
#include "utils.h"
#include "soc.h"
#include "riscv.h"

//#define	USB10_DEBUG_MORE	1

#if(USB10_DEBUG)
	#define	usb10_printf(format, ...)	{printf(format, __VA_ARGS__);}
#else
	#define	usb10_printf(format, ...)	{ }
#endif

USB10_DescriptionUnion usb_dev_desc = {0};

int usb10_wait_cmd_complete(USB10_Reg* reg, int timeout) {

	int i;

	// Make sure command execution has begun
	for(i = 0; i < 50; i++)
	       if(reg->STATUS & USB10_STATUS_BUSY_BIT)
		       break;

	// Wait for execution to complete
	while(timeout--)
	       if((reg->STATUS & USB10_STATUS_BUSY_BIT) == 0)
			return timeout; // OK

	return 0; // Fail
}


int usb10_wait_while_busy(USB10_Reg* reg, int timeout) {

	int i;

	// Wait for execution to complete
	while(timeout--)
	       if((reg->STATUS & USB10_STATUS_BUSY_BIT) == 0)
			return timeout;

	return 0;
}


#define	USB10_DEVICE_RESET_STR	"usb10_device_reset"

int usb10_bus_reset(USB10_Reg* reg, int wait_us)
{

	if(reg->STATUS & USB10_STATUS_ERROR_BIT) {
		usb10_printf("%s: device not connected!\r\n", USB10_DEVICE_RESET_STR);
		return -1;
	}

	// RESET

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET(USB10_CMD_BUS_RESET);
			
	if(!usb10_wait_cmd_complete(reg, 600000)) { // ~30ms timeout
		usb10_printf("%s: hung in RESET!\r\n", USB10_DEVICE_RESET_STR);
		return -2;
	}


	if(wait_us)
		delay_us(wait_us); // Usually wait for 20 ms for device to settle 

	return 0;
}

#define	USB10_DEVICE_GET_DESCR_STR	"usb10_device_get_description"

int usb10_device_get_description(USB10_Reg* reg, int address)
{

	// Wait till bus is free
	if(!usb10_wait_while_busy(reg, 10000)) { // ~1ms timeout
		usb10_printf("%s: bus is busy for too long!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -15;
	}

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init

	int retry = 3;
	int ret = 0;

	again_setup:

	// SETUP I/O with 0:0
	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_SETUP) |
			USB10_CMD_SET_ADDR(address) |
			USB10_CMD_SET_ENDP(0) |
			USB10_CMD_SET(USB10_CMD_SEND_TOKEN);

	if(!usb10_wait_cmd_complete(reg, 20000)) { // ~2ms timeout
		usb10_printf("%s: hang after SETUP token!\r\n", USB10_DEVICE_GET_DESCR_STR);
		ret = -1;
		goto fail;
	}

	// Send DATA0: Get Description
	reg->SEND_DATA_LOW = 0x01000680;
	reg->SEND_DATA_HIGH = 0x00120000;

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_LEN(8*8-1) |
			USB10_CMD_SET_PID(USB10_PID_DATA0) |
			USB10_CMD_SET(USB10_CMD_SEND_DATA);

	if(!usb10_wait_cmd_complete(reg, 5000)) { // ~0.5ms timeout
		usb10_printf("%s: hang after DATA0 packet!\r\n", USB10_DEVICE_GET_DESCR_STR);
		ret = -2;
		goto fail;
	}

	if(!wait_bit_set_timeout(&reg->STATUS, USB10_STATUS_RECEIVED_BIT, 2500)) { /// 0.5ms
		usb10_printf("%s: No response after first SETUP/DATA!\r\n", USB10_DEVICE_GET_DESCR_STR);

		if(--retry)
			goto again_setup;

		ret = -4;
		goto fail;
	}


	#if(USB10_DEBUG_MORE)
	usb10_printf("%s: packet received (0), PID = 0x%02X, RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
		USB10_DEVICE_GET_DESCR_STR, USB10_STATUS_PID(reg->STATUS),
		reg->RX_STATUS, reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
	);
	#endif

	if(USB10_STATUS_PID(reg->STATUS) != USB10_PID_ACK) {
		usb10_printf("%s: expected %s packet instead of 0x%02X\r\n", USB10_DEVICE_GET_DESCR_STR, "ACK",
			USB10_STATUS_PID(reg->STATUS));
		ret = -5;
		goto fail;
	}

	// Request First part of Descriptor

	retry = 8;

	again_first_descr:

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_IN) |
			USB10_CMD_SET_ADDR(address) |
			USB10_CMD_SET_ENDP(0) |
			USB10_CMD_SET(USB10_CMD_SEND_TOKEN);
			
	if(!wait_bit_set_timeout(&reg->STATUS, USB10_STATUS_RECEIVED_BIT, 20000)) { /// 2ms
		usb10_printf("%s: No response after IN(0:0)!\r\n", USB10_DEVICE_GET_DESCR_STR);

		if(retry--)
			goto again_first_descr;

		ret = -6;
		goto fail;
	}

	#if(USB10_DEBUG_MORE)
	usb10_printf("%s: packet received (1), PID = 0x%02X, RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
		USB10_DEVICE_GET_DESCR_STR, USB10_STATUS_PID(reg->STATUS), reg->RX_STATUS,
		reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
	);
	#endif


	if(USB10_STATUS_PID(reg->STATUS) != USB10_PID_DATA0 && 
	   USB10_STATUS_PID(reg->STATUS) != USB10_PID_DATA1) {
		usb10_printf("%s: expected %s packet instead of 0x%02X\r\n", USB10_DEVICE_GET_DESCR_STR, "DATA",
			USB10_STATUS_PID(reg->STATUS));

		if(retry--)
			goto again_first_descr;

		ret = -8;
		goto fail;
	}

	if(USB10_RX_STATUS_LEN(reg->RX_STATUS) != USB10_LOW_SPEED_PACKET_SIZE) {
		usb10_printf("%s: received bogus data packet size: %d bits != %d\r\n", USB10_DEVICE_GET_DESCR_STR,
			USB10_RX_STATUS_LEN(reg->RX_STATUS), USB10_LOW_SPEED_PACKET_SIZE
		);

		if(retry--)
			goto again_first_descr;

		ret = -9;
		goto fail;
	}

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_ACK) |
			USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
	if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
		usb10_printf("%s: hang after ACK!\r\n", USB10_DEVICE_GET_DESCR_STR);
		ret = -7;
		goto fail;
	}

	usb_dev_desc.data[0] = reg->RECV_DATA_LOW;
	usb_dev_desc.data[1] = reg->RECV_DATA_HIGH;


	// Request Second part of Descriptor

	retry = 8;

	again_second_descr:

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_IN) |
			USB10_CMD_SET_ADDR(address) |
			USB10_CMD_SET_ENDP(0) |
			USB10_CMD_SET(USB10_CMD_SEND_TOKEN);
			
	if(!wait_bit_set_timeout(&reg->STATUS, USB10_STATUS_RECEIVED_BIT, 20000)) { /// 2ms
		usb10_printf("%s: No response after IN(0:0)!\r\n", USB10_DEVICE_GET_DESCR_STR);

		if(retry--)
			goto again_second_descr;

		ret = -6;
		goto fail;
	}

	#if(USB10_DEBUG_MORE)
	usb10_printf("%s: packet received (2), PID = 0x%02X, RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
		USB10_DEVICE_GET_DESCR_STR, USB10_STATUS_PID(reg->STATUS), reg->RX_STATUS,
		reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
	);
	#endif


	if(USB10_STATUS_PID(reg->STATUS) != USB10_PID_DATA0 && 
	   USB10_STATUS_PID(reg->STATUS) != USB10_PID_DATA1) {
		usb10_printf("%s: expected %s packet instead of 0x%02X\r\n", USB10_DEVICE_GET_DESCR_STR, "DATA",
			USB10_STATUS_PID(reg->STATUS));

		if(retry--)
			goto again_second_descr;

		ret = -8;
		goto fail;
	}

	if(USB10_RX_STATUS_LEN(reg->RX_STATUS) != USB10_LOW_SPEED_PACKET_SIZE) {
		usb10_printf("%s: received bogus data packet size: %d bits != %d\r\n", USB10_DEVICE_GET_DESCR_STR,
			USB10_RX_STATUS_LEN(reg->RX_STATUS), USB10_LOW_SPEED_PACKET_SIZE
		);

		if(retry--)
			goto again_second_descr;

		ret = -9;
		goto fail;
	}

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_ACK) |
			USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
	if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
		usb10_printf("%s: hang after ACK!\r\n", USB10_DEVICE_GET_DESCR_STR);
		ret = -7;
		goto fail;
	}


	usb_dev_desc.data[2] = reg->RECV_DATA_LOW;
	usb_dev_desc.data[3] = reg->RECV_DATA_HIGH;

	usb10_printf("%s: Device description received: bLength = %d\r\n"
	       "	VID:PID:bcdDev = 0x%04X:0x%04X:0x%04X, Class:subClass = 0x%04X:0x%04X, Protocol = 0x%02X\r\n",
		USB10_DEVICE_GET_DESCR_STR,
		usb_dev_desc.descr.bLength,
		usb_dev_desc.descr.idVendor, usb_dev_desc.descr.idProduct,
		usb_dev_desc.descr.bcdDevice,
		usb_dev_desc.descr.bDeviceClass, usb_dev_desc.descr.bDeviceSubClass,
		usb_dev_desc.descr.bDeviceProtocol);

/*
	usb10_printf("%s: Device description received: bLength = %d, bcdDev = 0x%04X,"
			" Class:subClass = 0x%04X:0x%04X, Protocol = 0x%02X\r\n",
		USB10_DEVICE_GET_DESCR_STR,
		usb_dev_desc.descr.bLength,
		usb_dev_desc.descr.bcdDevice,
		usb_dev_desc.descr.bDeviceClass, usb_dev_desc.descr.bDeviceSubClass,
		usb_dev_desc.descr.bDeviceProtocol);
*/

	fail:
	
	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	return ret;
}


#define	USB10_DEVICE_SET_ADDR_STR	"usb10_device_set_address"

int usb10_device_set_address(USB10_Reg* reg, int address)
{
	// Wait till bus is free
	if(!usb10_wait_while_busy(reg, 10000)) { // ~1ms timeout
		usb10_printf("%s: bus is busy for too long!\r\n", USB10_DEVICE_GET_DESCR_STR);
		return -15;
	}


	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init

	int retry = 3;
	int ret = 0;

	// Get Description: SETUP
	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_SETUP) |
			USB10_CMD_SET_ADDR(0) |
			USB10_CMD_SET_ENDP(0) |
			USB10_CMD_SET(USB10_CMD_SEND_TOKEN);

	if(!usb10_wait_cmd_complete(reg, 20000)) { // ~2ms timeout
		usb10_printf("%s: hang after SETUP token!\r\n", USB10_DEVICE_SET_ADDR_STR);
		ret = -1;
		goto fail;
	}

	// Send DATA0: Set Address

	reg->SEND_DATA_LOW = 0x00000500 | (address << 16);
	reg->SEND_DATA_HIGH = 0x00000000;
	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_LEN(8*8-1) |
			USB10_CMD_SET_PID(USB10_PID_DATA0) |
			USB10_CMD_SET(USB10_CMD_SEND_DATA);

	if(!usb10_wait_cmd_complete(reg, 20000)) { // ~2ms timeout
		usb10_printf("%s: hang after DATA0 packet!\r\n", USB10_DEVICE_SET_ADDR_STR);
		ret = -2;
		goto fail;
	}

	if(!wait_bit_set_timeout(&reg->STATUS, USB10_STATUS_RECEIVED_BIT, 20000)) { /// 2ms
		usb10_printf("%s: No response after SETUP/DATA!\r\n", USB10_DEVICE_SET_ADDR_STR);
		ret = -3;
		goto fail;
	}

	#if(USB10_DEBUG_MORE)
	usb10_printf("%s: packet received (0), PID = 0x%02X, RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
		USB10_DEVICE_SET_ADDR_STR, USB10_STATUS_PID(reg->STATUS),
		reg->RX_STATUS, reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
	);
	#endif

	if(USB10_STATUS_PID(reg->STATUS) != USB10_PID_ACK) {
		usb10_printf("%s: expected %s packet instead of 0x%02X\r\n", USB10_DEVICE_SET_ADDR_STR, "ACK",
			USB10_STATUS_PID(reg->STATUS));
		ret = -4;
		goto fail;
	}


	// Make Set Address request 

	again:

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_IN) |
			USB10_CMD_SET_ADDR(0) |
			USB10_CMD_SET_ENDP(0) |
			USB10_CMD_SET(USB10_CMD_SEND_TOKEN);
			
	if(!wait_bit_set_timeout(&reg->STATUS, USB10_STATUS_RECEIVED_BIT, 20000)) { /// 2ms
		usb10_printf("%s: No response after IN(0:0)!\r\n", USB10_DEVICE_SET_ADDR_STR);

		if(retry--)
			goto again;

		ret = -5;
		goto fail;
	}

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_ACK) |
			USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
	if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
		usb10_printf("%s: hang after ACK!\r\n", USB10_DEVICE_SET_ADDR_STR);
		ret = -6;
		goto fail;
	}

	#if(USB10_DEBUG_MORE)
	usb10_printf("%s: packet received (1), PID = 0x%02X, RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
		USB10_DEVICE_SET_ADDR_STR,
		USB10_STATUS_PID(reg->STATUS), reg->RX_STATUS, reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
	);
	#endif


	if(USB10_STATUS_PID(reg->STATUS) != USB10_PID_DATA0 && 
	   USB10_STATUS_PID(reg->STATUS) != USB10_PID_DATA1) {
		usb10_printf("%s: expected %s packet instead of 0x%02X\r\n", USB10_DEVICE_SET_ADDR_STR, "ACK",
			USB10_STATUS_PID(reg->STATUS));

		if(retry--)
			goto again;

		ret =-7;
		goto fail;
	}
		
	// At this point we should have received a response of 6 bytes (24 bits), what are they ?

	usb10_printf("%s: Device address set to = %d, last DATA = %08X:%08\r\n", USB10_DEVICE_SET_ADDR_STR,
			address, reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW);

	fail:
	
	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	return ret;
}

