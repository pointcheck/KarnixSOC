#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soc.h"
#include "utils.h"
#include "env.h"
#include "qspi.h"

volatile extern uint32_t console_rx_buf_len;

void show_help();

void cli_cmd_env(char *argv[], int argn) {

	if(argv[1] && strnstr(argv[1], "print", 5)) { // print environment variable

		unsigned char *p = (unsigned char *)ENV_MEM_ADDRESS;
		int env_count = 0;

		while(1) {

			if(p >= (unsigned char *)ENV_MEM_ADDRESS + ENV_MEM_SIZE)
				break;

			int var_name_len = p[0];
			int var_value_len = p[1];

			if(var_name_len) {
				char *var_name = p + 2;
				char *var_value = var_name + var_name_len;

				if(var_name_len == 255 || var_name_len == 255) // erased flash block ?
					break;

				#if(DEBUG_CLI)
				if(var_name >= (char *)ENV_MEM_ADDRESS + ENV_MEM_SIZE) {
					xprintf("%s: %s = 0x%08x points outsize of ENV_MEM region (%d@0x%08x)\r\n",
						"env", "var_name", var_name, ENV_MEM_SIZE, ENV_MEM_ADDRESS);
					break;
				}
				#endif

				#if(DEBUG_CLI)
				if(var_value >= (char *)ENV_MEM_ADDRESS + ENV_MEM_SIZE) {
					xprintf("%s: %s = 0x%08x points outsize of ENV_MEM region (%d@0x%08x)\r\n",
						"env", "var_value", var_value, ENV_MEM_SIZE, ENV_MEM_ADDRESS);
					break;
				}
				#endif

				xprintf("ENV: %s = %s\r\n", var_name, var_value);

				p = (unsigned char*)var_value + var_value_len; 
			}
		}

		xprintf("%s: %d env variables found\r\n", "env", env_count);

		return;
	} 


	if(argv[1] && strnstr(argv[1], "erase", 5)) { // erase NOR sectors containing env vars
		uint32_t addr = ENV_MEM_ADDRESS;

	        uint32_t t0 = get_mtime();

		for(int i = 0; i < ENV_MEM_SIZE; i += 4096) {
			xprintf("%s: erasing %p\r", "env", (addr + i));
			fflush(stdout);
	
	                qspi_erase_sector((uint32_t)(addr+i));
	
	                while(qspi_get_status() & QSPI_DEVICE_STATUS_BUSY);
	
			if(console_rx_buf_len)
				break;
		}
	
	        uint32_t t1= get_mtime();
	
	        xprintf("%s: env erase completed in %u uS, qspi status = %p\r\n", "env", t1-t0, qspi_get_status());

		return;
	}

	show_help(argv, argn);
}

