#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "sdmmc.h"
#include "fat.h"

#define	DEBUG_CLI		1		// 0 - off, 1 - few, 2 - more

volatile extern uint32_t console_rx_buf_len;

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

	if(argv[1] && strnstr(argv[1], "mo", 2)) { // mount 
		int dev = 0;
		char *path = "";

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

	if(argv[1] && strnstr(argv[1], "um", 2)) { // umount 
		char *path = "";

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

	if(argv[1] && strnstr(argv[1], "ty", 2)) { // type 
		char *path = argv[2];

		#if(DEBUG_CLI)
		xprintf("%s: typing file %s\r\n", "fat32", path);
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

	show_help(argv, argn);
}

/*
    err = fat_file_open(&file, "/mnt/source/fat.c", FAT_READ);
    CHECK_ERROR(err);

    for (;;)
    {
      err = fat_file_read(&file, buf, 512, &cnt);
      CHECK_ERROR(err);

      printf("%.*s", cnt, buf);
      if (cnt != 512)
        break;
    }

    err = fat_file_close(&file);
    CHECK_ERROR(err);

*/
