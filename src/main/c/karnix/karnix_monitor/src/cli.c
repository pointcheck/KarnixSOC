#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <alloca.h>
#include "riscv.h"
#include "soc.h"
#include "qspi.h"
#include "utils.h"
#include "crc32.h"
#include "context.h"
#include "zmodem.h"

#define	CLI_BUF_SIZE       	(128*2)
#define	CLI_HISTORY_SIZE	4
#define CONSOLE_RX_BUF_SIZE     (128*2)
#define CONSOLE_RX_DELAY_US     10000

#define	DEBUG_CLI		1		// 0 - off, 1 - few, 2 - more
#define	ARGN_MAX		8		// Max number of arguments in cmd line
#define	CRC32_POLYNOMIAL	0xEDB88320	// same as in "cksum -o 3"
#define	OHEX_BYTES_PER_LINE	16		// num of bytes in IHEX line, should be power of 2
#define	PATHLEN			257		// Max file name len in 4.2BSD.

volatile uint32_t console_rx_buf_len = 0;
volatile uint8_t console_rx_buf[CONSOLE_RX_BUF_SIZE];


uint8_t cli_history[CLI_HISTORY_SIZE][CLI_BUF_SIZE+1] = {0};
uint32_t cli_history_idx = 0;

uint8_t *cli_buf = &cli_history[0][0];
uint32_t cli_buf_len = 0;

uint32_t current_address = 0x80000000;
extern volatile uint32_t reg_sys_print_stats;

void* ZModemWriteAddress;

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

void cli_cmd_help(char *argv[], int argn) {
	printf(
"/// List of general commands:\r\n"
"stats	[period]		- Enable printing statistics each 'period' sec, use 0 to disable.\r\n"
"r[b|d]	[*|addr] [len]		- Read and print 'len' bytes or dwords of memory beginning at 'addr'.\r\n"
"w[b|d]	[*|addr] [data] [many]	- Write 'data' byte or dword to memory at 'addr' as 'many' times.\r\n"
"addr	[*|addr]		- Set current address pointer to 'addr'.\r\n"
"call	[*|addr] [args]		- Call subroutine at 'addr', 'args' will be provided as argv/argn.\r\n"
"dump	[*|addr] [len]		- Read 'len' bytes from memory at 'addr' and print in ASCII.\r\n"
"type	[*|addr]		- Print ASCII string in memory at 'addr'.\r\n"
"ihex	[*|addr]		- Input IHEX, decode and store at 'addr' or current location.\r\n"
"ohex	[*|addr] [len] [entry]  - Ouput 'len' bytes of mem in IHEX format beginning at 'addr'.\r\n"
"crc	[*|addr] [len] [poly]	- Calc CRC32 of mem block beginning at 'addr' and size of 'len' bytes.\r\n"
"rz	[*|addr]		- Receive file over ZModem to mem 'addr'.\r\n"
"nor	[?|erase|cp]		- NOR flash operations.\r\n"
"\r\n"
	);
}


void cli_cmd_nor_help(char *argv[], int argn) {
	printf(
"/// List of NOR flash commands:\r\n"
"nor	erase <addr> <len>	- Erase sectors beginnign at 'addr', ending at 'addr+len'.\r\n"
"nor	cp <addr1> <addr2> <len>	- Copy data 'len' bytes of data from memory 'addr2' to NOR flash at 'addr1'\r\n"
	);
}


void cli_cmd_nor_erase(char *argv[], int argn) {
	uint8_t *addr = (uint8_t*) current_address;
	int len = 1;

	if(argn < 4) {
		cli_cmd_nor_help(argv, argn);
		return;
	}

	if(argv[2] && argv[2][0] != '*')
		addr = (uint8_t*) strtoul(argv[2], NULL, 0);

	if(argv[3])
		len = strtoul(argv[3], NULL, 0);

	#if(DEBUG_CLI)
		printf("/// nor erase: addr = %p, len = %u\r\n", addr, len);
	#endif

	current_address = (uint32_t) addr; // remember last address used

        uint32_t t0 = get_mtime();
	uint32_t i;

	for(i = 0; i < len; i += 4096) {
		printf("%p\r", (addr + i));
		fflush(stdout);

                qspi_erase_sector((uint32_t)(addr+i));

                while(qspi_get_status() & QSPI_DEVICE_STATUS_BUSY);

		if(console_rx_buf_len)
			break;
	}

        uint32_t t1= get_mtime();

        printf("\r\n/// nor erase: complete %u bytes in %u uS, status = %p\r\n", i, t1-t0, qspi_get_status());
}


void cli_cmd_nor_copy(char *argv[], int argn) {
	uint32_t *addr1 = (uint32_t*) current_address;
	uint32_t *addr2 = (uint32_t*) current_address;
	uint32_t len = 4;

	if(argn < 5) {
		cli_cmd_nor_help(argv, argn);
		return;
	}

	if(argv[2] && argv[2][0] != '*')
		addr1 = (uint32_t*) strtoul(argv[2], NULL, 0);

	if(argv[3] && argv[3][0] != '*')
		addr2 = (uint32_t*) strtoul(argv[3], NULL, 0);

	if(argv[4])
		len = strtoul(argv[4], NULL, 0) & 0xfffffffc;

	#if(DEBUG_CLI)
		printf("/// nor copy: to = %p, from = %p, len = %u\r\n", addr1, addr2, len);
	#endif

	current_address = (uint32_t) addr1; // remember last address used

        uint32_t t0 = get_mtime();
	uint32_t i;

	for(i = 0; i < len/4; i ++) {
		qspi_write_enable();

		*(addr1++) = *(addr2++);

                while(qspi_get_status() & QSPI_DEVICE_STATUS_BUSY);

		if(console_rx_buf_len)
			break;
	}

        uint32_t t1 = get_mtime();

        printf("\r\n/// nor copy: complete %d bytes in %u uS, status = 0x%02x\r\n", i*4, t1-t0, qspi_get_status());
}


void cli_cmd_read_byte(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	int len = 1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);

	if(argv[2])
		len = strtoul(argv[2], NULL, 0);

	#if(DEBUG_CLI)
		printf("/// rb: addr = %p, len = %u\r\n", addr, len);
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
		printf("/// rd: addr = %p, len = %u\r\n", addr, len);
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
		printf("/// dump: addr = %p, len = %u\r\n", addr, len);
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


void cli_cmd_type(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	int len = 1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);

	#if(DEBUG_CLI)
		printf("/// type: addr = %p\r\n", addr);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	printf("%s\r\n", addr);
}


void cli_cmd_crc32(char *argv[], int argn) {

	uint32_t *addr = (uint32_t*) current_address;
	uint32_t poly = CRC32_POLYNOMIAL;
	int len = 4;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint32_t*) strtoul(argv[1], NULL, 0);

	if(argv[2])
		len = strtoul(argv[2], NULL, 0);

	if(argv[3])
		poly = strtoul(argv[3], NULL, 0);

	#if(DEBUG_CLI)
		printf("/// crc32: addr = %p, len = %u bytes, polynomial = %p\r\n", addr, len, poly);
	#endif

	int count = 0;
	char str[24];

	current_address = (uint32_t) addr; // remember last address used

	uint32_t crc = crc32(addr, len, 0, poly);

	printf("/// crc32: %p\r\n", crc);
}


void cli_cmd_addr(char *argv[], int argn) {

	uint32_t *addr = (uint32_t*) current_address;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint32_t*) strtoul(argv[1], NULL, 0);

	current_address = (uint32_t) addr; // remember last address used

	#if(DEBUG_CLI)
		printf("/// addr = %p\r\n", addr);
	#endif
}

void cli_cmd_rz(char *argv[], int argn) {

	uint32_t *addr = (uint32_t*) current_address;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint32_t*) strtoul(argv[1], NULL, 0);

	#if(DEBUG_CLI)
		printf("/// rx zmodem: addr = %p\r\n", addr);
	#endif

	ZModemWriteAddress = addr;

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts

	// Allocate room for filename on stack (257 bytes)
	char *filename = (char*) alloca(PATHLEN);

	// Receive by ZModem: three attempts, 1M max file size
	int rc = wcreceive(15, 1024*1024, filename);

	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	if(rc == 0) {
		printf("/// rx zmodem success: read_bytes = %d, file = %s, crc32 = %p\r\n",
			ZModemRxBytes, filename, crc32((uint8_t*)addr, ZModemRxBytes, 0, CRC32_POLYNOMIAL));
	} else {
		printf("/// rx zmodem fail: rc = %d, read_bytes = %u, left_bytes = %u, max = %u\r\n",
				rc, ZModemRxBytes, ZModemBytesleft, 1024*1024);
	}

	current_address = (uint32_t) addr; // remember last address used
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
		printf("/// wb: addr = %p, value = %02x, many = %d\r\n", addr, value, many);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	for(int i = 0; i < many; i++) {
		// Is this writing to QSPI NOR flash ?
		if(addr >= (uint8_t*)(QSPI_MEMORY_ADDRESS) &&
		   addr < (uint8_t*)(QSPI_MEMORY_ADDRESS+QSPI_MEMORY_SIZE))
			qspi_write_enable();
			
		*addr++ = value;
	}
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
		printf("/// wd: addr = %p, value = %p, many = %d\r\n", addr, value, many);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	for(int i = 0; i < many; i++) {
		// Is this writing to QSPI NOR flash ?
		if(addr >= (uint32_t*)(QSPI_MEMORY_ADDRESS) &&
		   addr < (uint32_t*)(QSPI_MEMORY_ADDRESS+QSPI_MEMORY_SIZE))
			qspi_write_enable();
			
		*addr++ = value;
	}

}


void cli_cmd_call(char *argv[], int argn) {

	uint32_t *addr = (uint32_t*) current_address;
	int len = 1;
	uint64_t t0, t1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint32_t*) strtoul(argv[1], NULL, 0);

	#if(DEBUG_CLI)
		printf("/// call: addr = %p, argn = %d\r\n", addr, argn-1);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	uint32_t (*long_jump)(char *argv[], int arg) = (uint32_t (*)(char *argv[], int arg)) addr;

	t0 = get_mtime();

	uint32_t rc = long_jump(&(argv[1]), argn-1);

	t1 = get_mtime();

	// Anonymous function possibly messed up with our context,
	// so restore context completely.
	context_restore();

	printf("/// call: ret = %p, exec time = %lu millisecs\r\n", rc, t1 - t0);
}

void cli_cmd_ihex(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	uint32_t base = 0, offset = 0, start16 = 0, start32 = 0;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);


	printf("/// ihex: addr = %p, press Ctrl-C to break.\r\n", addr);

	current_address = (uint32_t) addr; // remember last address used

	//csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts

	int parse_data_flag = 0;

	uint8_t str[256*2+10+1]; // max len of IHEX string
	uint32_t idx = 0;
	uint32_t console_rx_buf_idx = 0;
	uint32_t bytes_read = 0;
	uint32_t crc = 0;

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
			printf("/// ihex: User interrupt (char = 0x%02x)\r\n", c);
			break;
		}

		if(!parse_data_flag)
			continue;

		if(c == '\r' || c == '\n') {
			// Parse collected string
		
			str[idx] = 0;

			#if(DEBUG_CLI>2)
			printf("/// ihex: %s, idx = %d\r\n", str, idx);
			#endif

			int data_size = strntoul(str, 2, 16);

			int len = strlen(str);

			if(len != data_size*2+10) {
				printf("/// ihex: Calculated str size %d mismatches received len %d\r\n", data_size*2+10, len);
				goto end_parse;
			}


			uint8_t sum = 0;

			for(int i = 0; i < len - 2; i += 2) 
				sum += strntoul(str+i, 2, 16);

			sum = ~sum + 1; 

			uint8_t his_sum = (uint8_t) strntoul(str+8+data_size*2, 2, 16);

			if(sum != his_sum) {
				printf("/// ihex: Checksum mismatch: sum = %02x, his = %02x\r\n", sum, his_sum);
				goto end_parse;
			}

			int offset = strntoul(str+2, 4, 16);

			int type = strntoul(str+6, 2, 16);


			// Data record
			// The byte count specifies number of data bytes in the record.
			// The example has 0B (eleven) data bytes. The 16-bit starting address for the data
			// (in the example at addresses beginning at 0010) and the data
			// ex: 0B0010006164647265737320676170A7
			if(type == 0) {
				for(int i = 0; i < data_size; i ++) 
					*(addr + base + offset + i) = strntoul(str+(i*2)+8, 2, 16);

				crc = crc32((const void*)(addr + base + offset), data_size, crc, CRC32_POLYNOMIAL);

				bytes_read += data_size;
			}

			// End Of File
			// Must occur exactly once per file in the last record of the file.
			// The byte count is 00, the address field is typically 0000 and the data field is omitted.
			if(type == 1)
				break;

			// Start Linear Address
			// The byte count is always 04, the address field is 0000.
			// The four data bytes represent a 32-bit address value (big endian).
			// In the case of CPUs that support it, this 32-bit address is the address
			// at which execution should start.
			if(type == 5)
				start32 = strntoul(str+8, 8, 16); 

			// Extended Linear Address
			// Allows for 32 bit addressing (up to 4 GiB). The byte count is always 02 and the address field
			// is ignored (typically 0000). The two data bytes (big endian) specify the upper 16 bits
			// of the 32 bit absolute address for all subsequent type 00 records; these upper address bits
			// apply until the next 04 record. The absolute address for a type 00 record is formed
			// by combining the upper 16 address bits of the most recent 04 record with the low 16 address
			// bits of the 00 record. If a type 00 record is not preceded by any type 04 records then its upper
			// 16 address bits default to 0000. 
			if(type == 4)
				base = strntoul(str+8, 4, 16) << 16;

			// Extended Segment Address
			// The byte count is always 02, the address field (typically 0000) is ignored and the data field
			// contains a 16-bit segment base address. This is multiplied by 16 and added to each subsequent
			// data record address to form the starting address for the data. This allows addressing up to one
			// mebibyte (1048576 bytes) of address space.
			if(type == 2)
				base = strntoul(str+8, 4, 16) << 4;

			#if(DEBUG_CLI>1)
			printf("/// ihex tp: %d, sz: %d, addr: %p\r\n",
					type, data_size, addr + base + offset);
			#endif

			end_parse:
				parse_data_flag = 0;

		} else {
			str[idx++] = c;

			if(idx >= 256*2+10) {
				str[idx] = 0;
				printf("/// Too long: %s\r\n", str);
				parse_data_flag = 0;
			}
		}
	}

	printf("/// ihex: bytes_read = %u, addr = %p, entry = %p, crc32 = %p (cksum -o 3)\r\n",
			bytes_read, addr, addr + start32, crc);

}

void cli_cmd_ohex(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	uint32_t start32 = 0, len = 1, offset = 0;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);

	if(argv[2])
		len = strtoul(argv[2], NULL, 0);

	if(argv[3])
		start32 = strtoul(argv[3], NULL, 0) - (uint32_t)addr;

	printf("/// ohex: addr = %p, len = %u, entry = %p, press Ctrl-C to break.\r\n", addr, len, start32);

	current_address = (uint32_t) addr; // remember last address used

	while(offset < len) {
		uint8_t hex_len = (offset + OHEX_BYTES_PER_LINE <= len) ? 
			OHEX_BYTES_PER_LINE  : (len - offset) % OHEX_BYTES_PER_LINE;
		printf(":%02X%04X%02X", hex_len, offset & 0xffff, 0x0);
		uint8_t sum = hex_len + ((offset>>8)&0xff)+(offset&0xff)+0;
		for(int i = 0; i < hex_len; i++) {
			uint8_t byte = *(addr + offset++);
			printf("%02X", byte);
			sum += byte;
		}
		sum = ~sum + 1;
		printf("%02X\r\n", sum);

		if(console_rx_buf_len) {
			printf("/// ohex: User interrupt\r\n");
			break;
		}

		if((offset & 0xffff) == 0) // 64K overlap ?
			printf(":02000004%04X%02X\r\n", (offset >> 16) & 0xffff,
				(~(2+0+0+4+((offset>>24)&0xff)+((offset>>16)&0xff)) + 1) & 0xff);
	}

	if(offset == len) // Successful end ?
		printf(":04000005%08X%02X\r\n"
		       ":00000001FF\r\n", start32,
		       (~(4+0+0+5+((start32>>24)&0xff)+((start32>>16)&0xff)+((start32>>8)&0xff)+(start32&0xff)) + 1) & 0xff);

	printf("/// ohex: end\r\n%c", 0x4); // send End-of-Transmission (Ctrl-D) in the end
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
		printf("/// argn is 0!\r\n");
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

	if(argv[0][0] == 't' && argv[0][1] == 'y') {
		cli_cmd_type(argv, argn);
		return;
	}

	if(argv[0][0] == 'i' && argv[0][1] == 'h') {
		cli_cmd_ihex(argv, argn);
		return;
	}

	if(argv[0][0] == 'o' && argv[0][1] == 'h') {
		cli_cmd_ohex(argv, argn);
		return;
	}

	if(argv[0][0] == 'c' && argv[0][1] == 'r') {
		cli_cmd_crc32(argv, argn);
		return;
	}

	if(argv[0][0] == 'a' && argv[0][1] == 'd') {
		cli_cmd_addr(argv, argn);
		return;
	}

	if(argv[0][0] == 'r' && argv[0][1] == 'z') {
		cli_cmd_rz(argv, argn);
		return;
	}

	//if(strncasecmp(argv[0], "**B01000000", 11) == 0) {
	if(argv[0][0] == '*' && argv[0][1] == '*' && argv[0][2] == 'B' && argv[0][3] == '0') {
		cli_cmd_rz(argv, argn);
		return;
	}

	if(argv[0][0] == 'n' && argv[0][1] == 'o') {

		if(argn < 4) {
			cli_cmd_nor_help(argv, argn);
			return;
		}

		if(argv[1][0] == 'e' && argv[1][1] == 'r')
			cli_cmd_nor_erase(argv, argn);
		else if(argv[1][0] == 'c' && argv[1][1] == 'p')
			cli_cmd_nor_copy(argv, argn);
		else
			cli_cmd_nor_help(argv, argn);
		return;
	}

	if(argv[0][0] != 0)
		printf("/// Unknown command: %s\r\n", argv[0]);

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

