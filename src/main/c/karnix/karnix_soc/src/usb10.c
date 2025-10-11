#include "utils.h"
#include "soc.h"
#include "riscv.h"

//#define	USB10_DEBUG_MORE	1

#if(USB10_DEBUG)
	#define	usb10_printf(...)	{printf( __VA_ARGS__);}
#else
	#define	usb10_printf(...)	{ }
#endif

USB10_DeviceDescriptorUnion usb10_device_descr = {0};
USB10_ConfigurationDescriptorUnion usb10_config_descr = {0};
USB10_InterfaceDescriptorUnion usb10_interface_descr = {0};
USB10_EndpointDescriptorUnion usb10_endpoint_descr = {0};

uint8_t usb10_device_address = 0;

struct _usb10_last_data_pid {
	uint8_t sent;
	uint8_t recv;
} usb10_last_data_pid[MAX_ADDRESSES][MAX_ENDPOINTS];

static inline void usb10_write_reg(volatile uint32_t* reg, uint32_t val) {
	asm volatile ("sw %0, (%1)" :  : "r"(val), "r"(reg));
}

static inline uint32_t usb10_read_reg(volatile uint32_t* reg) {
	return *reg;
}

static inline int usb10_wait_cmd_complete(USB10_Reg* reg, int timeout) {

	int i;

	// Make sure command execution has begun
	for(i = 0; i < 50; i++)
	       if(usb10_read_reg(&reg->STATUS) & USB10_STATUS_BUSY_BIT)
		       break;

	// Wait for execution to complete
	while(timeout--)
	       if((usb10_read_reg(&reg->STATUS) & USB10_STATUS_BUSY_BIT) == 0)
			return timeout; // OK

	return 0; // Fail
}


static inline int usb10_wait_while_busy(USB10_Reg* reg, int timeout) {

	int i;

	// Wait for execution to complete
	while(timeout--)
	       if((usb10_read_reg(&reg->STATUS) & USB10_STATUS_BUSY_BIT) == 0)
			return timeout;

	return 0;
}


#define	USB10_DEVICE_RESET_STR	"usb10_device_reset"

int usb10_bus_reset(USB10_Reg* reg, int wait_us)
{

	if(usb10_read_reg(&reg->STATUS) & USB10_STATUS_ERROR_BIT) {
		usb10_printf("%s: device not connected!\r\n", USB10_DEVICE_RESET_STR);
		return -1;
	}

	// RESET

	usb10_write_reg(&reg->COMMAND, USB10_CMD_START_BIT |
			USB10_CMD_SET(USB10_CMD_BUS_RESET));
			
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


#define	USB10_SETUP_REQUEST_STR	"usb10_setup_request"

int usb10_setup_request(USB10_Reg* reg, uint8_t address, uint8_t endpoint,
	uint8_t *request_data, uint8_t* response_data, uint32_t response_size)
{

	if(address >= MAX_ADDRESSES)
		return -999;

	// Wait till bus is free
	if(!usb10_wait_while_busy(reg, 10000)) { // ~1ms timeout
		usb10_printf("%s: bus is busy for too long!\r\n", USB10_SETUP_REQUEST_STR);
		return -1;
	}

	usb10_printf("%s: request %02X %02X to addr = %d\r\n", USB10_SETUP_REQUEST_STR,
		request_data[0], request_data[1], address);

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during I/O 

	uint32_t rx_status;
	int timeout;
	int retry = 3;
	int ret = 0;
	int bytes_received = 0;

	usb10_last_data_pid[address][endpoint].sent = USB10_PID_DATA1;
	usb10_last_data_pid[address][endpoint].recv = USB10_PID_DATA0;

	again_setup:

	// Prepare DATA to be sent 
	usb10_write_reg(&reg->SEND_DATA_LOW, *(uint32_t*)(request_data + 0));
	usb10_write_reg(&reg->SEND_DATA_HIGH, *(uint32_t*)(request_data + 4));

	int data_pid = (usb10_last_data_pid[address][0].sent == USB10_PID_DATA0) ? 
		USB10_PID_DATA1 : USB10_PID_DATA0;

	// SETUP I/O with 0:0
	usb10_write_reg(&reg->COMMAND, USB10_CMD_START_BIT |
			USB10_CMD_SET_PID(USB10_PID_SETUP) |
			USB10_CMD_SET_ADDR(address) |
			USB10_CMD_SET_ENDP(endpoint) |
			USB10_CMD_SET(USB10_CMD_SEND_LONG_TOKEN));
 

	if(!usb10_wait_cmd_complete(reg, 20000)) { // ~2ms timeout
		usb10_printf("%s: hung after %s packet!\r\n", USB10_SETUP_REQUEST_STR, "SETUP");
		ret = -2;
		goto fail;
	}

	usb10_write_reg(&reg->COMMAND, USB10_CMD_START_BIT |
			USB10_CMD_SET_LEN(8*8-1) |
			USB10_CMD_SET_PID(data_pid) |
			USB10_CMD_SET(USB10_CMD_SEND_DATA));

	if(!usb10_wait_cmd_complete(reg, 5000)) { // ~0.5ms timeout
		usb10_printf("%s: hung after %s packet!\r\n", USB10_SETUP_REQUEST_STR, "DATA");
		ret = -3;
		goto fail;
	}

	timeout = 2500;
	while(timeout--)
		if(usb10_read_reg(&reg->STATUS) & USB10_STATUS_RECEIVED_BIT)
			goto rcvd1;

	{
		usb10_printf("%s: no response after %s(%d:%d)\r\n", USB10_SETUP_REQUEST_STR,
			"DATA", address, 0);

		if(--retry)
			goto again_setup;

		ret = -4;
		goto fail;
	}

	rcvd1:


	usb10_last_data_pid[address][0].sent = data_pid; // remember last used DATA PID

	rx_status = usb10_read_reg(&reg->RX_STATUS); // read once, use many times

	if(USB10_RX_STATUS_PID(rx_status) != USB10_PID_ACK) {
		usb10_printf("%s: expected %s packet instead of 0x%02X, RX_STATUS = %08X\r\n", USB10_SETUP_REQUEST_STR, "ACK",
			USB10_RX_STATUS_PID(rx_status), rx_status);

		if(--retry)
			goto again_setup;

		ret = -5;
		goto fail;
	}

	//delay_us(10); // Let device process request

	int num_of_data_packets = response_size % USB10_LOW_SPEED_DATA_SIZE ?
		response_size / USB10_LOW_SPEED_DATA_SIZE + 1 : response_size / USB10_LOW_SPEED_DATA_SIZE;

	for(int i = 0; i < num_of_data_packets; i++) {

		// Request part of response data

		retry = 8;

		int packet_size_bits = (i + 1) * USB10_LOW_SPEED_DATA_SIZE <= response_size ?
				(USB10_LOW_SPEED_DATA_SIZE * 8 + 8 + 16) : // PID8 + DATA*8 + CRC16 
				(response_size % USB10_LOW_SPEED_DATA_SIZE) * 8 + 8 + 16; 

		again_data:

		usb10_write_reg(&reg->COMMAND, USB10_CMD_START_BIT |
				USB10_CMD_SET_PID(USB10_PID_IN) |
				USB10_CMD_SET_ADDR(address) |
				USB10_CMD_SET_ENDP(endpoint) |
				USB10_CMD_SET(USB10_CMD_SEND_LONG_TOKEN));

		timeout = 2500;
		while(timeout--)
			if(usb10_read_reg(&reg->STATUS) & USB10_STATUS_RECEIVED_BIT)
				goto rcvd2;

		{
			usb10_printf("%s: no response after %s(%d:%d)\r\n", USB10_SETUP_REQUEST_STR,
				"IN", address, 0);

			if(retry--)
				goto again_data;

			ret = -6;
			goto fail;
		}
		
		rcvd2:

		rx_status = usb10_read_reg(&reg->RX_STATUS); // read once, use many times

		int received_pid = USB10_RX_STATUS_PID(rx_status);

		if(received_pid == USB10_PID_STALL) { // STALL indicates fatal error
			ret = -12;
			goto fail;
		}

		// Note: NAK packets missing CRC16 which also considered as fail 

		#if(1)
		if(!(usb10_read_reg(&reg->STATUS) & USB10_STATUS_CRC16_OK_BIT)) {
			usb10_printf("%s: CRC16 error (%d), STATUS = %08X, RX_STATUS = %08X, RX_STATUS2 = %08X, DATA = %08X:%08X\r\n",
				USB10_SETUP_REQUEST_STR, i, usb10_read_reg(&reg->STATUS), rx_status,
				usb10_read_reg(&reg->RX_STATUS2), usb10_read_reg(&reg->RECV_DATA_HIGH),
				usb10_read_reg(&reg->RECV_DATA_LOW)
			);


			delay_us(20);

			if(retry--)
				goto again_data;

			ret = -10;
			goto fail;
		}
		#endif

		// Send ACK for received packet

		usb10_write_reg(&reg->COMMAND, USB10_CMD_START_BIT |
				USB10_CMD_SET_PID(USB10_PID_ACK) |
				USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN));

		if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
			usb10_printf("%s: hung after %s packet!\r\n", USB10_SETUP_REQUEST_STR, "ACK");
			ret = -7;
			goto fail;
		}

		// Now analyze what we've got here

		if(received_pid != USB10_PID_DATA0 && received_pid != USB10_PID_DATA1) {
			usb10_printf("%s: expected %s packet instead of 0x%02X, RX_STATUS = %08X\r\n", USB10_SETUP_REQUEST_STR, "DATA",
				USB10_RX_STATUS_PID(rx_status), rx_status);

			if(retry--)
				goto again_data;

			ret = -8;
			goto fail;
		}

		if(received_pid == usb10_last_data_pid[address][endpoint].recv) {

			usb10_printf("%s: received %d dupe data, RX_STATUS = %08X, DATA = %08X:%08X\r\n",
				USB10_SETUP_REQUEST_STR, i, rx_status,
				usb10_read_reg(&reg->RECV_DATA_HIGH), usb10_read_reg(&reg->RECV_DATA_LOW)
			);

			if(retry--)
				goto again_data;

			ret = -9;
			goto fail;
		}

		usb10_printf("%s: packet received (%d), RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
			USB10_SETUP_REQUEST_STR, i, rx_status,
			usb10_read_reg(&reg->RECV_DATA_HIGH), usb10_read_reg(&reg->RECV_DATA_LOW)
		);

		usb10_last_data_pid[address][endpoint].recv = received_pid; // Remember last received DATA PID

		if(response_size && response_data) { // collect data if response expected
			*(uint32_t*)(response_data + i * 8 + 0) = usb10_read_reg(&reg->RECV_DATA_LOW);
			*(uint32_t*)(response_data + i * 8 + 4) = usb10_read_reg(&reg->RECV_DATA_HIGH);
		}

		bytes_received += (USB10_RX_STATUS_LEN(rx_status) - 8 - 16) / 8;

		#if(1) // Enable this if packet size matching is necessary
		if(USB10_RX_STATUS_LEN(rx_status) != packet_size_bits) {
			usb10_printf("%s: received bogus data packet (%d) size: %d bits != %d, DATA = %08X:%08X\r\n",
				USB10_SETUP_REQUEST_STR, i, USB10_RX_STATUS_LEN(rx_status), packet_size_bits,
				usb10_read_reg(&reg->RECV_DATA_HIGH), usb10_read_reg(&reg->RECV_DATA_LOW)
			);

			break;
		}
		#endif

	}


	usb10_printf("%s: RX %d bytes of DATA: ", bytes_received, USB10_SETUP_REQUEST_STR);

	for(int i = 0; i < bytes_received; i++)
		usb10_printf("%02X ", response_data[i]);

	usb10_printf("\r\n", 0);
	
	ret = bytes_received;

	fail:
	
	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	return ret;
}


#define	USB10_IN_REQUEST_STR	"usb10_in_request"

int usb10_in_request(USB10_Reg* reg, uint8_t address, uint8_t endpoint,
	uint8_t* response_data, uint32_t response_size)
{
	if(address >= MAX_ADDRESSES || endpoint >= MAX_ENDPOINTS)
		return -999;

	// Wait till bus is free
	if(!usb10_wait_while_busy(reg, 10000)) { // ~1ms timeout
		usb10_printf("%s: bus is busy for too long!\r\n", USB10_IN_REQUEST_STR);
		return -1;
	}

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during I/O 

	int ret = 0;
	int bytes_received = 0;

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

		usb10_write_reg(&reg->COMMAND, USB10_CMD_START_BIT |
				USB10_CMD_SET_PID(USB10_PID_IN) |
				USB10_CMD_SET_ADDR(address) |
				USB10_CMD_SET_ENDP(endpoint) |
				USB10_CMD_SET(USB10_CMD_SEND_LONG_TOKEN));

		timeout = 25000;
		while(timeout--)
			if(usb10_read_reg(&reg->STATUS) & USB10_STATUS_RECEIVED_BIT)
				goto rcvd2;

		{
			usb10_printf("%s: no response after %s(%d:%d)\r\n", USB10_IN_REQUEST_STR, "IN",
				address, endpoint);

			if(retry--)
				goto again_data;

			ret = -6;
			goto fail;
		}
		
		rcvd2:

		rx_status = usb10_read_reg(&reg->RX_STATUS); // read once, use many times

		int received_pid = USB10_RX_STATUS_PID(rx_status);

		if(received_pid == USB10_PID_NAK) {
			ret = -8;
			goto fail;
		}

		if(received_pid == USB10_PID_STALL) { // STALL indicated fatal error
			ret = -12;
			goto fail;
		}

		// Note: Some short token packets missing CRC16 which also considered as fail 

		#if(1)
		if(!(usb10_read_reg(&reg->STATUS) & USB10_STATUS_CRC16_OK_BIT)) {
			usb10_printf("%s: CRC16 error (%d), STATUS = %08X, RX_STATUS = %08X, RX_STATUS2 = %08X, DATA = %08X:%08X\r\n",
				USB10_IN_REQUEST_STR, i, usb10_read_reg(&reg->STATUS), rx_status,
				usb10_read_reg(&reg->RX_STATUS2),
				usb10_read_reg(&reg->RECV_DATA_HIGH), usb10_read_reg(&reg->RECV_DATA_LOW)
			);

/*
//			reg->COMMAND = USB10_CMD_START_BIT |
//				USB10_CMD_SET_PID(USB10_PID_NAK) |
//				USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN);
			
			usb10_write_reg(&reg->COMMAND, USB10_CMD_START_BIT |
				USB10_CMD_SET_PID(USB10_PID_NAK) |
				USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN));
			
			if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
				usb10_printf("%s: hung after %s packet!\r\n", USB10_IN_REQUEST_STR, "NAK");
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

		// Send ACK for received packet

		usb10_write_reg(&reg->COMMAND, USB10_CMD_START_BIT |
				USB10_CMD_SET_PID(USB10_PID_ACK) |
				USB10_CMD_SET(USB10_CMD_SEND_SHORT_TOKEN));

		if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
			usb10_printf("%s: hung after %s packet!\r\n", USB10_IN_REQUEST_STR, "ACK");
			ret = -7;
			goto fail;
		}

		// Now analyze what we've got here

		if(received_pid != USB10_PID_DATA0 && received_pid != USB10_PID_DATA1) {
			usb10_printf("%s: expected %s packet instead of 0x%02X, RX_STATUS = %08X\r\n", USB10_IN_REQUEST_STR,
				"DATA", received_pid, rx_status);

			if(retry--)
				goto again_data;

			ret = -9;
			goto fail;
		}

		if(received_pid == usb10_last_data_pid[address][endpoint].recv) {

			usb10_printf("%s: received %d dupe data, RX_STATUS = %08X, DATA = %08X:%08X\r\n",
				USB10_IN_REQUEST_STR, i, rx_status,
				usb10_read_reg(&reg->RECV_DATA_HIGH), usb10_read_reg(&reg->RECV_DATA_LOW)
			);

			if(retry--)
				goto again_data;

			ret = -10;
			goto fail;
		}

		usb10_printf("%s: packet received (%d), RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
			USB10_IN_REQUEST_STR, i, rx_status,
			usb10_read_reg(&reg->RECV_DATA_HIGH), usb10_read_reg(&reg->RECV_DATA_LOW)
		);

		usb10_last_data_pid[address][endpoint].recv = received_pid; // Remember last received DATA PID

		if(response_size && response_data) { // collect data if response expected
			*(uint32_t*)(response_data + i * 8 + 0) = usb10_read_reg(&reg->RECV_DATA_LOW);
			*(uint32_t*)(response_data + i * 8 + 4) = usb10_read_reg(&reg->RECV_DATA_HIGH);
		}

		bytes_received += (USB10_RX_STATUS_LEN(rx_status) - 8 - 16) / 8;

		#if(1) // Enable this if packet size matching is necessary
		if(USB10_RX_STATUS_LEN(rx_status) != packet_size_bits) {
			usb10_printf("%s: received bogus data packet (%d) size: %d bits != %d, DATA = %08X:%08X\r\n",
				USB10_IN_REQUEST_STR, i, USB10_RX_STATUS_LEN(rx_status), packet_size_bits,
				usb10_read_reg(&reg->RECV_DATA_HIGH), usb10_read_reg(&reg->RECV_DATA_LOW)
			);

			break;
		}
		#endif

	}


	usb10_printf("%s: RX DATA: ", USB10_IN_REQUEST_STR);
	usb10_printf("%s: RX %d bytes of DATA: ", bytes_received, USB10_IN_REQUEST_STR);

	for(int i = 0; i < bytes_received; i++)
		usb10_printf("%02X ", response_data[i]);

	usb10_printf("\r\n", 0);
	
	ret = bytes_received;

	fail:
	
	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	return ret;
}


#define	USB10_OUT_REQUEST_STR	"usb10_out_request"

int usb10_out_request(USB10_Reg* reg, uint8_t address, uint8_t endpoint,
	uint8_t* request_data, uint32_t request_size)
{
	/* NOTE: request_data buffer should be at least 8 bytes long, even if request_size is zero !!! */

	if(address >= MAX_ADDRESSES || endpoint >= MAX_ENDPOINTS)
		return -999;

	// Wait till bus is free
	if(!usb10_wait_while_busy(reg, 10000)) { // ~1ms timeout
		usb10_printf("%s: bus is busy for too long!\r\n", USB10_OUT_REQUEST_STR);
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
			usb10_write_reg(&reg->SEND_DATA_LOW, *(uint32_t*)(request_data + 0));
			usb10_write_reg(&reg->SEND_DATA_HIGH, *(uint32_t*)(request_data + 4));
		}

		// Send OUT token first
				
		usb10_write_reg(&reg->COMMAND, USB10_CMD_START_BIT |
				USB10_CMD_SET_PID(USB10_PID_OUT) |
				USB10_CMD_SET_ADDR(address) |
				USB10_CMD_SET_ENDP(endpoint) |
				USB10_CMD_SET(USB10_CMD_SEND_LONG_TOKEN));
				
		if(!usb10_wait_cmd_complete(reg, 20000)) { // 2ms timeout
			usb10_printf("%s: hung after %s packet!\r\n", USB10_OUT_REQUEST_STR, "OUT");
			ret = -2;
			goto fail;
		}

		// Send DATA packet
		usb10_write_reg(&reg->COMMAND, USB10_CMD_START_BIT |
				USB10_CMD_SET_LEN(packet_size_bits) |
				USB10_CMD_SET_PID(data_pid) |
				USB10_CMD_SET(USB10_CMD_SEND_DATA));

		if(!usb10_wait_cmd_complete(reg, 5000)) { // ~0.5ms timeout
			usb10_printf("%s: hung after %s packet!\r\n", USB10_OUT_REQUEST_STR, "DATA");
			ret = -3;
			goto fail;
		}

		timeout = 2500;
		while(timeout--)
			if(usb10_read_reg(&reg->STATUS) & USB10_STATUS_RECEIVED_BIT)
				goto recv1;

		{
			usb10_printf("%s: no response after %s(%d:%d)\r\n", USB10_OUT_REQUEST_STR, "DATA",
				address, endpoint);

			if(retry--)
				goto again_out;

			ret = -6;
			goto fail;
		}
		
		recv1:

		rx_status = usb10_read_reg(&reg->RX_STATUS); // read once, use many times

		// Now analyze what we've got here

		int received_pid = USB10_RX_STATUS_PID(rx_status);

		if(received_pid == USB10_PID_NAK) {
			ret = -8;
			goto fail;
		}

		if(received_pid != USB10_PID_ACK) {
			usb10_printf("%s: expected %s packet instead of 0x%02X, RX_STATUS = %08X\r\n",
				USB10_OUT_REQUEST_STR, "ACK",
				USB10_RX_STATUS_PID(rx_status), rx_status);

			if(received_pid == USB10_PID_STALL) {
				ret = -9;
				goto fail;
			}

			if(retry--)
				goto again_out;

			ret = -9;
			goto fail;
		}

		usb10_last_data_pid[address][endpoint].sent = data_pid;

		usb10_printf("%s: packet sent (%d), RX_STATUS = 0x%08X, DATA = %08X:%08X\r\n",
			USB10_OUT_REQUEST_STR, i, rx_status,
			usb10_read_reg(&reg->SEND_DATA_HIGH), usb10_read_reg(&reg->SEND_DATA_LOW)
		);

	}

	fail:
	
	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	return ret;
}


#define	USB10_GET_DEVICE_DESCR_STR	"usb10_get_device_descr"

int usb10_get_device_descriptor(USB10_Reg* reg, uint8_t address, uint8_t endpoint, uint8_t descr_type, uint8_t descr_item,
	uint16_t resp_len, void* descr_resp)
{
	int ret = 0;
	int bytes_read = 0;
	
	USB10_SetupRequest request;

	request.bmRequestType = 0x80; // device->host, standard, device
	request.bRequest = 0x06; // GET_DESCRIPTOR
	request.wValue = (descr_type << 8) | descr_item;
	request.bIndex = 0; // LANGID
	request.wLength = resp_len;

	usb10_printf("%s: %s\r\n", USB10_GET_DEVICE_DESCR_STR, "begin");

	// Begin of transaction
	if((ret = usb10_setup_request(USB1, address, endpoint, (uint8_t*)&request, (uint8_t*)descr_resp, resp_len)) < 0) {
		usb10_printf("%s(%d): setup ret = %d\r\n", USB10_GET_DEVICE_DESCR_STR, descr_type, ret);
		return ret;
	}

	bytes_read = ret;

	// End of transaction 
	if((ret = usb10_out_request(USB1, address, endpoint, NULL, 0)) < 0) {
		usb10_printf("%s: out ret = %d\r\n", USB10_GET_DEVICE_DESCR_STR, ret);
		return ret;
	}

	usb10_printf("%s: %s\r\n", USB10_GET_DEVICE_DESCR_STR, "ok");

	return ret < 0 ? ret : bytes_read;
}

#define	USB10_SET_VALUE_STR	"usb10_set_value"

int usb10_set_value(USB10_Reg* reg, uint8_t address, uint8_t endpoint, uint8_t bmRequestType, uint8_t bRequest,
	uint16_t wValue, uint8_t bIndex, uint16_t wLength)
{
	int ret = 0;

	USB10_SetupRequest request;

	request.bmRequestType = bmRequestType;
	request.bRequest = bRequest;
	request.wValue = wValue;
	request.bIndex = bIndex;
	request.wLength = wLength;

	usb10_printf("%s: %s\r\n", USB10_SET_VALUE_STR, "begin");

	// Begin of transaction
	if((ret = usb10_setup_request(USB1, address, endpoint, (uint8_t*)&request, NULL, 0)) < 0) { 
		usb10_printf("%s: setup ret = %d\r\n", USB10_SET_VALUE_STR, ret);
		return ret;
	}

	// End of transaction
	for(int i = 0; i < 5; i++) {
		ret = usb10_in_request(USB1, address, endpoint, NULL, 0);

		if(ret > 0)
			break;

 		if(ret == -8)
			continue; // NAK, try again

		usb10_printf("%s: in ret = %d\r\n", USB10_SET_VALUE_STR, ret);
		return ret;
	}

	usb10_printf("%s: %s\r\n", USB10_SET_VALUE_STR, "ok");

	return ret;
}


#define	USB10_SCAN_STR	"usb10_scan"

int usb10_scan(USB10_Reg* reg, uint8_t* new_device_address,
	USB10_DeviceDescriptorUnion **device_resp,
	USB10_ConfigurationDescriptorUnion **config_resp,
	USB10_InterfaceDescriptorUnion **interface_resp,
	USB10_EndpointDescriptorUnion **endpoint_resp)
{
	/* NOTE: descr_resp, config_resp, interface_resp and endpoint_resp can be NULL pointers,
	   so do not use these here, use global data structures instead! */

	int ret = 0;

	usb10_printf("\r%s: Scanning for devices...\r\n", USB10_SCAN_STR);

	if((ret = usb10_bus_reset(USB1, 12000)) < 0)
		goto usb10_error;

	if((ret = usb10_get_device_descriptor(USB1, USB10_UNNUMBERED_DEVICE, USB10_EP0, USB10_DESCR_TYPE_DEVICE,
					0, 18, &usb10_device_descr)) < 0)
		goto usb10_error;

	//usb10_printf("\r%s: Device detected: VID/PID = 0x%04X/0x%04X, "
	printf("\r%s: Device detected: VID/PID = 0x%04X/0x%04X, "
		"class/subclass = 0x%02X/0x%02X, bcdUSB = 0x%04X\r\n",
		USB10_SCAN_STR,
		usb10_device_descr.device.idVendor,
		usb10_device_descr.device.idProduct,
		usb10_device_descr.device.bDeviceClass,
		usb10_device_descr.device.bDeviceSubClass,
		usb10_device_descr.device.bcdUSB);

	if((ret = usb10_bus_reset(USB1, 12000)) < 0)
		goto usb10_error;

	int device_address = (usb10_device_address + 1) % MAX_ADDRESSES;

	// Assign new device address to device_address 
	if((ret = usb10_set_value(USB1, USB10_UNNUMBERED_DEVICE, USB10_EP0, 0x00, USB10_REQ_SET_ADDRESS,
				device_address, 0, 0)) < 0)
		goto usb10_error;

	// Request as many as possible description data

	uint8_t bulk_config_data[256];

	if((ret = usb10_get_device_descriptor(USB1, device_address, USB10_EP0, USB10_DESCR_TYPE_CONFIG,
					0, 255, bulk_config_data)) < 0)
		goto usb10_error;

	usb10_printf("%s: total config size = %d\r\n", USB10_SCAN_STR, ret);

	uint8_t *p = bulk_config_data;
	int bytes_parsed = 0;

	// Mark old descriptors as obsolete

	*(uint8_t*)&usb10_config_descr = 0;
	*(uint8_t*)&usb10_interface_descr = 0;
	*(uint8_t*)&usb10_endpoint_descr = 0;

	while(bytes_parsed < ret) {

		usb10_printf("%s: descriptor type = %d, size = %d\r\n", USB10_SCAN_STR, p[1], p[0]);

		// Save first ocurance as default descriptor of its type

		switch(p[1]) {
			case 0x02:	// Config
				if(*(uint8_t*)&usb10_config_descr == 0)
					memcpy(&usb10_config_descr, p, sizeof(usb10_config_descr));
				break;
			case 0x04:	//Interface
				if(*(uint8_t*)&usb10_interface_descr == 0)
					memcpy(&usb10_interface_descr, p, sizeof(usb10_interface_descr));
				break;
			case 0x05:	// Endpoint
				if(*(uint8_t*)&usb10_endpoint_descr == 0)
					memcpy(&usb10_endpoint_descr, p, sizeof(usb10_endpoint_descr));
				break;
		}

		bytes_parsed += p[0];
		p += p[0];
	}
	
	// Activate 1st configuration
	if((ret = usb10_set_value(USB1, device_address, USB10_EP0, 0x00, USB10_REQ_SET_CONFIG, 1, 0, 0)) < 0)
		goto usb10_error;

	usb10_device_address = device_address;	
	
	//usb10_printf("%s: Device addr = %d, Config: bLength = %d "
	printf("%s: Address = %d, Config wTotal = %d, "
		"Interface Class/Subclass/Proto = %d/%d/%d, "
		"EPAddress = 0x%02X, Interval = %d ms, MaxPacket = %d\r\n",
		USB10_SCAN_STR,
		usb10_device_address,
		usb10_config_descr.config.wTotalLength,
		usb10_interface_descr.iface.bInterfaceClass,
		usb10_interface_descr.iface.bInterfaceSubclass,
		usb10_interface_descr.iface.bInterfaceProtocol,
		usb10_endpoint_descr.endp.bEndpointAddress,
		usb10_endpoint_descr.endp.bInterval,
		usb10_endpoint_descr.endp.wMaxPacketSize);

	ok:
		// Fill-in returning data structures 

	if(new_device_address)
		*new_device_address = usb10_device_address;

	if(device_resp)
		*device_resp = &usb10_device_descr;

	if(config_resp)
		*config_resp = &usb10_config_descr;

	if(interface_resp)
		*interface_resp = &usb10_interface_descr;

	if(endpoint_resp)
		*endpoint_resp = &usb10_endpoint_descr;

	usb10_printf("%s: %s\r\n", USB10_SCAN_STR, "ok");

	return 0;

	usb10_error: 

	usb10_device_address = 0; // this can be used as flag to indicate device is ready

	usb10_printf("%s: %s\r\n", USB10_SCAN_STR, "fail");

	return ret;
}


#define	USB10_HID_SET_LED_STR	"usb10_hid_set_led"

int usb10_hid_set_led(USB10_Reg* reg, uint8_t address, uint8_t endpoint, uint8_t leds)
{
	uint8_t led_status[8];
	int ret = 0;

	USB10_SetupRequest request;

	request.bmRequestType = 0x21; // host->device, Class, Interface
	request.bRequest = 0x09; // SET_CONFIG
	request.wValue = 0x0200; // ???
	request.bIndex = endpoint;
	request.wLength = 1;	// One byte of data will contain LEDs status

	led_status[0] = leds;

	usb10_printf("%s: %s\r\n", USB10_HID_SET_LED_STR, "begin");

	// Begin transaction
	if((ret = usb10_setup_request(USB1, address, USB10_EP0, (uint8_t*)&request, NULL, 0)) < 0) {
		usb10_printf("%s: setup ret = %d\r\n", USB10_HID_SET_LED_STR, ret);
		return ret;
	}

	delay_us(200);

	// Send LED status data
	if((ret = usb10_out_request(USB1, address, USB10_EP0, led_status, 1)) < 0) {
		usb10_printf("%s: out ret = %d\r\n", USB10_HID_SET_LED_STR, ret);
		return ret;
	}

	// End transaction
	if((ret = usb10_in_request(USB1, address, USB10_EP0, NULL, 0)) < 0) {
		usb10_printf("%s: in ret = %d\r\n", USB10_HID_SET_LED_STR, ret);
		return ret;
	}

	usb10_printf("%s: %s\r\n", USB10_HID_SET_LED_STR, "ok");

	return ret;
}


