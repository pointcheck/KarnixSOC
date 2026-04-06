#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "sdmmc.h"
#include "fat.h"
#include "elf.h"

#define	DEBUG_CLI		1		// 0 - off, 1 - few, 2 - more
#ifndef FAT32_DEFAULT_SDMMC_DEVICE
#define	FAT32_DEFAULT_SDMMC_DEVICE	1	// default sdmmc device number
#endif

volatile extern uint32_t console_rx_buf_len;
extern const char* months[];
extern uint32_t current_address;

static bool disk_read(uint32_t dev, uint8_t* buf, uint32_t sect) {
	return sdmmc_read_block(dev, sect, 1, buf) == 0;
}


static bool disk_write(uint32_t dev, const uint8_t* buf, uint32_t sect) {
	return sdmmc_write_block(dev, sect, 1, buf) == 0;
}

static DiskOps fat_ops =
{
	.read  = disk_read,
	.write = disk_write,
};


void show_help();

void cli_cmd_fat32(char *argv[], int argn) {
	int ret = 0;

	if(argv[1] && strnstr(argv[1], "mount", 5)) { // mount point 
		int dev = FAT32_DEFAULT_SDMMC_DEVICE;
		char *path = "mnt";

		if(argv[2])
			dev = strtoul(argv[2], NULL, 0);

		if(argv[3])
			path = argv[3];

		#if(DEBUG_CLI)
		xprintf("%s: mounting device %d as volume '%s'\r\n", "fat32", dev, path);
		#endif

		if(dev >= SDMMC_DEVICES || dev < 0) {
			xprintf("%s: no such SD/MMC device %d\r\n", "fat32", dev);
			return;
		}

		if((ret = sdmmc_init(dev)) < 0) {
			xprintf("%s: failed to init SD/MMC device %d, ret = %d\r\n",
				"fat32", dev, ret);
			return;
		}

		Fat *fat = (Fat*) malloc(sizeof(Fat));

		if(fat == NULL) {
			xprintf("%s: failed to allocate mem for FAT table\r\n", "fat32");
			return;
		}

  		if((ret = fat_mount(&fat_ops, dev, 0, fat, path)) != 0) {
			xprintf("%s: failed to mount FAT on device %d, ret = %d (%s)\r\n",
				"fat32", dev, ret, fat_get_error(ret));
			free(fat);
			return;
		}

		return;
	}

	if(argv[1] && strnstr(argv[1], "umount", 6)) { // unmount point 
		char *path = "mnt";

		if(argv[2])
			path = argv[2];

		#if(DEBUG_CLI)
		xprintf("%s: unmounting volume '%s'\r\n", "fat32", path);
		#endif

		Fat* fat = find_fat_volume(path, strlen(path));

		if(fat == NULL) {
			xprintf("%s: volume '%s' was not mounted\r\n", "fat32", path);
			return;
		}

		fat_umount(fat);

		free(fat);

		return;
	}

	if(argv[1] && strnstr(argv[1], "mkdir", 5)) { // create directory 
		char *path = argv[2];

		if(!path) {
			xprintf("%s: provide new dir full /path/name\r\n", "fat32");
			return;
		}

		#if(DEBUG_CLI)
		xprintf("%s: creating directory %s\r\n", "fat32", path);
		#endif

		Dir dir;

		int err = fat_dir_create(&dir, path);

		if(err)
			xprintf("%s: failed to create dir %s: %s\r\n", "fat32", path, fat_get_error(err));

		return;
	}

	if(argv[1] && strnstr(argv[1], "rm", 5)) { // unlink file or directory 
		char *path = argv[2];

		if(!path) {
			xprintf("%s: provide /path/name to remove\r\n", "fat32");
			return;
		}

		#if(DEBUG_CLI)
		xprintf("%s: unlinking %s\r\n", "fat32", path);
		#endif

		int err = fat_unlink(path);

		if(err)
			xprintf("%s: failed unlinking %s: %s\r\n", "fat32", path, fat_get_error(err));
		#if(DEBUG_CLI)
		else
			xprintf("%s: unlinked path %s\r\n", "fat32", path);
		#endif

		return;
	}


	if(argv[1] && strnstr(argv[1], "ls", 2)) { // list directory 
		char *path = "/mnt";

		if(argv[2])
			path = argv[2];

		#if(DEBUG_CLI)
		xprintf("%s: listing files in %s\r\n", "fat32", path);
		#endif

		int file_count = 0;

		Dir dir;
		DirInfo info;

		int err = fat_dir_open(&dir, path);

		if(err) {
			if(err == FAT_ERR_EOF)
				xprintf("%s: not found dir %s\r\n", "fat32", path);
			else
				xprintf("%s: cannot open dir %s: %s\r\n", "fat32", path, fat_get_error(err));
			return;
		}

		while(1) {
			err = fat_dir_read(&dir, &info);

			if(err == FAT_ERR_EOF)
				break;

			if(err) {
				xprintf("%s: error reading dir %s: %s\r\n", "fat32", path, fat_get_error(err));
				return;
			}

			xprintf("%5d   %s %02d   %02d:%02d   %.*s%c\r\n",
				info.size, months[info.modified.month - 1], info.modified.day,
				info.modified.hour, info.modified.min, 
				info.name_len, info.name, info.attr & FAT_ATTR_DIR ? '/' : ' ');

			file_count++;

			err = fat_dir_next(&dir);
		}

		xprintf("%s: total %d files in %s\r\n", "fat32", file_count, path);

		return;
	}

	if(argv[1] && strnstr(argv[1], "cat", 3)) { // typing ASCII file to console
		char *path = argv[2];

		#if(DEBUG_CLI)
		xprintf("%s: catting file %s\r\n", "fat32", path);
		#endif

		File file;
		int cnt;

 		if((ret = fat_file_open(&file, path, FAT_READ)) != 0) {
			xprintf("%s: failed to open %s for read, ret = %d (%s)\r\n",
				"fat32", path, ret, fat_get_error(ret));
			return;
		}

		char buf[512+2]; // 2 extra bytes for CRC16
		int total_read = 0;

		while(1) {

			if((ret = fat_file_read(&file, buf, 512, &cnt)) != 0) {
				xprintf("%s: failed to read from %s, ret = %d (%s)\r\n",
					"fat32", path, ret, fat_get_error(ret));
				return;
			}

			total_read += cnt;

			buf[cnt] = 0;

			char *b = buf;

			// Convert \n to \r\n
			while(1) {
				char *sub = strchr(b, '\n');
				if(sub) {
					*sub = 0;
					xprintf("%s\r\n", b);
					b = sub+1;
				} else {
					xprintf("%s", b);
					break;
				}
			}

			if(cnt != 512)
				break;

			if(console_rx_buf_len)
				break;
		}

		#if(DEBUG_CLI)
		xprintf("\r\n%s: %d bytes read from %s\r\n", "fat32", total_read, path);
		#endif
			
		if((ret = fat_file_close(&file)) != 0) {
			xprintf("%s: failed to close %s, ret = %d (%s)\r\n",
				"fat32", path, ret, fat_get_error(ret));
			return;
		}

		return;
	}

	if(argv[1] && strnstr(argv[1], "load", 4)) { // load binary file to mem 
		char *addr = (char*) current_address;
		char *path = "";

		if(argv[2]) {
			if(argv[2][0] != '*')
				addr = (char*) strtoul(argv[2], NULL, 0);
		}

		if(argv[3])
			path = argv[3];

		#if(DEBUG_CLI)
		xprintf("%s: loading file %s at 0x%08X\r\n", "fat32", path, addr);
		#endif

		File file;
		int cnt;

 		if((ret = fat_file_open(&file, path, FAT_READ)) != 0) {
			xprintf("%s: failed to open %s for read, ret = %d (%s)\r\n",
				"fat32", path, ret, fat_get_error(ret));
			return;
		}

		char buf[512+2]; // 2 extra bytes for CRC16
		char *b = addr;
		int total_read = 0;

		while(1) {

			if((ret = fat_file_read(&file, buf, 512, &cnt)) != 0) {
				xprintf("%s: failed to read from %s, ret = %d (%s)\r\n",
					"fat32", path, ret, fat_get_error(ret));
				return;
			}

			total_read += cnt;
		
			memcpy(b, buf, cnt);

			b += cnt;

			xprintf(".");

			if(cnt != 512)
				break;

			if(console_rx_buf_len)
				break;
		}

		xprintf("\r\n");

		#if(DEBUG_CLI)
		xprintf("\r\n%s: %d bytes read from %s to 0x%08X\r\n", "fat32", total_read, path, addr);
		#endif
			
		if((ret = fat_file_close(&file)) != 0) {
			xprintf("%s: failed to close %s, ret = %d (%s)\r\n",
				"fat32", path, ret, fat_get_error(ret));
			return;
		}

		current_address = (uint32_t) addr; // remember last address used

		return;
	}

	if(argv[1] && strnstr(argv[1], "save", 4)) { // save mem block to binary file 
		char *addr = (char*) current_address;
		int size = 512; 
		char *path = "/mnt/dump.bin";

		if(argv[2]) {
			if(argv[2][0] != '*')
				addr = (char*) strtoul(argv[2], NULL, 0);
		}

		if(argv[3])
			size = strtoul(argv[3], NULL, 0);

		if(argv[4])
			path = argv[4];

		#if(DEBUG_CLI)
		xprintf("%s: saving %d bytes at 0x%08X to file %s\r\n", "fat32", size, addr, path);
		#endif

		File file;
		int cnt;

 		if((ret = fat_file_open(&file, path, FAT_WRITE | FAT_CREATE | FAT_TRUNC)) != 0) {
			xprintf("%s: failed to open %s for write, ret = %d (%s)\r\n",
				"fat32", path, ret, fat_get_error(ret));
			return;
		}

		char buf[512];
		char *b = addr;
		int total_written = 0;

		while(total_written < size) {

			int cnt_wr;

			int cnt = size - total_written;

			if(cnt > 512)
				cnt = 512;

			if((ret = fat_file_write(&file, b, cnt, &cnt_wr))) {
				xprintf("%s: failed to write to %s, ret = %d (%s)\r\n",
					"fat32", path, ret, fat_get_error(ret));
				return;
			}

			total_written += cnt_wr;
		
			b += cnt_wr;

			xprintf(".");

			if(console_rx_buf_len)
				break;
		}

		xprintf("\r\n");

		#if(DEBUG_CLI)
		xprintf("\r\n%s: %d bytes written from 0x%08X to %s\r\n", "fat32", total_written, addr, path);
		#endif
			
		if((ret = fat_file_close(&file)) != 0) {
			xprintf("%s: failed to close %s, ret = %d (%s)\r\n",
				"fat32", path, ret, fat_get_error(ret));
			return;
		}

		current_address = (uint32_t) addr; // remember last address used

		return;
	}

	if(argv[1] && strnstr(argv[1], "cp", 2)) { // copy a file

		if(!argv[2]) {
			xprintf("%s: provide /path/src file to copy from\r\n", "fat32");
			return;
		}

		char *path_src = argv[2];

		if(!argv[3]) {
			xprintf("%s: provide /path/dst file to copy to\r\n", "fat32");
			return;
		}

		char *path_dst = argv[3];

		#if(DEBUG_CLI)
		xprintf("%s: copying file from %s to %s\r\n", "fat32", path_src, path_dst);
		#endif

		File file_src, file_dst;
		DirInfo info;

		if((ret = fat_stat(path_src, &info))) {
			xprintf("%s: failed stat %s, ret = %d (%s)\r\n",
				"fat32", path_src, ret, fat_get_error(ret));
			return;
		}

		if(info.attr & FAT_ATTR_DIR) {
			xprintf("%s: cannot copy a directory %s\r\n",
				"fat32", path_src, ret, fat_get_error(ret));
			return;
		}

		if((ret = fat_stat(path_dst, &info)) && ret != FAT_ERR_EOF) {
			xprintf("%s: failed stat %s, ret = %d (%s)\r\n",
				"fat32", path_dst, ret, fat_get_error(ret));
			return;
		}

		if((ret != FAT_ERR_EOF) && (info.attr & FAT_ATTR_DIR)) {
			xprintf("%s: cannot copy to a directory %s, provide file name!\r\n",
				"fat32", path_src, ret, fat_get_error(ret));
			return;
		}

		int cnt, total = 0;
		char buf[512+2]; // 2 extra bytes for CRC16


 		if((ret = fat_file_open(&file_src, path_src, FAT_READ)) != 0) {
			xprintf("%s: failed to open %s for read, ret = %d (%s)\r\n",
				"fat32", path_src, ret, fat_get_error(ret));
			return;
		}

 		if((ret = fat_file_open(&file_dst, path_dst, FAT_WRITE | FAT_CREATE | FAT_TRUNC)) != 0) {
			xprintf("%s: failed to open %s for write, ret = %d (%s)\r\n",
				"fat32", path_dst, ret, fat_get_error(ret));
			fat_file_close(&file_src);
			return;
		}


		while(1) {

			if((ret = fat_file_read(&file_src, buf, 512, &cnt)) != 0) {
				xprintf("%s: failed to read from %s, ret = %d (%s)\r\n",
					"fat32", path_src, ret, fat_get_error(ret));
				goto end_copy;
			}

			int cnt_wr;
			char *b = buf;
			int eof = (cnt != 512);

			while(cnt > 0) {
	
				if((ret = fat_file_write(&file_dst, b, cnt, &cnt_wr))) {
					xprintf("%s: failed to write to %s, ret = %d (%s)\r\n",
						"fat32", path_dst, ret, fat_get_error(ret));
					goto end_copy;
				}

				cnt -= cnt_wr;
				total += cnt_wr;
				b += cnt_wr;

				xprintf(".");
			}

			if(eof) {
				xprintf("\r\n");
				break;
			}

			if(console_rx_buf_len)
				break;
		}

		end_copy:

		fat_file_close(&file_dst);
		fat_file_close(&file_src);
		fat_sync(file_dst.fat);

		#if(DEBUG_CLI)
		xprintf("%s: %d bytes copied from %s to %s\r\n", "fat32", total, path_src, path_dst);
		#endif

		return;
	}

	if(argv[1] && strnstr(argv[1], "dump", 4)) { // hex dump a file

		if(!argv[2]) {
			xprintf("%s: provide /path/file to HEX dump\r\n", "fat32");
			return;
		}

		char *path = argv[2];

		#if(DEBUG_CLI)
		xprintf("%s: HEX dumping file %s\r\n", "fat32", path);
		#endif

		File file;
		int cnt;

 		if((ret = fat_file_open(&file, path, FAT_READ)) != 0) {
			xprintf("%s: failed to open %s for read, ret = %d (%s)\r\n",
				"fat32", path, ret, fat_get_error(ret));
			return;
		}

		char buf[512+2]; // 2 extra bytes for CRC16
		char str[128]; // string buffer for HEX dump

		int total_read = 0;
		int dump_count = 0;

		while(1) {

			if((ret = fat_file_read(&file, buf, 512, &cnt)) != 0) {
				xprintf("%s: failed to read from %s, ret = %d (%s)\r\n",
					"fat32", path, ret, fat_get_error(ret));
				return;
			}

			total_read += cnt;
			char *b = buf;

			while(dump_count < total_read) {
				xprintf("0x%08X: ", dump_count);
				str[0] = 0;
				for(int i = 0; i < 16 && dump_count < total_read; i++) {
					sprintf(str+i, "%c", ((*b > 0x20) && (*b < 0x7f)) ? *b : '.');
		       			xprintf("%02X ", *b++);
					dump_count++;
				}

				xprintf("| %s\r\n", str);

				if(console_rx_buf_len)
					goto end_dump;
			}

			if(cnt != 512)
				break;

			if(console_rx_buf_len)
				break;
		}

		end_dump:

		#if(DEBUG_CLI)
		xprintf("\r\n%s: %d bytes dumped from %s\r\n", "fat32", dump_count, path);
		#endif
			
		if((ret = fat_file_close(&file)) != 0) {
			xprintf("%s: failed to close %s, ret = %d (%s)\r\n",
				"fat32", path, ret, fat_get_error(ret));
			return;
		}

		return;
	}

	if(argv[1] && strnstr(argv[1], "exec", 4)) { // execute binary ELF file
		char *path = "";

		if(argv[2]) {
			path = argv[2];
		} else {
			xprintf("%s: provide ELF file name to execute\r\n", "fat32", path);
			return;
		}

		#if(DEBUG_CLI)
		xprintf("%s: preparing for execution file %s\r\n", "fat32", path);
		#endif

		File file;
		int cnt;

 		if((ret = fat_file_open(&file, path, FAT_READ)) != 0) {
			xprintf("%s: failed to open %s for read, ret = %d (%s)\r\n",
				"fat32", path, ret, fat_get_error(ret));
			return;
		}


		Elf32_Ehdr elf;

		if((ret = fat_file_read(&file, (char*)&elf, sizeof(elf), &cnt)) != 0) {
			xprintf("%s: failed to read from %s, ret = %d (%s)\r\n",
				"fat32", path, ret, fat_get_error(ret));
			return;
		}

		if(cnt != sizeof(elf)) {
			xprintf("%s: partial to read from %s, %d != %d\r\n",
				"fat32", path, cnt, sizeof(elf));
			goto exec_end;
		}

		#if(DEBUG_CLI)
		xprintf("%s: Magic: 0x%02X 0x%02X 0x%02X 0x%02X\r\n", "elf32",
			elf.e_ident[EI_MAG0], elf.e_ident[EI_MAG1], elf.e_ident[EI_MAG2], elf.e_ident[EI_MAG3]);
		xprintf("%s: Class: %s, Endian: %s, Version: %d, OS/ABI: %d, ISA: 0x%02X, Obj: %d\r\n",
			"elf32",
			elf.e_ident[EI_CLASS] == 1 ? "32-bit" : "64-bit",
			elf.e_ident[EI_DATA] == 1 ? "little" : "big",
			elf.e_ident[EI_VERSION],
			elf.e_ident[EI_OSABI],
			elf.e_machine, elf.e_type
			);
		xprintf("%s: Entry point: 0x%08X\r\n", "elf32", elf.e_entry);
		xprintf("%s: Section header: e_shoff = %d, e_shentsize = %d, e_shnum = %d\r\n", "elf32",
			elf.e_shoff, elf.e_shentsize, elf.e_shnum);
		#endif

		if(*(uint32_t*)elf.e_ident != 0x464c457f) {
			xprintf("%s: File is not en ELF!\r\n", "elf32");
			goto exec_end;
		}

		if(elf.e_ident[EI_CLASS] != 1) {
			xprintf("%s: ELF is not a 32-bit class!\r\n", "elf32");
			goto exec_end;
		}

		if(elf.e_ident[EI_OSABI] != ELFOSABI_SYSV) {
			xprintf("%s: OS/ABI is not SystemV!\r\n", "elf32");
			goto exec_end;
		}

		if(elf.e_machine != EM_RISCV) {
			xprintf("%s: Machine is not RISC-V!\r\n", "elf32");
			goto exec_end;
		}

		// Seek to Section Header and load it into shbuf

		char *shbuf = malloc(elf.e_shnum * elf.e_shentsize);

		if(shbuf == NULL) {
			xprintf("%s: failed to malloc %d bytes\r\n",
				"elf32", elf.e_shnum * elf.e_shentsize);
			goto exec_end;
		}

		if((ret = fat_file_seek(&file, elf.e_shoff, SEEK_SET)) != 0) {
			xprintf("%s: failed to seek to %d, ret = %d (%s)\r\n",
				"elf32", elf.e_shoff, ret, fat_get_error(ret));
			goto free_exec_end;
		}
		
		if((ret = fat_file_read(&file, (char*)shbuf, elf.e_shnum * elf.e_shentsize, &cnt)) != 0) {
			xprintf("%s: failed to read from %s, ret = %d (%s)\r\n",
				"elf32", path, ret, fat_get_error(ret));
			goto free_exec_end;
		}

		if(cnt != elf.e_shnum * elf.e_shentsize) { // less than one entry in section header ?
			xprintf("%s: failed to read section header, %d != %d\r\n",
				"elf32", cnt, elf.e_shnum * elf.e_shentsize);
			goto free_exec_end;
		}

		
		// Iterate through sections, load prog and data, zero BSS and stack. 

		xprintf("%s: SECT:      Type:    Offset:      Addr:      Size:     Flags:  Action:\r\n", "elf32");

		for(int sect = 0; sect < elf.e_shnum; sect++) {

			Elf32_Shdr *secth = ((Elf32_Shdr *)shbuf)+sect;

			xprintf("%s:  %02d   0x%08X 0x%08X 0x%08X 0x%08X 0x%08X ",
				"elf32", sect, secth->sh_type, secth->sh_offset, secth->sh_addr,
				secth->sh_size, secth->sh_flags);

			if(secth->sh_type == SHT_PROGBITS && secth->sh_addr != 0 && secth->sh_size != 0) {

				if(check_ram_regions(secth->sh_addr, secth->sh_size) != 0) {
					xprintf("Failed!\r\n");
					xprintf("%s: Section %d cannot be loaded into available RAM!\r\n",
						"elf32", sect);
					goto free_exec_end;
				}

				xprintf("Loading\r\n");

				if((ret = fat_file_seek(&file, secth->sh_offset, SEEK_SET)) != 0) {
					xprintf("%s: failed to seek to SECT[%d], offset = %d, ret = %d (%s)\r\n",
						"elf32", sect, secth->sh_offset, ret, fat_get_error(ret));
					goto free_exec_end;
				}
		
				if((ret = fat_file_read(&file, (char*)secth->sh_addr, secth->sh_size, &cnt)) != 0) {
					xprintf("%s: failed to read SECT[%d] from %s, ret = %d (%s)\r\n",
						"elf32", sect, path, ret, fat_get_error(ret));
					goto free_exec_end;
				}

				if(cnt != secth->sh_size) {
					xprintf("%s: partial read of SECT[%d], %d != %d\r\n",
						"elf32", sect, cnt, secth->sh_size);
					goto free_exec_end;
				}


			} else if(secth->sh_type == SHT_NOBITS && secth->sh_addr != 0 && secth->sh_size != 0) {
				xprintf("Zeroing\r\n");
				memset((void*)secth->sh_addr, secth->sh_size, 0);
			} else {
				xprintf("Skipping\r\n");
			}
		}

		#if(DEBUG_CLI)
		xprintf("%s: binary loaded OK\r\n", "elf32");
		#endif

		current_address = (uint32_t) elf.e_entry; // remember last address used

		free_exec_end:

			free(shbuf);

		exec_end:

			if((ret = fat_file_close(&file)) != 0) {
				xprintf("%s: failed to close %s, ret = %d (%s)\r\n",
					"fat32", path, ret, fat_get_error(ret));
				return;
			}

		return;
	}

	show_help(argv, argn);
}

