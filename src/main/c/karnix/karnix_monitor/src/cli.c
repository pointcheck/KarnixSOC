#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <alloca.h>
#include "riscv.h"
#include "soc.h"
#include "qspi.h"
#include "sdmmc.h"
#include "crc32.h"
#include "context.h"
#include "zmodem.h"
#include "cli.h"
#include "cga.h"
#include "utils.h"

//#define	DEBUG_CLI		2		// 0 - off, 1 - few, 2 - more
#define	ARGN_MAX		8		// Max number of arguments in cmd line
#define	CRC32_POLYNOMIAL	0xEDB88320	// same as in "cksum -o 3"
#define	OHEX_BYTES_PER_LINE	16		// num of bytes in IHEX line, should be power of 2
#define	PATHLEN			257		// Max file name len in 4.2BSD.

void cli_cmd_stats();
void cli_cmd_usb();
void cli_cmd_cga();
void cli_cmd_read_byte();
void cli_cmd_read_dword();
void cli_cmd_write_byte();
void cli_cmd_write_dword();
void cli_cmd_addr();
void cli_cmd_call();
void cli_cmd_copy();
void cli_cmd_dump();
void cli_cmd_type();
void cli_cmd_ihex();
void cli_cmd_ohex();
void cli_cmd_crc32();
void cli_cmd_rz();
void cli_cmd_reg();
void cli_cmd_nor();
void cli_cmd_sdmmc();
void cli_cmd_fat32();
void cli_cmd_help();

const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

struct _command_list {
	char cmd[4];
	void *func;
	char *help;
} command_list[] = {
	{
		.cmd = "help",
		.func = cli_cmd_help,
		.help = "help|?	[cmd]			- Print help on command 'cmd'"
	},
	{
		.cmd = "?",
		.func = cli_cmd_help,
		.help = (char*)-1,
	},
	{
		.cmd = "stat",
		.func = cli_cmd_stats,
		.help = "stat	[period]		- Enable printing statistics each 'period' sec,\r\n"
			"				  use 0 to disable"
	},
	{
		.cmd = "usb",
		.func = cli_cmd_usb,
		.help = "usb	[1/0]			- Enable/disable printing USB data"
	},
	{
		.cmd = "cga",
		.func = cli_cmd_cga,
		.help = "cga	[1/0]			- Choose CGA format: 0 - default, 1 - Addi 7\""
	},
	{
		.cmd = "rb",
		.func = cli_cmd_read_byte,
		.help = "rb|rd	[*|addr] [len]		- Read and print 'len' bytes or dwords of mem\r\n"
			"				  beginning at 'addr'"
	},
	{
		.cmd = "rd",
		.func = cli_cmd_read_dword,
		.help = (char*)-1
	},
	{
		.cmd = "wb",
		.func = cli_cmd_write_byte,
		.help = "wb|wd	[*|addr] [data] [many]	- Write 'data' byte or dword to mem at 'addr'\r\n"
			"				  as 'many' times"
	},
	{
		.cmd = "wd",
		.func = cli_cmd_write_dword,
		.help = (char*)-1
	},
	{
		.cmd = "addr",
		.func = cli_cmd_addr,
		.help = "addr	[*|addr]		- Set current address pointer to 'addr'"
	},
	{
		.cmd = "call",
		.func = cli_cmd_call,
		.help = "call	[*|addr] [args]		- Call subroutine at 'addr', 'args' will be\r\n"
			"				  provided as argv/argn"
	},
	{
		.cmd = "copy",
		.func = cli_cmd_copy,
		.help = "copy	[*|to] [*|from] [len]	- Copy 'len' bytes of memory from 'from' to 'to'"
	},
	{
		.cmd = "dump",
		.func = cli_cmd_dump,
		.help = "dump	[*|addr] [len]		- Read 'len' bytes from mem at 'addr' and print\r\n"
			"				  in ASCII"
	},
	{
		.cmd = "type",
		.func = cli_cmd_type,
		.help = "type	[*|addr]		- Print ASCII string in mem at 'addr'"
	},
	{
		.cmd = "ihex",
		.func = cli_cmd_ihex,
		.help = "ihex	[*|addr]		- Input IHEX, decode and store at 'addr' or at\r\n"
			"				  current location"
	},
	{
		.cmd = "ohex",
		.func = cli_cmd_ohex,
		.help = "ohex	[*|addr] [len] [entry]  - Output 'len' bytes of mem in IHEX format\r\n"
			"				  starting at 'addr'"
	},
	{
		.cmd = "crc",
		.func = cli_cmd_crc32,
		.help = "crc32	[*|addr] [len] [poly]	- Calc CRC32 of mem block starting at 'addr' and\r\n"
			"				  size of 'len' bytes"
	},
	{
		.cmd = "sd",
		.func = cli_cmd_sdmmc,
		.help = "sd	[cmd]			- SD/MMC card operations:\r\n"
			"	list			- Show available SD/MMC interafces\r\n"
			"	init dev		- Initialize SD/MMC card interface number 'dev'\r\n"
			"	info dev		- Show available cards\r\n"
			"	read dev blk addr cnt	- Read 'cnt' blocks from 'dev' at 'blk' to 'addr'\r\n"
			"	write dev blk addr cnt	- Write 'cnt' blocks to 'dev' at 'blk' from 'addr'\r\n"
			"	test dev blk addr cnt	- Test 'cnt' blocks on 'dev' at 'blk' using buf 'addr'\r\n"
			"	wp [blk]		- Set/Set Write-protected sectors below 'blk'"
	},
	{
		.cmd = "fat",
		.func = cli_cmd_fat32,
		.help = "fat	[cmd]			- FAT32 operations:\r\n"
			"	mount dev [mnt]		- Mount FAT32 on SD/MMC 'dev' to 'mnt'\r\n"
			"	umount [mnt]		- Unmount FAT from 'mnt'\r\n"
			"	ls /mnt/dir		- List dir entry at '/mnt/dir'\r\n"
			"	cp /mnt/src /mnt/dst	- Copy '/mnt/src' file to '/mnt/dst'\r\n"
			"	mkdir /mnt/dir		- Create directory '/mnt/dir'\r\n"
			"	rm /mnt/path		- Remove file of directory '/mnt/path'\r\n"
			"	cat /mnt/file		- Type ASCII file '/mnt/file'\r\n"
			"	dump /mnt/file		- HEX dump file '/mnt/file'\r\n"
			"	load addr /mnt/file	- Load binary '/mnt/file' to addr\r\n"
			"	save addr size /mnt/file- Save mem block at addr to file '/mnt/file'"
	},
	{
		.cmd = "rz",
		.func = cli_cmd_rz,
		.help = "rz	[*|addr]		- Receive file over ZModem to mem 'addr'"
	},
	{
		.cmd = "reg",
		.func = cli_cmd_reg,
		.help = "reg				- Print current context registers"
	},
	{
		.cmd = "nor",
		.func = cli_cmd_nor,
		.help = "nor	[?|erase|cp]		- NOR flash operations:\r\n"
			"	erase <addr> <len>	- Erase sectors beginning 'addr', ending 'addr+len'.\r\n"
			"	cp <addr1> <addr2> <len>- Copy 'len' bytes of data from memory 'addr2'\r\n"
			"				  to NOR flash at 'addr1'"
	}
};


void welcome(void);

volatile uint32_t console_rx_buf_len = 0;
volatile uint8_t console_rx_buf[CONSOLE_RX_BUF_SIZE] = {0};
volatile uint32_t console_rx_timestamp = 0;


uint8_t cli_history[CLI_HISTORY_SIZE][CLI_BUF_SIZE+1] = {0};
uint32_t cli_history_idx = 0;

uint8_t *cli_buf = cli_history[0];
uint32_t cli_buf_len = 0;

uint32_t current_address = 0x80000000;
extern volatile uint32_t reg_sys_print_stats;
extern volatile uint32_t reg_usb_print_stats;

void* ZModemWriteAddress;

void cli_process_command(uint8_t *cmd, uint32_t len);


void cli_history_push(void) {
	cli_history_idx = (cli_history_idx + 1) % CLI_HISTORY_SIZE;
	cli_buf = cli_history[cli_history_idx];
	cli_buf[0] = 0;
	cli_buf_len = 0;
}

void cli_history_popup(void) {
	if(cli_history_idx == 0)
		cli_history_idx = CLI_HISTORY_SIZE-1;
	else
		cli_history_idx--;
	cli_buf = cli_history[cli_history_idx];
	cli_buf_len = strlen(cli_buf);
}

void cli_history_popdown(void) {
	cli_history_idx = (cli_history_idx + 1) % CLI_HISTORY_SIZE;
	cli_buf = cli_history[cli_history_idx];
	cli_buf_len = strlen(cli_buf);
}

void cli_prompt(void) {
	xprintf("\rMONITOR[%p]-> %s", current_address, cli_buf);
	//fflush(stdout);
}

void show_help(char *argv[], int argn) {

	if(argv[0] == NULL)
		argv[0] = "";

	for(int i = 0; i < sizeof(command_list)/sizeof(command_list[0]); i++)
		if(command_list[i].cmd)
			if(strnstr(command_list[i].cmd, argv[0], 4)) {
				// -1 is special case: refers to prev item
				if((int)command_list[i].help == -1)
					if(argv[0][0])
						xprintf("%s\r\n", command_list[i-1].help);
					else {}
				else if(command_list[i].help)
					xprintf("%s\r\n", command_list[i].help);
			}

	xprintf("\r\n");
}


void cli_cmd_help(char *argv[], int argn) {
	welcome();

	show_help(++argv, --argn);
}


void cli_cmd_nor_erase(char *argv[], int argn) {
	uint8_t *addr = (uint8_t*) current_address;
	int len = 1;

	if(argn < 4) {
		show_help(argv, argn);
		return;
	}

	if(argv[2] && argv[2][0] != '*')
		addr = (uint8_t*) strtoul(argv[2], NULL, 0);

	if(argv[3])
		len = strtoul(argv[3], NULL, 0);

	#if(DEBUG_CLI)
		xprintf("nor erase: addr = %p, len = %u\r\n", addr, len);
	#endif

	current_address = (uint32_t) addr; // remember last address used

        uint32_t t0 = get_mtime();
	uint32_t i;

	for(i = 0; i < len; i += 4096) {
		xprintf("%p\r", (addr + i));
		fflush(stdout);

                qspi_erase_sector((uint32_t)(addr+i));

                while(qspi_get_status() & QSPI_DEVICE_STATUS_BUSY);

		if(console_rx_buf_len)
			break;
	}

        uint32_t t1= get_mtime();

        xprintf("\r\nnor erase: complete %u bytes in %u uS, status = %p\r\n", i, t1-t0, qspi_get_status());
}


void cli_cmd_nor_copy(char *argv[], int argn) {
	uint32_t *addr1 = (uint32_t*) current_address;
	uint32_t *addr2 = (uint32_t*) current_address;
	uint32_t len = 4;

	if(argn < 5) {
		show_help(argv, argn);
		return;
	}

	if(argv[2] && argv[2][0] != '*')
		addr1 = (uint32_t*) strtoul(argv[2], NULL, 0);

	if(argv[3] && argv[3][0] != '*')
		addr2 = (uint32_t*) strtoul(argv[3], NULL, 0);

	if(argv[4])
		len = strtoul(argv[4], NULL, 0) & 0xfffffffc;

	#if(DEBUG_CLI)
		xprintf("nor copy: to = %p, from = %p, len = %u\r\n", addr1, addr2, len);
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

        xprintf("\r\nnor copy: complete %d bytes in %u uS, status = 0x%02x\r\n", i*4, t1-t0, qspi_get_status());
}


void cli_cmd_read_byte(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	int len = 1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);

	if(argv[2])
		len = strtoul(argv[2], NULL, 0);

	#if(DEBUG_CLI)
		xprintf("rb: addr = %p, len = %u\r\n", addr, len);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	int count = 0;

	while(count < len) {
		xprintf("rb[%p]:	", addr);
		for(int i = 0; i < 16 && count++ < len; i++)
		       xprintf("0x%02x ", *addr++);	
		xprintf("\r\n");

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
		xprintf("rd: addr = %p, len = %u\r\n", addr, len);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	int count = 0;

	while(count < len) {
		xprintf("rd[%p]:	", addr);
		for(int i = 0; i < 4 && count++ < len; i++)
		       xprintf("0x%08x ", *addr++);	
		xprintf("\r\n");

		if(console_rx_buf_len)
			break;
	}
}

void cli_cmd_copy(char *argv[], int argn) {

	uint8_t *to = (uint8_t*) current_address;
	uint8_t *from = (uint8_t*) current_address;
	uint32_t len = 0;
	
	if(argv[1] && argv[1][0] != '*')
		to = (uint8_t*) strtoul(argv[1], NULL, 0);

	if(argv[2] && argv[2][0] != '*')
		from = (uint8_t*) strtoul(argv[2], NULL, 0);

	if(argv[3])
		len = strtoul(argv[3], NULL, 0);

	#if(DEBUG_CLI)
		xprintf("copy: to = %p, from = %p, len = %u\r\n", to, from, len);
	#endif

	memcpy(to, from, len);

	current_address = (uint32_t) to; // remember last address used
}

void cli_cmd_dump(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	int len = 1;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);

	if(argv[2])
		len = strtoul(argv[2], NULL, 0);

	#if(DEBUG_CLI)
		xprintf("dump: addr = %p, len = %u\r\n", addr, len);
	#endif

	int count = 0;
	char str[24];

	current_address = (uint32_t) addr; // remember last address used

	while(count < len) {
		xprintf("0x%02X: ", addr);

		str[0] = 0;

		for(int i = 0; i < 16 && count++ < len; i++) {
		       sprintf(str+i, "%c", ((*addr > 0x20) && (*addr < 0x7f)) ? *addr : '.');
		       xprintf("%02X ", *addr++);
		}

		xprintf("| %s\r\n", str);

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
		xprintf("type: addr = %p\r\n", addr);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	xprintf("%s\r\n", addr);
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
		xprintf("crc32: addr = %p, len = %u bytes, polynomial = %p\r\n", addr, len, poly);
	#endif

	int count = 0;
	char str[24];

	current_address = (uint32_t) addr; // remember last address used

	uint32_t crc = crc32(addr, len, 0, poly);

	xprintf("crc32: %p\r\n", crc);
}


void cli_cmd_sdmmc(char *argv[], int argn) {
	int count = 0;
	int dev = 0;
	uint32_t *addr = (uint32_t*) current_address;

	if(argv[1] && strnstr(argv[1], "li", 2)) { // list
		#if(DEBUG_CLI)
		xprintf("%s: available SD/MMC card devices:\r\n", "sdmmc");
		#endif

		for(int dev = 0; dev < SDMMC_DEVICES; dev++)
			xprintf("%s: dev = %d, reg = %p, ss = %d\r\n", "sdmmc",
				dev,
				sdmmc_devices[dev].reg,
				sdmmc_devices[dev].ss
			);

		return;
	}

	if(argv[2])
		dev = strtoul(argv[2], NULL, 0);

	if(dev >= SDMMC_DEVICES || dev < 0) {
		xprintf("%s: no such SD/MMC dev: %d\r\n", "sdmmc", dev);
		return;
	}

	if(argv[1] && strnstr(argv[1], "ini", 3)) { // init

		int ret = sdmmc_init(dev);
	
		xprintf("%s: init SD/MMC card dev: %d, ret = %d\r\n", "sdmmc", dev, ret);

		return;
	}

	if(argv[1] && strnstr(argv[1], "inf", 3)) { // info 

		uint8_t* cid = sdmmc_cards[dev].cid_data;

		xprintf("%s: dev = %d, type = %d (%s), OCR = 0x%08X, blocks = %d (%d MiB)\r\n", "sdmmc",
			dev, sdmmc_cards[dev].type,
			sdmmc_types[sdmmc_cards[dev].type],
			sdmmc_cards[dev].ocr,
			sdmmc_cards[dev].blocks,
			(sdmmc_cards[dev].blocks / 1024) * 512 / 1024
		);

		xprintf("%s: Card ID:	", "sdmmc");
		xprintf("MID: 0x%02X, ", cid[0]);
		xprintf("OID: %c%c, ", cid[1], cid[2]);
		xprintf("PNM: %c%c%c%c%c, ", cid[3], cid[4], cid[5], cid[6], cid[7]);
		xprintf("PRV: 0x%02X, ", cid[8]);
		xprintf("PSN: 0x%08X, ", (cid[9] << 0) | (cid[10] << 8)| (cid[11] << 16) | (cid[12] << 24));
		xprintf("MDT: %03X, ", ((cid[13] & 0x0f) << 8) | cid[14]);
		xprintf("CRC: 0x%02X\r\n", cid[15] >> 1);

		uint8_t* csd = sdmmc_cards[dev].csd_data;

		xprintf("%s: Card SD:	", "sdmmc");
		for(int i = 0; i < 16; i++)
			xprintf("%02X ", csd[i]);
		
		xprintf("\r\n");
			
		return;
	}

	if(argv[1] && strnstr(argv[1], "rea", 3)) { // read 

		int block = 0;
		int addr = current_address;
		int count = 1;

		if(argv[3])
			block = strtoul(argv[3], NULL, 0);

		if(argv[4] && argv[4][0] != '*')
			addr = strtoul(argv[4], NULL, 0);

		if(argv[5])
			count = strtoul(argv[5], NULL, 0);

		#if(DEBUG_CLI)
		xprintf("%s: read dev = %d, block = %d, addr = 0x%08x, count = %d\r\n", "sdmmc",
			dev, block, addr, count
		);
		#endif

        	uint32_t t0 = get_mtime();

		int ret = sdmmc_read_block(dev, block, count, (uint8_t*) addr); 

        	uint32_t dt = (get_mtime() - t0);
		uint32_t cps = 512 * count / (dt / 1024);

		xprintf("%s: read ret = %d, time = %d us, cps = %d KB/s\r\n", "sdmmc", ret, dt, cps);

		current_address = (uint32_t) addr; // remember last address used

		return;
	}

	if(argv[1] && strnstr(argv[1], "wri", 3)) { // write 

		int block = 0;
		int addr = current_address;
		int count = 1;

		if(argv[3])
			block = strtoul(argv[3], NULL, 0);

		if(argv[4] && argv[4][0] != '*')
			addr = strtoul(argv[4], NULL, 0);

		if(argv[5])
			count = strtoul(argv[5], NULL, 0);

		#if(DEBUG_CLI)
		xprintf("%s: write dev = %d, block = %d, addr = 0x%08x, count = %d\r\n", "sdmmc",
			dev, block, addr, count
		);
		#endif

        	uint32_t t0 = get_mtime();

		int ret = sdmmc_write_block(dev, block, count, (uint8_t*) addr); 

        	uint32_t dt = (get_mtime() - t0);
		uint32_t cps = 512 * count / (dt / 1024);

		xprintf("%s: write ret = %d, time = %d us, cps = %d KB/s\r\n", "sdmmc", ret, dt, cps);

		current_address = (uint32_t) addr; // remember last address used

		return;
	}

	if(argv[1] && strnstr(argv[1], "tes", 3)) { // test read/write 

		int block = 0;
		int addr = current_address;
		int count = 1;

		if(argv[3])
			block = strtoul(argv[3], NULL, 0);

		if(argv[4] && argv[4][0] != '*')
			addr = strtoul(argv[4], NULL, 0);

		if(argv[5])
			count = strtoul(argv[5], NULL, 0);

		#if(DEBUG_CLI)
		xprintf("%s: test dev = %d, block = %d, addr = 0x%08x, count = %d\r\n", "sdmmc",
			dev, block, addr, count
		);
		#endif

        	uint32_t t0 = get_mtime();
		uint32_t errs = 0;
		int i;

		for(i = 0; i < count; i++) {

			char *p = (char*)addr;

			for(int j = 0; j < 512; j++)
				*p++ = ((i+j) ^ 0xaa);

			char *a = (char*)addr;
			char *b = (char*)addr+512;

			if(sdmmc_read_block(dev, block+i, 1, (uint8_t*) b) < 0)
				break;

			if(sdmmc_write_block(dev, block+i, 1, (uint8_t*) a) < 0)
				break;

			if(sdmmc_read_block(dev, block+i, 1, (uint8_t*) b) < 0)
				break;


			for(int j = 0; j < 512; j++)
				if(*a++ != *b++) {
					xprintf("%s: test %d failed at %d, block = %d\r\n", "sdmmc", i, j, block+i);
					errs++;
					break;
				}

			if(console_rx_buf_len)
				break;
		}

        	uint32_t dt = (get_mtime() - t0);

		xprintf("%s: tests %d made, errors = %d, time = %d us\r\n", "sdmmc", i, errs, dt);

		current_address = (uint32_t) addr; // remember last address used

		return;
	}

	if(argv[1] && strnstr(argv[1], "wp", 2)) { // Set/Get write-protected sector number 

		if(argv[2])
			sdmmc_write_protected_sectors = strtoul(argv[2], NULL, 0);

		xprintf("%s: blocks below %d are write-protected!\r\n", "sdmmc", sdmmc_write_protected_sectors);

		return;
	}


	show_help(argv, argn);
}


void cli_cmd_cga(char *argv[], int argn) {

	if(argv[1]) {
		int format_idx = (uint32_t) strtoul(argv[1], NULL, 0) % CGA_NUM_FORMATS;
		memcpy((void*)&CGA->CTRL3, (void*)&cga_video_formats[format_idx], sizeof(CGA_Video_Format));

		xprintf("cga: loaded video format = %s (%d)\r\n",
			cga_video_formats[format_idx].name, format_idx);
	} else {
		xprintf("cga: available video formats:\r\n");
		for(int i = 0; i < CGA_NUM_FORMATS; i++)
		xprintf("	%d - %s\r\n", i, cga_video_formats[i]. name);

		xprintf("cga: current video format:\r\n");
	}

	xprintf(
		"	CTRL3_HFP_HBP:             0x%08X\r\n"
		"	CTRL4_HSPOL_HSYNC_HACTIVE: 0x%08X\r\n"
		"	CTRL5_VFP_VBP:             0x%08X\r\n"
		"	CTRL6_VSPOL_VSYNC_VACTIVE: 0x%08X\r\n",
		CGA->CTRL3, CGA->CTRL4, CGA->CTRL5, CGA->CTRL6
	);
}


void cli_cmd_addr(char *argv[], int argn) {

	uint32_t *addr = (uint32_t*) current_address;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint32_t*) strtoul(argv[1], NULL, 0);

	current_address = (uint32_t) addr; // remember last address used

	#if(DEBUG_CLI)
		xprintf("addr = %p\r\n", addr);
	#endif
}


void cli_cmd_reg(char *argv[], int argn) {

	xprintf("Context: sp = %p, gp = %p, tp = %p, ra = %p, pc = %p\r\n",
		context.sp, context.gp, context.tp, context.ra, context.pc);
}

void cli_cmd_rz(char *argv[], int argn) {

	uint32_t *addr = (uint32_t*) current_address;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint32_t*) strtoul(argv[1], NULL, 0);

	#if(DEBUG_CLI)
		xprintf("rx zmodem: addr = %p\r\n", addr);
	#endif

	ZModemWriteAddress = addr;

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts

	// Allocate room for filename on stack (257 bytes)
	char *filename = (char*) alloca(PATHLEN);

	// Receive by ZModem: three attempts, 1M max file size
	int rc = wcreceive(15, 1024*1024, filename);

	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	if(rc == 0) {
		xprintf("rx zmodem success: read_bytes = %d, file = %s, crc32 = %p\r\n",
			ZModemRxBytes, filename, crc32((uint8_t*)addr, ZModemRxBytes, 0, CRC32_POLYNOMIAL));
	} else {
		xprintf("rx zmodem fail: rc = %d, read_bytes = %u, left_bytes = %u, max = %u\r\n",
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
		xprintf("wb: addr = %p, value = %02x, many = %d\r\n", addr, value, many);
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
		xprintf("wd: addr = %p, value = %p, many = %d\r\n", addr, value, many);
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
		xprintf("*** Monitor context: SP = %p, GP = %p, RA = %p\r\n",
			(unsigned int)context.sp, (unsigned int)context.gp, (unsigned int)context.ra);
		xprintf("*** Monitor context: _stack_start = %p, _bss_start = %p, "
			"_bss_end = %p, _ram_heap_start = %p, _ram_heap_end = %p\r\n",
			(unsigned int)& _stack_start, (unsigned int)& _bss_start,
			(unsigned int)& _bss_end, (unsigned int)& _ram_heap_start, (unsigned int)& _ram_heap_end);
	#endif

	current_address = (uint32_t) addr; // remember last address used

	uint32_t (*long_jump)(char *argv[], int arg) = (uint32_t (*)(char *argv[], int arg)) addr;
	uint32_t rc;

	context_save();

	t0 = get_mtime();

	rc = long_jump(&(argv[1]), argn-1);

	// Anonymous function possibly messed up with our context,
	// so restore context completely.
	context_restore();

	#if(DEBUG_CLI)
		xprintf("*** Monitor context: SP = %p, GP = %p, RA = %p\r\n",
			(unsigned int)context.sp, (unsigned int)context.gp, (unsigned int)context.ra);
		xprintf("*** Monitor context: _stack_start = %p, _bss_start = %p, "
			"_bss_end = %p, _ram_heap_start = %p, _ram_heap_end = %p\r\n",
			(unsigned int)& _stack_start, (unsigned int)& _bss_start,
			(unsigned int)& _bss_end, (unsigned int)& _ram_heap_start, (unsigned int)& _ram_heap_end);
	#endif

	t1 = get_mtime();

	xprintf("call: ret = %p (%d), exec time = %lu usecs\r\n", rc, rc, (uint32_t)(t1 - t0));

}

void cli_cmd_ihex(char *argv[], int argn) {

	uint8_t *addr = (uint8_t*) current_address;
	uint32_t base = 0, offset = 0, origin = 0, start16 = 0, start32 = 0;
	
	if(argv[1] && argv[1][0] != '*')
		addr = (uint8_t*) strtoul(argv[1], NULL, 0);


	xprintf("ihex: addr = %p, press Ctrl-C to break.\r\n", addr);

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
			xprintf("ihex: User interrupt (char = 0x%02x)\r\n", c);
			break;
		}

		if(!parse_data_flag)
			continue;

		if(c == '\r' || c == '\n') {
			// Parse collected string
		
			str[idx] = 0;

			#if(DEBUG_CLI>2)
			xprintf("ihex: %s, idx = %d\r\n", str, idx);
			#endif

			int data_size = strntoul(str, 2, 16);

			int len = strlen(str);

			if(len != data_size*2+10) {
				xprintf("ihex: Calculated str size %d mismatches received len %d\r\n", data_size*2+10, len);
				goto end_parse;
			}


			uint8_t sum = 0;

			for(int i = 0; i < len - 2; i += 2) 
				sum += strntoul(str+i, 2, 16);

			sum = ~sum + 1; 

			uint8_t his_sum = (uint8_t) strntoul(str+8+data_size*2, 2, 16);

			if(sum != his_sum) {
				xprintf("ihex: Checksum mismatch: sum = %02x, his = %02x\r\n", sum, his_sum);
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

				crc = crc32((const void*)(addr + base), data_size, crc, CRC32_POLYNOMIAL);

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
			if(type == 4) {
				uint32_t tmp = strntoul(str+8, 4, 16) << 16;
			       	if(origin == 0)
					origin = tmp;
				base = tmp - origin;
			}


			// Extended Segment Address
			// The byte count is always 02, the address field (typically 0000) is ignored and the data field
			// contains a 16-bit segment base address. This is multiplied by 16 and added to each subsequent
			// data record address to form the starting address for the data. This allows addressing up to one
			// mebibyte (1048576 bytes) of address space.
			//if(type == 2)
			//	base = strntoul(str+8, 4, 16) << 4;

			#if(DEBUG_CLI>1)
			xprintf("ihex tp: %d, sz: %d, addr: %p\r\n",
					type, data_size, addr + base + offset);
			#endif

			end_parse:
				parse_data_flag = 0;

		} else {
			str[idx++] = c;

			if(idx >= 256*2+10) {
				str[idx] = 0;
				xprintf("Too long: %s\r\n", str);
				parse_data_flag = 0;
			}
		}
	}

	xprintf("ihex: bytes_read = %u, location = %p, crc32 = %p (cksum -o 3)\r\n",
			bytes_read, addr, crc);
	xprintf("ihex: data from file: origin = %p, entry = %p\r\n",
			origin, start32);

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

	xprintf("ohex: addr = %p, len = %u, entry = %p, press Ctrl-C to break.\r\n", addr, len, start32);

	current_address = (uint32_t) addr; // remember last address used

	while(offset < len) {
		uint8_t hex_len = (offset + OHEX_BYTES_PER_LINE <= len) ? 
			OHEX_BYTES_PER_LINE  : (len - offset) % OHEX_BYTES_PER_LINE;
		xprintf(":%02X%04X%02X", hex_len, offset & 0xffff, 0x0);
		uint8_t sum = hex_len + ((offset>>8)&0xff)+(offset&0xff)+0;
		for(int i = 0; i < hex_len; i++) {
			uint8_t byte = *(addr + offset++);
			xprintf("%02X", byte);
			sum += byte;
		}
		sum = ~sum + 1;
		xprintf("%02X\r\n", sum);

		if(console_rx_buf_len) {
			xprintf("ohex: User interrupt\r\n");
			break;
		}

		if((offset & 0xffff) == 0) // 64K overlap ?
			xprintf(":02000004%04X%02X\r\n", (offset >> 16) & 0xffff,
				(~(2+0+0+4+((offset>>24)&0xff)+((offset>>16)&0xff)) + 1) & 0xff);
	}

	if(offset == len) // Successful end ?
		xprintf(":04000005%08X%02X\r\n"
		       ":00000001FF\r\n", start32,
		       (~(4+0+0+5+((start32>>24)&0xff)+((start32>>16)&0xff)+((start32>>8)&0xff)+(start32&0xff)) + 1) & 0xff);

	xprintf("ohex: end\r\n%c", 0x4); // send End-of-Transmission (Ctrl-D) in the end
}

void cli_cmd_stats(char *argv[], int argn) {

	if(argv[1])
		reg_sys_print_stats = strtoul(argv[1], NULL, 0);;

	xprintf("reg_sys_print_stats = %d\r\n", reg_sys_print_stats);
}
	
void cli_cmd_usb(char *argv[], int argn) {

	if(argv[1])
		reg_usb_print_stats = strtoul(argv[1], NULL, 0);;

	xprintf("reg_usb_print_stats = %d\r\n", reg_usb_print_stats);
}


void cli_cmd_nor(char *argv[], int argn) {
	if(argn < 4) {
		show_help(argv, argn);
		return;
	}

	if(argv[1][0] == 'e' && argv[1][1] == 'r')
		cli_cmd_nor_erase(argv, argn);
	else if(argv[1][0] == 'c' && argv[1][1] == 'p')
		cli_cmd_nor_copy(argv, argn);
	else
		show_help(argv, argn);
}

// Process CLI command once Enter is pressed
void cli_process_command(uint8_t *cmdline, uint32_t len) {

	char *cmdsep = " \t";
	char *argv[ARGN_MAX];
	int argn = 0;

	if(cmdline == NULL || len == 0)
		return;

	#if(DEBUG_CLI>1)
	xprintf("\r\nYou entered: %s\r\n\r\n", cmdline);
	#else
	xprintf("\r\n");
	#endif

	memset(argv, 0, sizeof(argv[0]) * ARGN_MAX); // zero argv pointers

	for(char* word = strtok(cmdline, cmdsep);
	    word && argn < ARGN_MAX;
	    word = strtok(NULL, cmdsep)) {
		argv[argn++] = word;
	}

	if(argn == 0) {
		#if(DEBUG_CLI)
		xprintf("argn is 0!\r\n");
		#endif
		return;
	}

	#if(DEBUG_CLI>1)
	for(int i = 0; i < argn; i++) {
		xprintf("ARGV[%u]: %p, ", i, argv[i]);
		xprintf("'%s'\r\n", argv[i]);
	}
	#endif

	// Convert command to lower case
	
	for(int i = 0; argv[0][i]; i++)
		argv[0][i] = tolower(argv[0][i]);

	// Interpret command
	
	for(int i = 0; i < sizeof(command_list)/sizeof(command_list[0]); i++)
		if(command_list[i].cmd && command_list[i].func)
			if(strnstr(command_list[i].cmd, argv[0], 4)) {
				((void (*)(char *argv[], int arg)) command_list[i].func)(argv, argn);
				return;
			}

	if(argv[0][0] != 0)
		xprintf("Unknown command: %s\r\n", argv[0]);
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
				xprintf("\033[2K"); // erase current line
				goto cli_process_input_end;
			}
			if(c == 0x42) {
				cli_history_popdown();
				xprintf("\033[2K"); // erase current line
				goto cli_process_input_end;
			}

			continue;
		}

		switch(c) {
			case 0x07: {
				xprintf("%c", c);
				break;
			}

			case 0x08: {
				if(cli_buf_len == 0)
					break;

				xprintf("%c%c%c", c, 0x20, c);
				cli_buf[--cli_buf_len] = 0;
				break;
			}

			case 0x0a:
			case 0x0d: {
				if(cli_buf_len == 0) {
					cli_history_popup();
					xprintf("\033[2K"); // erase current line
					goto cli_process_input_end;
				}

				char cli_tmp[64+1];
				cli_buf[cli_buf_len] = 0;
				memcpy(cli_tmp, cli_buf, cli_buf_len+1);
				cli_process_command(cli_tmp, cli_buf_len);
				cli_history_push();
				break;
			}

			case 0x1b: {
				esc_flag = 1;
				break;
			}

			case 0x15: { // Ctrl-U
				xprintf("\033[2K"); // erase current line
				cli_buf_len = 0;
				break;
			}

			case 0x0C: { // Ctrl-L
				xprintf("\033[2J"); // erase entire screen 
				xprintf("\033[H"); // cursor to home position 
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
		xprintf("console_rx() buffer overflow, console_rx_buf_len = %d, lost %d bytes\r\n",
			console_rx_buf_len, overflow);

}

