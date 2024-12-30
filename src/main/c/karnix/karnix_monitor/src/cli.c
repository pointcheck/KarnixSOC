#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "riscv.h"
#include "soc.h"
#include "utils.h"

#define	CLI_BUF_SIZE       	(128*2)
#define	CLI_HISTORY_SIZE	4
#define CONSOLE_RX_BUF_SIZE     (128*2)
#define CONSOLE_RX_DELAY_US     10000

#define	DEBUG_CLI	1	// 0 - off, 1 - few, 2 - more
#define	ARGN_MAX	8

volatile uint32_t console_rx_buf_len = 0;
volatile uint8_t console_rx_buf[CONSOLE_RX_BUF_SIZE];


uint8_t cli_history[CLI_HISTORY_SIZE][CLI_BUF_SIZE+1] = {0};
uint32_t cli_history_idx = 0;

uint8_t *cli_buf = &cli_history[0][0];
uint32_t cli_buf_len = 0;

uint32_t current_address = 0x80000000;
extern volatile uint32_t reg_sys_print_stats;

void cli_process_command(uint8_t *cmd, uint32_t len);


void cli_history_push(void) {
	cli_history_idx = (cli_history_idx + 1) % CLI_HISTORY_SIZE;
	cli_buf = &cli_history[cli_history_idx][0];
}

void cli_history_popup(void) {
	cli_history_idx = (cli_history_idx - 1) % CLI_HISTORY_SIZE;
	cli_buf = &cli_history[cli_history_idx][0];
	cli_buf_len = strlen(cli_buf);
}

void cli_history_popdown(void) {
	cli_history_idx = (cli_history_idx + 1) % CLI_HISTORY_SIZE;
	cli_buf = &cli_history[cli_history_idx][0];
	cli_buf_len = strlen(cli_buf);
}

void cli_prompt(void) {
	printf("\rMONITOR[%p]-> %s", current_address, cli_buf);
	fflush(stdout);
}

// Print usage info 
void cli_cmd_help(char *argv[], int argn) {
	printf(
"*** List of commands:\r\n"
"r[b|d] [*|addr] [len]	- Read and print 'len' bytes or dwords of memory beginning at 'addr'.\r\n"
"w[b|d] [*|addr] [data] [many]	- Write 'data' byte or dword to memory at 'addr' as 'many' times.\r\n"
"call   [*|addr] [args]	- Call subroutine at 'addr', 'args' will be provided as argv/argn.\r\n"
"dump   [*|addr] [len]	- Read 'len' bytes from memory at 'addr' and print in ASCII.\r\n"
"ihex   [*|addr]	- Input IHEX, decode and store at 'addr' or current location.\r\n"
"stats  [period]	- Enable printing statistics each 'period' sec, use 0 to disable.\r\n"
"\r\n"
	);
}


void cli_cmd_read_byte(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	int len = 1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);

	if(argv[2])
		len = strtoul(argv[2], NULL, 0);

	#if(DEBUG_CLI)
		printf("*** rb: addr = %p, len = %u\r\n", addr, len);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	int count = 0;

	while(count < len) {
		printf("rb[%p]:	", addr);
		for(int i = 0; i < 16 && count++ < len; i++)
		       printf("0x%02x ", *addr++);	
		printf("\r\n");

		if(console_rx_buf_len)
			break;
	}
}

void cli_cmd_read_dword(char *argv[], int argn) {

	uint32_t *addr = (uint32_t*) current_address;
	int len = 1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint32_t*) strtoul(argv[1], NULL, 0);

	if(argv[2])
		len = strtoul(argv[2], NULL, 0);

	#if(DEBUG_CLI)
		printf("*** rd: addr = %p, len = %u\r\n", addr, len);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	int count = 0;

	while(count < len) {
		printf("rd[%p]:	", addr);
		for(int i = 0; i < 4 && count++ < len; i++)
		       printf("0x%08x ", *addr++);	
		printf("\r\n");

		if(console_rx_buf_len)
			break;
	}
}

void cli_cmd_dump(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	int len = 1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);

	if(argv[2])
		len = strtoul(argv[2], NULL, 0);

	#if(DEBUG_CLI)
		printf("*** dump: addr = %p, len = %u\r\n", addr, len);
	#endif

	int count = 0;
	char str[24];

	current_address = (uint32_t) addr; // remember last address used

	while(count < len) {
		printf("dump[%p]:	", addr);

		str[0] = 0;

		for(int i = 0; i < 16 && count++ < len; i++) {
		       sprintf(str+i, "%c", ((*addr > 0x20) && (*addr < 0x7f)) ? *addr : '.');
		       printf("%02X ", *addr++);
		}

		printf(" | %s\r\n", str);

		if(console_rx_buf_len)
			break;
	}
}


void cli_cmd_write_byte(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	uint8_t value = 0;
	uint32_t many = 1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);

	if(argv[2])
		value = (uint8_t) strtoul(argv[2], NULL, 0);

	if(argv[3])
		many = (uint32_t) strtoul(argv[3], NULL, 0);

	#if(DEBUG_CLI)
		printf("*** wb: addr = %p, value = %02x, many = %d\r\n", addr, value, many);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	for(int i = 0; i < many; i++)
		*addr++ = value;
}


void cli_cmd_write_dword(char *argv[], int argn) {

	uint32_t *addr = (uint32_t*) current_address;
	uint32_t value = 0;
	uint32_t many = 1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint32_t*) strtoul(argv[1], NULL, 0);

	if(argv[2])
		value = (uint32_t) strtoul(argv[2], NULL, 0);

	if(argv[3])
		many = (uint32_t) strtoul(argv[3], NULL, 0);

	#if(DEBUG_CLI)
		printf("*** wd: addr = %p, value = %p, many = %d\r\n", addr, value, many);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	for(int i = 0; i < many; i++)
		*addr++ = value;

}


void cli_cmd_call(char *argv[], int argn) {

	uint32_t *addr = (uint32_t*) current_address;
	int len = 1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint32_t*) strtoul(argv[1], NULL, 0);

	#if(DEBUG_CLI)
		printf("*** call: addr = %p, argn = %d\r\n", addr, argn-1);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	uint32_t (*long_jump)(char *argv[], int arg) = (uint32_t (*)(char *argv[], int arg)) addr;

	uint32_t rc = long_jump(&(argv[1]), argn-1);

	printf("*** call: ret = %p\r\n", rc);
}

void cli_cmd_input_ihex(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	uint32_t base = 0, offset = 0, start16 = 0, start32 = 0;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);


	#if(DEBUG_CLI)
		printf("*** input_ihex: addr = %p, press Ctrl-C to break.\r\n", addr);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	//csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts

	int parse_data_flag = 0;

	uint8_t str[256*2+10+1]; // max len of IHEX string
	uint32_t idx = 0;
	uint32_t console_rx_buf_idx = 0;

	while(1) {

		if(console_rx_buf_len == 0)
			continue;

		csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts

		uint8_t c = console_rx_buf[console_rx_buf_idx++];

		if(console_rx_buf_idx == console_rx_buf_len) {
			console_rx_buf_len = 0;
			console_rx_buf_idx = 0;
		}

		csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

		if(c == ':') {
			parse_data_flag = 1;
			idx = 0;
			continue;
		}

		if(c == 0x03 || c == 0x04 || c == 0x08 || c == 0x7f) { // Ctrl-C, Ctrl-D or Backspace or DEL
			printf("*** User interrupt (char = 0x%02x)\r\n", c);
			break;
		}

		if(!parse_data_flag)
			continue;

		if(c == '\r' || c == '\n') {
			// Parse collected string
		
			str[idx] = 0;

			#if(DEBUG_CLI>1)
			printf("*** IHEX: %s, idx = %d\r\n", str, idx);
			#endif

			int data_size = strntoul(str, 2, 16);

			int len = strlen(str);

			if(len != data_size*2+10) {
				printf("*** Calculated str size %d mismatches received len %d\r\n", data_size*2+10, len);
				goto end_parse;
			}


			uint8_t sum = 0;

			for(int i = 0; i < len - 2; i += 2) 
				sum += strntoul(str+i, 2, 16);

			sum = ~sum + 1; 

			uint8_t his_sum = (uint8_t) strntoul(str+8+data_size*2, 2, 16);

			if(sum != his_sum) {
				printf("*** Checksum mismatch: sum = %02x, his = %02x\r\n", sum, his_sum);
				goto end_parse;
			}

			int offset = strntoul(str+2, 4, 16);

			int type = strntoul(str+6, 2, 16);

			printf("*** IHEX type: %d, size: %d, offset: 0x%04x\r\n", type, data_size, offset);

			if(type == 0) { // Data record
				for(int i = 0; i < data_size; i ++) 
					*(addr + base + offset + i) = strntoul(str+(i*2)+8, 2, 16);
			}

			end_parse:
				parse_data_flag = 0;

		} else {
			str[idx++] = c;

			if(idx >= 256*2+10) {
				str[idx] = 0;
				printf("*** Too long: %s\r\n", str);
				parse_data_flag = 0;
			}
		}
	}

	//csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts
}

void cli_cmd_stats(char *argv[], int argn) {

	if(argv[1])
		reg_sys_print_stats = strtoul(argv[1], NULL, 0);;

	printf("reg_sys_print_stats = %d\r\n", reg_sys_print_stats);
}
	
// Process CLI command once Enter is pressed
void cli_process_command(uint8_t *cmdline, uint32_t len) {

	char *cmdsep = " \t";
	char *argv[ARGN_MAX];
	int argn = 0;

	if(cmdline == NULL || len == 0)
		return;

	#if(DEBUG_CLI>1)
	printf("\r\nYou entered: %s\r\n\r\n", cmdline);
	#else
	printf("\r\n");
	#endif

	memset(argv, 0, sizeof(argv[0]) * ARGN_MAX); // zero argv pointers

	for(char* word = strtok(cmdline, cmdsep);
	    word && argn < ARGN_MAX;
	    word = strtok(NULL, cmdsep)) {
		argv[argn++] = word;
	}

	if(argn == 0) {
		#if(DEBUG_CLI)
		printf("*** argn is 0!\r\n");
		#endif
		return;
	}

	#if(DEBUG_CLI>1)
	for(int i = 0; i < argn; i++) {
		printf("ARGV[%u]: %p, ", i, argv[i]);
		printf("'%s'\r\n", argv[i]);
	}
	#endif

	// Convert command to lower case
	
	for(int i = 0; argv[0][i]; i++)
		argv[0][i] = tolower(argv[0][i]);

	// Interpret command
	
	if(argv[0][0] == '?' || (argv[0][0] == 'h' && argv[0][1] == 'e')) {
		cli_cmd_help(argv, argn);
		return;
	}

	if(argv[0][0] == 's' && argv[0][1] == 't') {
		cli_cmd_stats(argv, argn);
		return;
	}

	if(argv[0][0] == 'r' && argv[0][1] == 'b') {
		cli_cmd_read_byte(argv, argn);
		return;
	}

	if(argv[0][0] == 'r' && argv[0][1] == 'd') {
		cli_cmd_read_dword(argv, argn);
		return;
	}

	if(argv[0][0] == 'w' && argv[0][1] == 'b') {
		cli_cmd_write_byte(argv, argn);
		return;
	}

	if(argv[0][0] == 'w' && argv[0][1] == 'd') {
		cli_cmd_write_dword(argv, argn);
		return;
	}

	if(argv[0][0] == 'c' && argv[0][1] == 'a') {
		cli_cmd_call(argv, argn);
		return;
	}

	if(argv[0][0] == 'd' && argv[0][1] == 'u') {
		cli_cmd_dump(argv, argn);
		return;
	}

	if(argv[0][0] == 'i' && argv[0][1] == 'h') {
		cli_cmd_input_ihex(argv, argn);
		return;
	}

	cli_process_command_end:
	;
}


// Parse key stroke buffer, perform basic CLI editing features
void cli_process_input(uint8_t *buf, uint32_t len) {

	if(len == 0 || buf == NULL)
	       return;

	static int esc_flag = 0;

	for(int i = 0; i < len; i++) {
		uint8_t c = buf[i];

		if(esc_flag == 1) {
			if(c == 0x5b) {
				esc_flag = 2;
			} else
				esc_flag = 0;
			continue;
		}

		if(esc_flag == 2) {
			esc_flag = 0;

			if(c == 0x41) {
				cli_history_popup();
				printf("\033[2K"); // erase current line
				goto cli_process_input_end;
			}
			if(c == 0x42) {
				cli_history_popdown();
				printf("\033[2K"); // erase current line
				goto cli_process_input_end;
			}

			continue;
		}

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
				char cli_tmp[CLI_BUF_SIZE+1];
				if(cli_buf_len == 0) {
					cli_history_popup();
					printf("\033[2K"); // erase current line
					goto cli_process_input_end;
				}

				cli_buf[cli_buf_len] = 0;
				memcpy(cli_tmp, cli_buf, cli_buf_len+1);
				cli_process_command(cli_tmp, cli_buf_len);
				cli_history_push();
				cli_buf_len = 0;
				break;
			}

			case 0x1b: {
				esc_flag = 1;
				break;
			}

			case 0x15: { // Ctrl-U
				printf("\033[2K"); // erase current line
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

	cli_process_input_end:

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

	uint32_t rx_len = MIN(console_rx_buf_len, CONSOLE_RX_BUF_SIZE);
	memcpy(rx_buf, (void*)console_rx_buf, rx_len);
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

