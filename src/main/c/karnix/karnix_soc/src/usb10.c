#include "usb10.h"
#include "utils.h"
#include "soc.h"
#include "riscv.h"

//#define	USB10_DEBUG_MORE	1

#if(USB10_DEBUG)
	#define	usb10_printf(...)	{printf( __VA_ARGS__);}
#else
	#define	usb10_printf(...)	{ }
#endif

USB10_DescriptionUnion usb10_descr_resp = {0};
USB10_ConfigurationUnion usb10_config_resp = {0};

uint8_t usb10_descr_req[] = {0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x12, 0x00};
uint8_t usb10_config_req[] = {0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x19, 0x00};
uint8_t usb10_setaddr_req[] = {0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t usb10_setconf_req[] = {0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t usb10_device_address = 0;

uint8_t usb10_last_data_pid = USB10_PID_DATA0;

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


#define	USB10_DEVICE_SETUP_REQUEST_STR	"usb10_device_setup_request"

int usb10_device_setup_request(USB10_Reg* reg, uint8_t address, uint8_t *request_data,
	uint8_t* response_data, uint32_t response_size)
{

	// Wait till bus is free
	if(!usb10_wait_while_busy(reg, 10000)) { // ~1ms timeout
		usb10_printf("%s: bus is busy for too long!\r\n", USB10_DEVICE_SETUP_REQUEST_STR);
		return -1;
	}

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during I/O 

	uint32_t rx_status;
	int timeout;
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
		usb10_printf("%s: hang after SETUP token!\r\n", USB10_DEVICE_SETUP_REQUEST_STR);
		ret = -2;
		goto fail;
	}

	// Send DATA0: 
	reg->SEND_DATA_LOW = *(uint32_t*)(request_data + 0);
	reg->SEND_DATA_HIGH = *(uint32_t*)(request_data + 4);

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_LEN(8*8-1) |
			USB10_CMD_SET_PID(USB10_PID_DATA0) |
			USB10_CMD_SET(USB10_CMD_SEND_DATA);

	usb10_last_data_pid = USB10_PID_DATA0;

	if(!usb10_wait_cmd_complete(reg, 5000)) { // ~0.5ms timeout
		usb10_printf("%s: hang after DATA0 packet!\r\n", USB10_DEVICE_SETUP_REQUEST_STR);
		ret = -3;
		goto fail;
	}

	timeout = 2500;
	while(timeout--)
		if(reg->STATUS & USB10_STATUS_RECEIVED_BIT)
			goto rcvd1;

	{
		usb10_printf("%s: No response after first SETUP/DATA!\r\n", USB10_DEVICE_SETUP_REQUEST_STR);

		if(--retry)
			goto again_setup;

		ret = -4;
		goto fail;
	}

	rcvd1:

	rx_status = reg->RX_STATUS; // read once, use many times

	if(USB10_RX_STATUS_PID(rx_status) != USB10_PID_ACK) {
		usb10_printf("%s: expected %s packet instead of 0x%02X\r\n", USB10_DEVICE_SETUP_REQUEST_STR, "ACK",
			USB10_RX_STATUS_PID(rx_status));

		if(--retry)
			goto again_setup;

		ret = -5;
		goto fail;
	}


	int num_of_data_packets = response_size % USB10_LOW_SPEED_DATA_SIZE ?
		response_size / USB10_LOW_SPEED_DATA_SIZE + 1 : response_size / USB10_LOW_SPEED_DATA_SIZE;

	if(response_size == 0)
		num_of_data_packets = 1; // read at least one data packet

	for(int i = 0; i < num_of_data_packets; i++) {

		// Request part of response data

		retry = 8;

		int packet_size_bits = (i + 1) * USB10_LOW_SPEED_DATA_SIZE <= response_size ?
				(USB10_LOW_SPEED_DATA_SIZE * 8 + 8 + 16) : // PID8 + DATA*8 + CRC16 
				(response_size % USB10_LOW_SPEED_DATA_SIZE) * 8 + 8 + 16; 

		again_data:

		reg->COMMAND = USB10_CMD_START_BIT |
				USB10_CMD_SET_PID(USB10_PID_IN) |
				USB10_CMD_SET_ADDR(address) |
				USB10_CMD_SET_ENDP(0) |
				USB10_CMD_SET(USB10_CMD_SEND_TOKEN);
				
		timeout = 2500;
		while(timeout--)
			if(reg->STATUS & USB10_STATUS_RECEIVED_BIT)
				goto rcvd2;

		{
			usb10_printf("%s: No response after IN(%d:%d)\r\n", USB10_DEVICE_SETUP_REQUEST_STR, 0, 0);

			if(retry--)
				goto again_data;

			ret = -6;
			goto fail;
		}
		
		rcvd2:

		rx_status = reg->RX_STATUS; // read once, use many times

		// ACK received packet no matter what it is

		reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_ACK) |
			USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
		if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
			usb10_printf("%s: hang after ACK!\r\n", USB10_DEVICE_SETUP_REQUEST_STR);
			ret = -7;
			goto fail;
		}

		// Now analyze what we've got here

		int received_pid = USB10_RX_STATUS_PID(rx_status);

		if(received_pid != USB10_PID_DATA0 && received_pid != USB10_PID_DATA1) {
			//usb10_printf("%s: expected %s packet instead of 0x%02X\r\n", USB10_DEVICE_SETUP_REQUEST_STR, "DATA",
			//	USB10_RX_STATUS_PID(rx_status));

			if(retry--)
				goto again_data;

			ret = -8;
			goto fail;
		}

		if(received_pid == usb10_last_data_pid) {

			usb10_printf("%s: received %d dupe data, STATUS = %08X, DATA = %08X:%08X\r\n",
				USB10_DEVICE_SETUP_REQUEST_STR, i, USB10_RX_STATUS_LEN(rx_status),
				reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
			);

			if(retry--)
				goto again_data;

			ret = -9;
			goto fail;
		}

		if(USB10_RX_STATUS_LEN(rx_status) != packet_size_bits) {
			usb10_printf("%s: received bogus data packet (%d) size: %d bits != %d, DATA = %08X:%08X\r\n",
				USB10_DEVICE_SETUP_REQUEST_STR, i, USB10_RX_STATUS_LEN(rx_status), packet_size_bits,
				reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
			);

			if(retry--)
				goto again_data;

			ret = -10;
			goto fail;
		}

		usb10_printf("%s: packet received (%d), RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
			USB10_DEVICE_SETUP_REQUEST_STR, i, rx_status, reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
		);

		usb10_last_data_pid = received_pid;

		if(response_size) { // collect data if response expected
			*(uint32_t*)(response_data + i * 8 + 0) = reg->RECV_DATA_LOW;
			*(uint32_t*)(response_data + i * 8 + 4) = reg->RECV_DATA_HIGH;
		}

	}


	usb10_printf("%s: RX DATA: ", USB10_DEVICE_SETUP_REQUEST_STR);

	for(int i = 0; i < response_size; i++)
		usb10_printf("%02X ", response_data[i]);

	usb10_printf("\r\n", 0);
	

	fail:
	
	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	return ret;
}


#define	USB10_DEVICE_IN_REQUEST_STR	"usb10_device_in_request"

int usb10_device_in_request(USB10_Reg* reg, uint8_t address, uint8_t endpoint,
	uint8_t* response_data, uint32_t response_size)
{

	// Wait till bus is free
	if(!usb10_wait_while_busy(reg, 10000)) { // ~1ms timeout
		usb10_printf("%s: bus is busy for too long!\r\n", USB10_DEVICE_IN_REQUEST_STR);
		return -1;
	}

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during I/O 

	int ret = 0;

	int num_of_data_packets = response_size % USB10_LOW_SPEED_DATA_SIZE ?
		response_size / USB10_LOW_SPEED_DATA_SIZE + 1 : response_size / USB10_LOW_SPEED_DATA_SIZE;

	if(response_size == 0)
		num_of_data_packets = 1; // read at least one data packet

	for(int i = 0; i < num_of_data_packets; i++) {

		// Request part of response data

		uint32_t rx_status;
		int timeout;
		int retry = 8;

		int packet_size_bits = (i + 1) * USB10_LOW_SPEED_DATA_SIZE  <= response_size ?
				(USB10_LOW_SPEED_DATA_SIZE * 8 + 8 + 16) : // PID8 + DATA*8 + CRC16 
				(response_size % USB10_LOW_SPEED_DATA_SIZE) * 8 + 8 + 16; 

		again_data:

		reg->COMMAND = USB10_CMD_START_BIT |
				USB10_CMD_SET_PID(USB10_PID_IN) |
				USB10_CMD_SET_ADDR(address) |
				USB10_CMD_SET_ENDP(endpoint) |
				USB10_CMD_SET(USB10_CMD_SEND_TOKEN);
				
		timeout = 2500;
		while(timeout--)
			if(reg->STATUS & USB10_STATUS_RECEIVED_BIT)
				goto rcvd2;

		{
			usb10_printf("%s: No response after IN(%d:%d)\r\n", USB10_DEVICE_IN_REQUEST_STR,
				address, endpoint);

			if(retry--)
				goto again_data;

			ret = -6;
			goto fail;
		}
		
		rcvd2:

		rx_status = reg->RX_STATUS; // read once, use many times

		// ACK received packet no matter what it is

		reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_ACK) |
			USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
		if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
			usb10_printf("%s: hang after ACK!\r\n", USB10_DEVICE_IN_REQUEST_STR);
			ret = -7;
			goto fail;
		}

		// Now analyze what we've got here

		int received_pid = USB10_RX_STATUS_PID(rx_status);

		if(received_pid == USB10_PID_NAK) {
			ret = -8;
			goto fail;
		}

		if(received_pid != USB10_PID_DATA0 && received_pid != USB10_PID_DATA1) {
			//usb10_printf("%s: expected %s packet instead of 0x%02X\r\n", USB10_DEVICE_IN_REQUEST_STR, "DATA",
			//	USB10_RX_STATUS_PID(rx_status));

			if(retry--)
				goto again_data;

			ret = -9;
			goto fail;
		}

		if(received_pid == usb10_last_data_pid) {

			usb10_printf("%s: received %d dupe data, STATUS = %08X, DATA = %08X:%08X\r\n",
				USB10_DEVICE_IN_REQUEST_STR, i, USB10_RX_STATUS_LEN(rx_status),
				reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
			);

			if(retry--)
				goto again_data;

			ret = -10;
			goto fail;
		}

		if(USB10_RX_STATUS_LEN(rx_status) != packet_size_bits) {
			usb10_printf("%s: received bogus data packet (%d) size: %d bits != %d, DATA = %08X:%08X\r\n",
				USB10_DEVICE_IN_REQUEST_STR, i, USB10_RX_STATUS_LEN(rx_status), packet_size_bits,
				reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
			);

			if(retry--)
				goto again_data;

			ret = -11;
			goto fail;
		}

		usb10_printf("%s: packet received (%d), RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
			USB10_DEVICE_IN_REQUEST_STR, i, rx_status, reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
		);

		usb10_last_data_pid = received_pid;

		if(response_size) { // collect data if response expected
			*(uint32_t*)(response_data + i * 8 + 0) = reg->RECV_DATA_LOW;
			*(uint32_t*)(response_data + i * 8 + 4) = reg->RECV_DATA_HIGH;
		}

	}


	usb10_printf("%s: RX DATA: ", USB10_DEVICE_IN_REQUEST_STR);

	for(int i = 0; i < response_size; i++)
		usb10_printf("%02X ", response_data[i]);

	usb10_printf("\r\n", 0);
	

	fail:
	
	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	return ret;
}


#define	USB10_DEVICE_GET_DESCR_STR	"usb10_device_get_description"

int usb10_device_get_description(USB10_Reg* reg, uint8_t address, USB10_DescriptionUnion** descr_resp)
{
	int ret = usb10_device_setup_request(USB1, address, usb10_descr_req, (uint8_t*)&usb10_descr_resp, 18);

	usb10_printf("%s: usb10_device_setup_request ret = %d\r\n", USB10_DEVICE_GET_DESCR_STR, ret);

	if(ret)
		return ret;

	if(descr_resp)
		*descr_resp = &usb10_descr_resp;	
	
	return 0;
}


#define	USB10_DEVICE_GET_CONFIG_STR	"usb10_device_get_config"

int usb10_device_get_config(USB10_Reg* reg, uint8_t address, USB10_ConfigurationUnion** config_resp)
{
	int ret = usb10_device_setup_request(USB1, address, usb10_config_req, (uint8_t*)&usb10_config_resp, 25);

	usb10_printf("%s: usb10_device_setup_request ret = %d\r\n", USB10_DEVICE_GET_CONFIG_STR, ret);

	if(ret)
		return ret;

	if(config_resp)
		*config_resp = &usb10_config_resp;	
	
	return 0;
}


#define	USB10_DEVICE_SET_ADDR_STR	"usb10_device_set_address"

int usb10_device_set_address(USB10_Reg* reg, uint8_t address_old, uint8_t address_new)
{
	usb10_setaddr_req[2] = address_new; // Device address

	uint8_t usb10_skip_resp[2];

	int ret = usb10_device_setup_request(USB1, address_old, usb10_setaddr_req, usb10_skip_resp, 0);

	usb10_printf("%s: usb10_device_setup_request ret = %d\r\n", USB10_DEVICE_SET_ADDR_STR, ret);

	return ret;
}


#define	USB10_DEVICE_SET_CONF_STR	"usb10_device_set_config"

int usb10_device_set_config(USB10_Reg* reg, uint8_t address, uint8_t config_num)
{
	usb10_setconf_req[2] = config_num;

	uint8_t usb10_skip_resp[2];

	int ret = usb10_device_setup_request(USB1, address, usb10_setconf_req, usb10_skip_resp, 0);

	usb10_printf("%s: usb10_device_setup_request ret = %d\r\n", USB10_DEVICE_SET_CONF_STR, ret);

	return ret;
}


#define	USB10_SCAN_STR	"usb10_scan"

int usb10_scan(USB10_Reg* reg, uint8_t* new_device_address, USB10_DescriptionUnion **descr_resp,
	USB10_ConfigurationUnion **config_resp)
{
	/* NOTE: descr_resp and config_resp can be NULL pointers, so do not use
	         these here, use global data structures usb10_descr_resp and usb10_config_resp instead! */

	int ret = 0;

	usb10_printf("%s: Scanning for devices...\r\n", USB10_SCAN_STR);

	if((ret = usb10_bus_reset(USB1, 12000)) < 0)
		goto usb10_error;

	if((ret = usb10_device_get_description(USB1, 0, descr_resp)) < 0)
		goto usb10_error;

	usb10_printf("%s: Device detected: bLength = %d, VID/PID = 0x%04X/0x%04X,\r\n"
		"\tclass/subclass = 0x%02X/0x%02X, bcdUSB = 0x%04X\r\n",
		USB10_SCAN_STR,
		usb10_descr_resp.descr.bLength,
		usb10_descr_resp.descr.idVendor,
		usb10_descr_resp.descr.idProduct,
		usb10_descr_resp.descr.bDeviceClass,
		usb10_descr_resp.descr.bDeviceSubClass,
		usb10_descr_resp.descr.bcdUSB);

	if((ret = usb10_bus_reset(USB1, 12000)) < 0)
		goto usb10_error;

	int device_address = (usb10_device_address + 1) % 7;

	if((ret = usb10_device_set_address(USB1, 0, device_address)) < 0)
		goto usb10_error;

	if((ret = usb10_device_get_config(USB1, device_address, config_resp)) < 0)
		goto usb10_error;
	
	if((ret = usb10_device_set_config(USB1, device_address, 1)) < 0) // set active config to 1
		goto usb10_error;

	usb10_device_address = device_address;	

	usb10_printf("%s: Device addr = %d, Config: bLength = %d\r\n"
		"\tEPAddress = 0x%02X, Interval = %d ms, MaxPacketSize = %d,\r\n"
		"\tbInterfaceClass/Subclass/Protocol = %d/%d/%d\r\n",
		USB10_SCAN_STR,
		usb10_device_address,
		usb10_config_resp.conf.conf.bLength,
		usb10_config_resp.conf.endp.bEndpointAddress,
		usb10_config_resp.conf.endp.bInterval,
		usb10_config_resp.conf.endp.wMaxPacketSize,
		usb10_config_resp.conf.iface.bInterfaceClass,
		usb10_config_resp.conf.iface.bInterfaceSubclass,
		usb10_config_resp.conf.iface.bInterfaceProtocol);
		
		goto ok;
	
	usb10_error: 
		usb10_device_address = 0; // this can be used as flag to indicate b10_device_addressevice is ready

	ok:
		// Fill-in returning data structures 

		if(new_device_address)
			*new_device_address = usb10_device_address;

		if(descr_resp)
			*descr_resp = &usb10_descr_resp;

		if(config_resp)
			*config_resp = &usb10_config_resp;

	return ret;
}

