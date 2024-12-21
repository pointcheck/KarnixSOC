#include <stdint.h>
#include <stdio.h>
#include "riscv.h"
#include "soc.h"
#include "utils.h"

#define	CLI_BUF_SIZE       	128
#define CONSOLE_RX_BUF_SIZE     128
#define CONSOLE_TX_BUF_SIZE     128
#define CONSOLE_RX_DELAY_US     10000

uint32_t console_rx_buf_len = 0;
uint8_t console_rx_buf[CONSOLE_RX_BUF_SIZE];

uint8_t cli_buf[CLI_BUF_SIZE+1] = {0};
uint32_t cli_buf_len = 0;

void cli_process_command(uint8_t *cmd, uint32_t len);


void cli_prompt(void) {
	printf("\rMONITOR[%03u]-> %s", cli_buf_len, cli_buf);
	fflush(stdout);
}


// Process CLI command once Enter is pressed
void cli_process_command(uint8_t *cmd, uint32_t len) {
	printf("\r\nYou entered: %s\r\n\r\n", cmd);
}


// Parse key stroke buffer, perform basic CLI editing features
void cli_process_input(uint8_t *buf, uint32_t len) {

	if(len == 0 || buf == NULL)
	       return;

	for(int i = 0; i < len; i++) {
		uint8_t c = buf[i];

		switch(c) {
			case 0x07: {
				printf("%c", c);
				break;
			}

			case 0x08: {
				if(cli_buf_len == 0)
					break;

				printf("%c%c%c", c, 0x20, c);
				cli_buf[--cli_buf_len] = 0;
				break;
			}

			case 0x0a:
			case 0x0d: {
				cli_buf[cli_buf_len] = 0;
				cli_process_command(cli_buf, cli_buf_len);
				cli_buf_len = 0;
				break;
			}

			default: {
				if(cli_buf_len >= CLI_BUF_SIZE) // CLI buffer overflow, skip input
					break;

				if(c >= 0x20 && c < 0x7f) // visual character ?
					cli_buf[cli_buf_len++] = c;


			}
		}
	}

	cli_buf[cli_buf_len] = 0;

	cli_prompt();
}


// Check (poll) input serial buffer
void console_poll(void) {

	if(console_rx_buf_len == 0)
		return;

	uint8_t rx_buf[CONSOLE_RX_BUF_SIZE];

	// Critical section: accessed data used by ISR 

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts

	uint32_t rx_len = console_rx_buf_len;
	memcpy(rx_buf, (void*)console_rx_buf, MIN(rx_len, CONSOLE_RX_BUF_SIZE));
	console_rx_buf_len = 0;

	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	cli_process_input(rx_buf, rx_len);

}


// Called from ISR to collect data from RX FIFO to internal buffer
void console_rx(void) {

	int overflow = 0;

	while(uart_readOccupancy(UART0)) {

		char c = UART0->DATA;

		if(console_rx_buf_len >= CONSOLE_RX_BUF_SIZE) {
			overflow++;
			continue;
		}

		console_rx_buf[console_rx_buf_len++] = c;
	}

	if(overflow)
		printf("console_rx() buffer overflow, console_rx_buf_len = %d, lost %d bytes\r\n",
			console_rx_buf_len, overflow);

}

