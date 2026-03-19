#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "sdmmc.h"
#include "fat.h"

#define	DEBUG_CLI		1		// 0 - off, 1 - few, 2 - more
#ifndef FAT32_DEFAULT_SDMMC_DEVICE
#define	FAT32_DEFAULT_SDMMC_DEVICE	1	// default sdmmc device number
#endif

volatile extern uint32_t console_rx_buf_len;
extern const char* months[];


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
			xprintf("%s: not enough mem for FAT table\r\n", "fat32");
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
			}



			if(eof)
				break;

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

	show_help(argv, argn);
}

