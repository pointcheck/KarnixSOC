#include "utils.h"
#include "soc.h"
#include "modbus.h"
#include "config.h"

volatile uint8_t modbus_rtu_rx_buf[MODBUS_RX_BUF_SIZE];
volatile uint32_t modbus_rtu_rx_len = 0;
volatile unsigned long long modbus_rtu_rx_timestamp = 0;

void modbus_rtu_init(void) {

	Uart_Config uart_config;

	uart_config.dataLength = UART_DATA_8;
	uart_config.parity = UART_PARITY_NONE;
	uart_config.stop = UART_STOP_ONE;
	
	uint32_t oldClockDivider = UART1->CLOCK_DIVIDER;

	uint32_t rxSamplePerBit = UART_PRE_SAMPLING_SIZE + UART_SAMPLING_SIZE + UART_POST_SAMPLING_SIZE;

	uart_config.clockDivider = SYSTEM_CLOCK_HZ / active_config.modbus_baud / rxSamplePerBit - 1;

	uart_applyConfig(UART1, &uart_config);

	printf("modbus_rtu_init() done, dataLength = %d, parity = %d, stop = %d, clockDivider = %d (was: %d)\r\n", 
		uart_config.dataLength,
		uart_config.parity,
		uart_config.stop,
		uart_config.clockDivider,
		oldClockDivider
	);

	modbus_rtu_rx_timestamp = get_mtime();
	modbus_rtu_rx_len = 0;
}

void modbus_rtu_poll(void) {
	
	if(modbus_rtu_rx_len > 0 && (get_mtime() - modbus_rtu_rx_timestamp >= MODBUS_RX_DELAY_US)) {

		uint8_t rx_buf[MODBUS_RX_BUF_SIZE];
		uint8_t tx_buf[MODBUS_TX_BUF_SIZE];

		#ifdef MODBUS_DEBUG
		printf("modbus_rtu_poll() recv %d bytes in request\r\n", modbus_rtu_rx_len);
		#endif

		
		// Critical section: this procedure getting called from timer ISR, all interrupts are blocked

		//csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts

		uint32_t rx_len = modbus_rtu_rx_len;
		memcpy(rx_buf, (void*)modbus_rtu_rx_buf, MIN(rx_len, MODBUS_RX_BUF_SIZE));
		modbus_rtu_rx_len = 0;

		//csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

		uint32_t tx_len = modbus_recv(rx_buf, rx_len, tx_buf);

		if(tx_len > 0) {
			for(int i = 0; i < tx_len; i++) {
				uart_write(UART1, tx_buf[i]);
			}

			#ifdef MODBUS_DEBUG
			printf("modbus_rtu_poll() sent %d bytes in response\r\n", tx_len);
			#endif
		}

	}

}

void modbus_rtu_rx(void) {
		
	modbus_rtu_rx_buf[modbus_rtu_rx_len++] = UART1->DATA;

	if(modbus_rtu_rx_len >= MODBUS_RX_BUF_SIZE)
		modbus_rtu_rx_len = 0;

	if(uart_readOccupancy(UART1) > 0) {

		printf("modbus_rtu_rx() stale RX FIFO %d bytes, rx_len = %d\r\n", uart_readOccupancy(UART1), modbus_rtu_rx_len);

		while(uart_readOccupancy(UART1)) {

			modbus_rtu_rx_buf[modbus_rtu_rx_len++] = UART1->DATA;

			if(modbus_rtu_rx_len >= MODBUS_RX_BUF_SIZE)
				modbus_rtu_rx_len = 0;
		}
	}

	modbus_rtu_rx_timestamp = get_mtime();
}

