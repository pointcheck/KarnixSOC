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
uint8_t usb10_setreport_req[] = {0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00};
 
uint8_t usb10_device_address = 0;

struct _usb10_last_data_pid {
	uint8_t sent;
	uint8_t recv;
} usb10_last_data_pid[MAX_ADDRESSES][MAX_ENDPOINTS];


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
		usb10_printf("%s: hung after %s packet!\r\n", USB10_DEVICE_RESET_STR, "RESET");
		return -2;
	}

	for(int addr = 0; addr < MAX_ADDRESSES; addr++)
		for(int endp = 0; endp < MAX_ENDPOINTS; endp++) {
			usb10_last_data_pid[addr][endp].sent = USB10_PID_DATA1;
			usb10_last_data_pid[addr][endp].recv = USB10_PID_DATA0;
		}

	if(wait_us)
		delay_us(wait_us); // Usually wait for 20 ms for device to settle 

	return 0;
}


#define	USB10_DEVICE_SETUP_REQUEST_STR	"usb10_device_setup_request"

int usb10_device_setup_request(USB10_Reg* reg, uint8_t address, uint8_t *request_data,
	uint8_t* response_data, uint32_t response_size)
{

	if(address >= MAX_ADDRESSES)
		return -999;

	// Wait till bus is free
	if(!usb10_wait_while_busy(reg, 10000)) { // ~1ms timeout
		usb10_printf("%s: bus is busy for too long!\r\n", USB10_DEVICE_SETUP_REQUEST_STR);
		return -1;
	}

	usb10_printf("%s: request %02X %02X to addr = %d\r\n", USB10_DEVICE_SETUP_REQUEST_STR,
		request_data[0], request_data[1], address);

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during I/O 

	uint32_t rx_status;
	int timeout;
	int retry = 3;
	int ret = 0;

	usb10_last_data_pid[address][0].sent = USB10_PID_DATA1;
	usb10_last_data_pid[address][0].recv = USB10_PID_DATA0;

	again_setup:

	// Prepare DATA to be sent 
	reg->SEND_DATA_LOW = *(uint32_t*)(request_data + 0);
	reg->SEND_DATA_HIGH = *(uint32_t*)(request_data + 4);

	int data_pid = (usb10_last_data_pid[address][0].sent == USB10_PID_DATA0) ? 
		USB10_PID_DATA1 : USB10_PID_DATA0;

	// SETUP I/O with 0:0
	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_SETUP) |
			USB10_CMD_SET_ADDR(address) |
			USB10_CMD_SET_ENDP(0) |
			USB10_CMD_SET(USB10_CMD_SEND_TOKEN);

	if(!usb10_wait_cmd_complete(reg, 20000)) { // ~2ms timeout
		usb10_printf("%s: hung after %s packet!\r\n", USB10_DEVICE_SETUP_REQUEST_STR, "SETUP");
		ret = -2;
		goto fail;
	}

	reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_LEN(8*8-1) |
			USB10_CMD_SET_PID(data_pid) |
			USB10_CMD_SET(USB10_CMD_SEND_DATA);

	if(!usb10_wait_cmd_complete(reg, 5000)) { // ~0.5ms timeout
		usb10_printf("%s: hung after %s packet!\r\n", USB10_DEVICE_SETUP_REQUEST_STR, "DATA");
		ret = -3;
		goto fail;
	}

	timeout = 2500;
	while(timeout--)
		if(reg->STATUS & USB10_STATUS_RECEIVED_BIT)
			goto rcvd1;

	{
		usb10_printf("%s: no response after %s(%d:%d)\r\n", USB10_DEVICE_SETUP_REQUEST_STR,
			"DATA", address, 0);

		if(--retry)
			goto again_setup;

		ret = -4;
		goto fail;
	}

	rcvd1:

	usb10_last_data_pid[address][0].sent = data_pid; // remember last used DATA PID

	rx_status = reg->RX_STATUS; // read once, use many times

	if(USB10_RX_STATUS_PID(rx_status) != USB10_PID_ACK) {
		usb10_printf("%s: expected %s packet instead of 0x%02X, RX_STATUS = %08X\r\n", USB10_DEVICE_SETUP_REQUEST_STR, "ACK",
			USB10_RX_STATUS_PID(rx_status), rx_status);

		if(--retry)
			goto again_setup;

		ret = -5;
		goto fail;
	}


	int num_of_data_packets = response_size % USB10_LOW_SPEED_DATA_SIZE ?
		response_size / USB10_LOW_SPEED_DATA_SIZE + 1 : response_size / USB10_LOW_SPEED_DATA_SIZE;

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
			usb10_printf("%s: no response after %s(%d:%d)\r\n", USB10_DEVICE_SETUP_REQUEST_STR,
				"IN", address, 0);

			if(retry--)
				goto again_data;

			ret = -6;
			goto fail;
		}
		
		rcvd2:

		rx_status = reg->RX_STATUS; // read once, use many times

		// Note: NAK packets missing CRC16 which also considered as fail 

		#if(1)
		if(!(reg->STATUS & USB10_STATUS_CRC16_OK_BIT)) {
			usb10_printf("%s: CRC16 error (%d), STATUS = %08X, RX_STATUS = %08X, RX_STATUS2 = %08X, DATA = %08X:%08X\r\n",
				USB10_DEVICE_SETUP_REQUEST_STR, i, reg->STATUS, rx_status, reg->RX_STATUS2,
				reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
			);


			delay_us(20);

			if(retry--)
				goto again_data;

			ret = -10;
			goto fail;
		}
		#endif

		#if(0) // Enable this if packet size matching is necessary
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
		#endif

		// Send ACK for received packet

		reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_ACK) |
			USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
		if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
			usb10_printf("%s: hung after %s packet!\r\n", USB10_DEVICE_SETUP_REQUEST_STR, "ACK");
			ret = -7;
			goto fail;
		}

		// Now analyze what we've got here

		int received_pid = USB10_RX_STATUS_PID(rx_status);

		if(received_pid != USB10_PID_DATA0 && received_pid != USB10_PID_DATA1) {
			usb10_printf("%s: expected %s packet instead of 0x%02X, RX_STATUS = %08X\r\n", USB10_DEVICE_SETUP_REQUEST_STR, "DATA",
				USB10_RX_STATUS_PID(rx_status), rx_status);

			if(retry--)
				goto again_data;

			ret = -8;
			goto fail;
		}

		if(received_pid == usb10_last_data_pid[address][0].recv) {

			usb10_printf("%s: received %d dupe data, RX_STATUS = %08X, DATA = %08X:%08X\r\n",
				USB10_DEVICE_SETUP_REQUEST_STR, i, rx_status,
				reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
			);

			if(retry--)
				goto again_data;

			ret = -9;
			goto fail;
		}

		usb10_printf("%s: packet received (%d), RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
			USB10_DEVICE_SETUP_REQUEST_STR, i, rx_status, reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
		);

		usb10_last_data_pid[address][0].recv = received_pid; // Remember last received DATA PID

		if(response_size && response_data) { // collect data if response expected
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
	if(address >= MAX_ADDRESSES || endpoint >= MAX_ENDPOINTS)
		return -999;

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
				
		timeout = 25000;
		while(timeout--)
			if(reg->STATUS & USB10_STATUS_RECEIVED_BIT)
				goto rcvd2;

		{
			usb10_printf("%s: no response after %s(%d:%d)\r\n", USB10_DEVICE_IN_REQUEST_STR, "IN",
				address, endpoint);

			if(retry--)
				goto again_data;

			ret = -6;
			goto fail;
		}
		
		rcvd2:

		rx_status = reg->RX_STATUS; // read once, use many times

		int received_pid = USB10_RX_STATUS_PID(rx_status);

		if(received_pid == USB10_PID_NAK) {
			ret = -8;
			goto fail;
		}

		// Note: Some short token packets missing CRC16 which also considered as fail 

		#if(1)
		if(!(reg->STATUS & USB10_STATUS_CRC16_OK_BIT)) {
			usb10_printf("%s: CRC16 error (%d), STATUS = %08X, RX_STATUS = %08X, RX_STATUS2 = %08X, DATA = %08X:%08X\r\n",
				USB10_DEVICE_IN_REQUEST_STR, i, reg->STATUS, rx_status, reg->RX_STATUS2,
				reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
			);

/*
			reg->COMMAND = USB10_CMD_START_BIT |
				USB10_CMD_SET_PID(USB10_PID_NAK) |
				USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
			if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
				usb10_printf("%s: hung after %s packet!\r\n", USB10_DEVICE_IN_REQUEST_STR, "NAK");
				ret = -7;
				goto fail;
			}
*/

			delay_us(20);

			if(retry--)
				goto again_data;

			ret = -10;
			goto fail;
		}
		#endif

		#if(0) // Enable this if packet size matching is necessary
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
		#endif

		// Send ACK for received packet

		reg->COMMAND = USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_ACK) |
			USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
		if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
			usb10_printf("%s: hung after %s packet!\r\n", USB10_DEVICE_IN_REQUEST_STR, "ACK");
			ret = -7;
			goto fail;
		}

		// Now analyze what we've got here

		if(received_pid != USB10_PID_DATA0 && received_pid != USB10_PID_DATA1) {
			usb10_printf("%s: expected %s packet instead of 0x%02X, RX_STATUS = %08X\r\n", USB10_DEVICE_IN_REQUEST_STR,
				"DATA", received_pid, rx_status);

			if(retry--)
				goto again_data;

			ret = -9;
			goto fail;
		}

		if(received_pid == usb10_last_data_pid[address][endpoint].recv) {

			usb10_printf("%s: received %d dupe data, RX_STATUS = %08X, DATA = %08X:%08X\r\n",
				USB10_DEVICE_IN_REQUEST_STR, i, rx_status,
				reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
			);

			if(retry--)
				goto again_data;

			ret = -10;
			goto fail;
		}

		usb10_printf("%s: packet received (%d), RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
			USB10_DEVICE_IN_REQUEST_STR, i, rx_status, reg->RECV_DATA_HIGH, reg->RECV_DATA_LOW
		);

		usb10_last_data_pid[address][endpoint].recv = received_pid; // Remember last received DATA PID

		if(response_size && response_data) { // collect data if response expected
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


#define	USB10_DEVICE_OUT_REQUEST_STR	"usb10_device_out_request"

int usb10_device_out_request(USB10_Reg* reg, uint8_t address, uint8_t endpoint,
	uint8_t* request_data, uint32_t request_size)
{
	/* NOTE: request_data buffer should be at least 8 bytes long, even if request_size is zero !!! */

	if(address >= MAX_ADDRESSES || endpoint >= MAX_ENDPOINTS)
		return -999;

	// Wait till bus is free
	if(!usb10_wait_while_busy(reg, 10000)) { // ~1ms timeout
		usb10_printf("%s: bus is busy for too long!\r\n", USB10_DEVICE_OUT_REQUEST_STR);
		return -1;
	}

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during I/O 

	int ret = 0;

	int num_of_data_packets = request_size % USB10_LOW_SPEED_DATA_SIZE ?
		request_size / USB10_LOW_SPEED_DATA_SIZE + 1 : request_size / USB10_LOW_SPEED_DATA_SIZE;

	if(request_size == 0)
		num_of_data_packets = 1; // send at least one data packet

	for(int i = 0; i < num_of_data_packets; i++) {

		// Send part of request data

		uint32_t rx_status;
		int timeout;
		int retry = 8;

		int packet_size_bits = (i + 1) * USB10_LOW_SPEED_DATA_SIZE  <= request_size ?
				(USB10_LOW_SPEED_DATA_SIZE * 8 - 1) : // DATA*8 - 1 
				(request_size % USB10_LOW_SPEED_DATA_SIZE) * 8 - 1; 

		// Calculate new DATA PID depending on last used/received
		int data_pid = (usb10_last_data_pid[address][endpoint].sent == USB10_PID_DATA0) ? 
				USB10_PID_DATA1 : USB10_PID_DATA0;
		again_out:

		// Prepare data to send, if such were provided
		if(request_size && request_data) {
			reg->SEND_DATA_LOW = *(uint32_t*)(request_data + 0);
			reg->SEND_DATA_HIGH = *(uint32_t*)(request_data + 4);
		}

		// Send OUT token first
		reg->COMMAND = USB10_CMD_START_BIT |
				USB10_CMD_SET_PID(USB10_PID_OUT) |
				USB10_CMD_SET_ADDR(address) |
				USB10_CMD_SET_ENDP(endpoint) |
				USB10_CMD_SET(USB10_CMD_SEND_TOKEN);
				
		if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
			usb10_printf("%s: hung after %s packet!\r\n", USB10_DEVICE_OUT_REQUEST_STR, "OUT");
			ret = -2;
			goto fail;
		}

		// Send DATA packet
		reg->COMMAND = USB10_CMD_START_BIT |
				USB10_CMD_SET_LEN(packet_size_bits) |
				USB10_CMD_SET_PID(data_pid) |
				USB10_CMD_SET(USB10_CMD_SEND_DATA);

		if(!usb10_wait_cmd_complete(reg, 5000)) { // ~0.5ms timeout
			usb10_printf("%s: hung after %s packet!\r\n", USB10_DEVICE_OUT_REQUEST_STR, "DATA");
			ret = -3;
			goto fail;
		}

		timeout = 2500;
		while(timeout--)
			if(reg->STATUS & USB10_STATUS_RECEIVED_BIT)
				goto recv1;

		{
			usb10_printf("%s: no response after %s(%d:%d)\r\n", USB10_DEVICE_OUT_REQUEST_STR, "DATA",
				address, endpoint);

			if(retry--)
				goto again_out;

			ret = -6;
			goto fail;
		}
		
		recv1:

		rx_status = reg->RX_STATUS; // read once, use many times

		// Now analyze what we've got here

		int received_pid = USB10_RX_STATUS_PID(rx_status);

		if(received_pid == USB10_PID_NAK) {
			ret = -8;
			goto fail;
		}

		if(received_pid != USB10_PID_ACK) {
			usb10_printf("%s: expected %s packet instead of 0x%02X, RX_STATUS = %08X\r\n", USB10_DEVICE_OUT_REQUEST_STR, "ACK",
				USB10_RX_STATUS_PID(rx_status), rx_status);

			if(retry--)
				goto again_out;

			ret = -9;
			goto fail;
		}

		usb10_last_data_pid[address][endpoint].sent = data_pid;

		usb10_printf("%s: packet sent (%d), RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
			USB10_DEVICE_OUT_REQUEST_STR, i, rx_status, reg->SEND_DATA_HIGH, reg->SEND_DATA_LOW
		);

	}

	fail:
	
	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	return ret;
}


#define	USB10_DEVICE_GET_DESCR_STR	"usb10_device_get_description"

int usb10_device_get_description(USB10_Reg* reg, uint8_t address, USB10_DescriptionUnion* descr_resp)
{
	int ret = 0;

	usb10_printf("%s: %s\r\n", USB10_DEVICE_GET_DESCR_STR, "begin");

	// Begin of transaction
	if((ret = usb10_device_setup_request(USB1, address, usb10_descr_req, (uint8_t*)descr_resp, 18)) < 0) {
		usb10_printf("%s: usb10_device_setup_request ret = %d\r\n", USB10_DEVICE_GET_DESCR_STR, ret);
		return ret;
	}

	delay_us(200); // let device process request

	// End of transaction 
	if((ret = usb10_device_out_request(USB1, address, USB10_EP0, NULL, 0)) < 0) {
		usb10_printf("%s: usb10_device_out_request ret = %d\r\n", USB10_DEVICE_GET_DESCR_STR, ret);
		return ret;
	}

	usb10_printf("%s: %s\r\n", USB10_DEVICE_GET_DESCR_STR, "ok");

	return ret;
}


#define	USB10_DEVICE_GET_CONFIG_STR	"usb10_device_get_config"

int usb10_device_get_config(USB10_Reg* reg, uint8_t address, USB10_ConfigurationUnion* config_resp)
{
	int ret = 0;

	usb10_printf("%s: %s\r\n", USB10_DEVICE_GET_CONFIG_STR, "begin");

	// Begin of transaction
	if((ret = usb10_device_setup_request(USB1, address, usb10_config_req, (uint8_t*)config_resp, 25)) < 0) {
		usb10_printf("%s: usb10_device_setup_request ret = %d\r\n", USB10_DEVICE_GET_CONFIG_STR, ret);
		return ret;
	}

	delay_us(200); // let device process request

	// End of transaction 
	if((ret = usb10_device_out_request(USB1, address, USB10_EP0, NULL, 0)) < 0) {
		usb10_printf("%s: usb10_device_out_request ret = %d\r\n", USB10_DEVICE_GET_CONFIG_STR, ret);
		return ret;
	}

	usb10_printf("%s: %s\r\n", USB10_DEVICE_GET_CONFIG_STR, "ok");

	return ret;
}


#define	USB10_DEVICE_SET_ADDR_STR	"usb10_device_set_address"

int usb10_device_set_address(USB10_Reg* reg, uint8_t address_old, uint8_t address_new)
{
	int ret = 0;

	usb10_setaddr_req[2] = address_new; // Device address

	usb10_printf("%s: %s\r\n", USB10_DEVICE_SET_ADDR_STR, "begin");

	// Begin of transaction
	if((ret = usb10_device_setup_request(USB1, address_old, usb10_setaddr_req, NULL, 0)) < 0) {
		usb10_printf("%s: usb10_device_setup_request ret = %d\r\n", USB10_DEVICE_SET_ADDR_STR, ret);
		return ret;
	}

	delay_us(200); // let device process request

	// End of transaction
	if((ret = usb10_device_in_request(USB1, address_old, USB10_EP0, NULL, 0)) < 0) {
		usb10_printf("%s: usb10_device_in_request ret = %d\r\n", USB10_DEVICE_SET_ADDR_STR, ret);
		return ret;
	}


	usb10_printf("%s: %s\r\n", USB10_DEVICE_SET_ADDR_STR, "ok");

	return ret;
}


#define	USB10_DEVICE_SET_CONF_STR	"usb10_device_set_config"

int usb10_device_set_config(USB10_Reg* reg, uint8_t address, uint8_t config_num)
{
	int ret = 0;

	usb10_setconf_req[2] = config_num;

	usb10_printf("%s: %s\r\n", USB10_DEVICE_SET_CONF_STR, "begin");

	// Begin of transaction
	if((ret = usb10_device_setup_request(USB1, address, usb10_setconf_req, NULL, 0)) < 0) { 
		usb10_printf("%s: usb10_device_setup_request ret = %d\r\n", USB10_DEVICE_SET_CONF_STR, ret);
		return ret;
	}

	delay_us(200); // let device process request

	// End of transaction
	if((ret = usb10_device_in_request(USB1, address, USB10_EP0, NULL, 0)) < 0) {
		usb10_printf("%s: usb10_device_in_request ret = %d\r\n", USB10_DEVICE_SET_CONF_STR, ret);
		return ret;
	}

	usb10_printf("%s: %s\r\n", USB10_DEVICE_SET_CONF_STR, "ok");

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

	if((ret = usb10_device_get_description(USB1, 0, &usb10_descr_resp)) < 0)
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

	if((ret = usb10_device_get_config(USB1, device_address, &usb10_config_resp)) < 0)
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
		

	ok:
		// Fill-in returning data structures 

	if(new_device_address)
		*new_device_address = usb10_device_address;

	if(descr_resp)
		*descr_resp = &usb10_descr_resp;

	if(config_resp)
		*config_resp = &usb10_config_resp;

	usb10_printf("%s: %s\r\n", USB10_SCAN_STR, "ok");

	return 0;

	usb10_error: 

	usb10_device_address = 0; // this can be used as flag to indicate b10_device_addressevice is ready

	usb10_printf("%s: %s\r\n", USB10_SCAN_STR, "fail");

	return ret;
}


#define	USB10_HID_SET_LED_STR	"usb10_hid_set_led"

int usb10_hid_set_led(USB10_Reg* reg, uint8_t address, uint8_t endpoint, uint8_t leds)
{
	uint8_t led_status[8];
	int ret = 0;

	usb10_setreport_req[4] = endpoint;
	led_status[0] = leds;

	usb10_printf("%s: %s\r\n", USB10_HID_SET_LED_STR, "begin");

	// Begin transaction
	if((ret = usb10_device_setup_request(USB1, address, usb10_setreport_req, NULL, 0)) < 0) {
		usb10_printf("%s: usb10_device_setup_request ret = %d\r\n", USB10_HID_SET_LED_STR, ret);
		return ret;
	}

	delay_us(200);

	// Send LED status data
	if((ret = usb10_device_out_request(USB1, address, USB10_EP0, led_status, 1)) < 0) {
		usb10_printf("%s: usb10_device_out_request ret = %d\r\n", USB10_HID_SET_LED_STR, ret);
		return ret;
	}

	// End transaction
	if((ret = usb10_device_in_request(USB1, address, USB10_EP0, NULL, 0)) < 0) {
		usb10_printf("%s: usb10_device_in_request ret = %d\r\n", USB10_HID_SET_LED_STR, ret);
		return ret;
	}

	usb10_printf("%s: %s\r\n", USB10_HID_SET_LED_STR, "ok");

	return ret;
}


