#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "soc.h"
#include "riscv.h"
#include "plic.h"
#include "i2c.h"
#include "uart.h"
#include "utils.h"
#include "eeprom.h"
#include "cga.h"
#include "tetris.h"
#include "config.h"

/* Below is some linker specific stuff */
extern unsigned int   _stack_start; /* Set by linker.  */
extern unsigned int   _stack_size; /* Set by linker.  */
extern unsigned int   trap_entry;
extern unsigned int   heap_start; /* programmer defined heap start */
extern unsigned int   heap_end; /* programmer defined heap end */

#define SRAM_SIZE       (512*1024)
#define SRAM_ADDR_BEGIN 0x90000000
#define SRAM_ADDR_END   (0x90000000 + SRAM_SIZE)

void println(const char*str);

volatile uint32_t uart_config_reset_counter = 0;
volatile uint32_t reg_irq_counter = 0;
volatile uint32_t reg_sys_counter = 0;

volatile uint32_t audiodac0_irqs = 0;
volatile uint32_t audiodac0_samples_sent = 0;
volatile uint32_t cga_vblank_irqs = 0;

uint32_t deadbeef = 0;		// If not zero - we are in soft-start mode

void println(const char*str){
	print_uart0(str);
	print_uart0("\r\n");
}

char to_hex_nibble(char n)
{
	n &= 0x0f;

	if(n > 0x09)
		return n + 'A' - 0x0A;
	else
		return n + '0';
}

void to_hex(char*s , unsigned int n)
{
	for(int i = 0; i < 8; i++) {
		s[i] = to_hex_nibble(n >> (28 - i*4));
	}
	s[8] = 0;
}

static unsigned char *videobuf_for_text = (unsigned char*)0x90001000;
static unsigned char *videobuf_for_graphics = (unsigned char*)0x90006000;

float my_sine(float x)
{
	
	x = ((int)(x * 57.297469) % 360) * 0.017452; // fmod(x, 2*PI)

	float res = 0, pow = x, fact = 1;

	for(int i = 0; i < 5; ++i) {
		res+=pow/fact;
		pow*=-1*x*x;
		fact*=(2*(i+1))*(2*(i+1)+1);
	}
	return res;
}

#include "sprites.h"
int _sprite_idx;

void show_greetings(void) {

	char *greetings = 
		ESC_FG "15;\t######  ######  ######  " ESC_FG "5;#####    ####   ####    ####         ##  ##\r\n"
		ESC_FG "15;\t  ##    ##        ##    " ESC_FG "5;##  ##    ##   ##      ##  ##        ##  ##\r\n"
		ESC_FG "15;\t  ##    ####      ##    " ESC_FG "5;#####     ##    ####   ##      ####  ##  ##\r\n"
		ESC_FG "15;\t  ##    ##        ##    " ESC_FG "5;##   #    ##       ##  ##  ##         ####\r\n"
		ESC_FG "15;\t  ##    ######    ##    " ESC_FG "5;##   ##  ####  #####    ####           ##\r\n"
		"\r\n\r\n"
		ESC_FG "12;"
		"\t   T O   S T A R T   G A M E   P R E S S   A N Y   B U T T O N\r\n"
		"\n"
		"\n"
		ESC_FG "5;"
		"\t\tGreetings to all users of Habr!\r\n"
		"\n"
		"\tHope you  enjoyed  reading  my post on  developing  CGA-like  video\r\n"
		"\tsubsystem for  FPGA  based synthesizable microcontroller  and liked\r\n"
		"\tthe VFXs demoed.\r\n"
		"\n"
		"\tThis work was inspired by those few who proceed coding for resource\r\n"
		"\tscarse systems  gaining  impossible  from them  using minuscule yet\r\n"
		"\tpowerful handcrafted tools.\r\n"
		"\n"
		"\t\tKudos to you dear old-time hacker fellows!\r\n"
		"\n"
		ESC_FG "14;"
		"\t\tBy @checkpoint <rz@fabmicro.ru>\r\n"
		"\n"
		"\n"
		ESC_FG "5;"
		"\t\tCredits to:\r\n"
		"\t\t===========\r\n"
		"\t" ESC_FG "14;" "Yuri Panchul" ESC_FG "5; for his work on basics-graphics-music Verilog labs\r\n"
		"\t" ESC_FG "14;" "Dmitry Petrenko" ESC_FG "5; for inspirations and ideas for Karnix FPGA board\r\n"
		"\t" ESC_FG "14;" "Vitaly Maltsev" ESC_FG "5; for helping with bitblit code optimization\r\n"
		"\t" ESC_FG "14;" "Victor Sergeev" ESC_FG "5; for demoscene inpirations\r\n"
		"\t" ESC_FG "14;" "Evgeny Korolenko" ESC_FG "5; for editing and testing\r\n"
		"\t" ESC_FG "14;" "svedev" ESC_FG "5; for tetris-c\r\n"
		"\n"
		"\t\tGreetings:\r\n"
		"\t\t==========\r\n"
		"\n"
		"\t" ESC_FG "14;" "@Manwe_Sand" ESC_FG "5; for his Good Apple for BK-0011M and other BK-0010 stuff\r\n"
		"\t" ESC_FG "14;" "@shiru8bit" ESC_FG "5; for is posts on old hardware and retro-gaming\r\n"
		"\t" ESC_FG "14;" "@undex" ESC_FG "5; for work on Far2l\r\n"
		"\t" ESC_FG "14;" "@@alex0x08" ESC_FG "5; for devotion to FreeBSD and posting about it\r\n"
		"\t" ESC_FG "14;" "@KeisN13" ESC_FG "5; for organizing FPGA community in Russia\r\n"
		"\t" ESC_FG "14;" "@frog" ESC_FG "5; for CC'24 and keeping Russian demoscene running...\r\n"
		"\n"
		"\t\t*\t*\t*\n\n\n\n";
	       
	memset(videobuf_for_text, 0, 20*1024);
	cga_text_print(videobuf_for_text, 0,  0, 15, 0, greetings);
	cga_set_cursor_xy(48, 22);

	memset(videobuf_for_graphics, 0, 20*1024);

	for(int y = 0; y < 240; y += 48)
		for(int x = 0; x < 320; x += 48)
			cga_bitblit((uint8_t*)sprites[(_sprite_idx+y) % 11], videobuf_for_graphics,
				x + 64 * my_sine((x+_sprite_idx)/80.0),
				y + 64 * my_sine((x+_sprite_idx)/80.0),
				16, 16, CGA_VIDEO_WIDTH, CGA_VIDEO_HEIGHT);

	_sprite_idx++;
}

int sram_test_write_random_ints(int interations) {
	volatile unsigned int *mem;
	unsigned int fill;
	int fails = 0;

	for(int i = 0; i < interations; i++) {
		fill = 0xdeadbeef + i;
		mem = (unsigned int*) SRAM_ADDR_BEGIN;

		printf("Filling SRAM at: %p, size: %d bytes...\r\n", mem, SRAM_SIZE);

		while((unsigned int)mem < SRAM_ADDR_END) {
			*mem++ = fill;
			fill += 0xdeadbeef; // generate pseudo-random data
		}

		fill = 0xdeadbeef + i;
		mem = (unsigned int*) SRAM_ADDR_BEGIN;

		printf("Checking SRAM at: %p, size: %d bytes...\r\n", mem, SRAM_SIZE);

		while((unsigned int)mem < SRAM_ADDR_END) {
			unsigned int tmp = *mem;
			if(tmp != fill) {
				printf("SRAM check failed at: %p, expected: %p, got: %p\r\n", mem, fill, tmp);
				fails++;
			} else {
				//printf("\r\nMem check OK     at: %p, expected: %p, got: %p\r\n", mem, fill, *mem);
			}
			mem++;
			fill += 0xdeadbeef; // generate pseudo-random data
			break;
		}

		if((unsigned int)mem == SRAM_ADDR_END)
			printf("SRAM Fails: %d\r\n", fails);

		if(fails)
			break;
	}

	return fails++;
}

void main() {

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init

	init_sbrk(NULL, 0); // Initialize heap for malloc to use on-chip RAM

	delay_us(2000000); // Allow user to connect to debug uart

	if(deadbeef != 0) {
		print("Soft-start, performing hard reset!\r\n");
		hard_reboot();
	} else {
		deadbeef = 0xdeadbeef;
	}

	printf("\r\n"
		"Karnix ASB-254. Tetris. Build %05d, date/time: " __DATE__ " " __TIME__ "\r\n"
		"Copyright (C) 2024 Fabmicro, LLC., Tyumen, Russia.\r\n\r\n",
		BUILD_NUMBER
	);

	GPIO->OUTPUT |= GPIO_OUT_LED0; // LED0 is ON - indicate we are not yet ready

	printf("Hardware init\r\n");

	// Test SRAM and initialize heap for malloc to use SRAM if tested OK
	if(sram_test_write_random_ints(10) == 0) {
		printf("Enabling SRAM...\r\n");
		init_sbrk((unsigned int*)SRAM_ADDR_BEGIN, SRAM_SIZE);
		printf("SRAM %s!\r\n", "enabled"); 
		// If this prints, we are running with new heap all right
		// Note, that some garbage can be printed along, that's ok!

		//test_byte_access();
	} else {
		printf("SRAM %s!\r\n", "disabled"); 
	}

	// Init CGA: enable graphics mode and load color palette
	cga_set_video_mode(CGA_MODE_GRAPHICS1);

	static uint32_t rgb_palette[16] = {
			0x00000000, 0x000000f0, 0x0000f000, 0x00f00000,
			0x0000f0f0, 0x00f000f0, 0x00f0f000, 0x00f0f0f0,
			0x000f0f0f, 0x000f0fff, 0x000fff0f, 0x00ff0f0f,
			0x000fffff, 0x00ff0fff, 0x00ffff0f, 0x00ffffff,
			};
	cga_set_palette(rgb_palette);

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init 

	// Enable writes to EEPROM
	GPIO->OUTPUT &= ~GPIO_OUT_EEPROM_WP; 

	// Configure interrupt controller 
	PLIC->POLARITY = 0x0000007f; // Configure PLIC IRQ polarity for UART0, UART1, MAC, I2C and AUDIODAC0 to Active High 
	PLIC->PENDING = 0; // Clear all pending IRQs
	PLIC->ENABLE = 0xffffffff; // Configure PLIC to enable interrupts from all possible IRQ lines

	// Configure UART0 IRQ sources: bit(0) - TX interrupts, bit(1) - RX interrupts 
	UART0->STATUS = (UART0->STATUS | UART_STATUS_RX_IRQ_EN); // Allow only RX interrupts 

	// Configure UART1 IRQ sources: bit(0) - TX interrupts, bit(1) - RX interrupts 
	UART1->STATUS = (UART1->STATUS | UART_STATUS_RX_IRQ_EN); // Allow only RX interrupts 

	// Configure RISC-V IRQ stuff
	//csr_write(mtvec, ((unsigned long)&trap_entry)); // Set IRQ handling vector

	// Setup TIMER0 to 100 ms timer for Mac: 25 MHz / 25 / 10000
	timer_prescaler(TIMER0, SYSTEM_CLOCK_HZ / 1000000);
	timer_run(TIMER0, 100000);

	// Setup TIMER0 to 50 ms timer for Modbus: 25 MHz / 25 / 10000
	timer_prescaler(TIMER1, SYSTEM_CLOCK_HZ / 1000000);
	timer_run(TIMER1, 50000);


	// I2C and EEPROM
	i2c_init(I2C0);
	

	// Reset to Factory Defaults if "***" sequence received from UART0

	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts during user input 

	fpurge(stdout);
	printf("Press '*' to reset config");
	fflush(stdout);

	for(int i = 0; i < 5; i++) {
		if(uart_config_reset_counter > 2) // has user typed *** on the console ?
			break;

		if((GPIO->INPUT & GPIO_IN_CONFIG_PIN) == 0) // is CONFIG pin tied to ground ?
			break;

		putchar('.');
		fflush(stdout);
		delay_us(500000);
	}

	printf("\r\n");

	active_config = default_config; 

	if(uart_config_reset_counter > 2) {
		uart_config_reset_counter = 0;
		printf("Defaults loaded by %s!\r\n", "user request");
 	} else if((GPIO->INPUT & GPIO_IN_CONFIG_PIN) == 0) {
		printf("Defaults loaded by %s!\r\n", "CONFIG pin");
	} else {
		if(eeprom_probe(I2C0) == 0) {
			if(config_load(&active_config) == 0) {
				printf("Config loaded from EEPROM\r\n");
			} else {
				active_config = default_config;
				printf("Defaults loaded by %s!\r\n", "EEPROM CRC ERROR");
			}
		} else {
			printf("Defaults loaded by %s!\r\n", "EEPROM malfunction");
		}
	} 


	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init

	// AUDIO DAC
	audiodac_init(AUDIODAC0, 16000);

	short *ring_buffer = (short *)malloc(30000*2);

	if(ring_buffer) {
		audiodac0_start_playback(ring_buffer, 30000);
	} else {
		printf("Failed to allocate ring_buffer!\r\n");
	}


	printf("Hardware init done\r\n");

	GPIO->OUTPUT &= ~(GPIO_OUT_LED0 | GPIO_OUT_LED1 | GPIO_OUT_LED2 | GPIO_OUT_LED3);

	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	// Tetris init

	cga_set_video_mode(CGA_MODE_TEXT);
	cga_fill_screen(0);

	delay_us(5000000); // Let monitor to sync

	const int targetFrameTime = 350;
	uint64_t lastTime = get_mtime();

	gameOver = 2; // show greetings

	while(1) {
		static int _scroll = 700;
		static int _scroll_dir = 0;
		static uint32_t last_keys = 0, keys = 0;
		uint32_t new_keys = GPIO->INPUT & (GPIO_IN_KEY0 | GPIO_IN_KEY1 | GPIO_IN_KEY2 | GPIO_IN_KEY3);

		if(new_keys != last_keys) {
	       		keys = new_keys;
			last_keys = new_keys;
		} else {
			keys = 0;
		}



		GPIO->OUTPUT |= GPIO_OUT_LED1; // ON: LED1 - ready

		GPIO->OUTPUT &= ~(GPIO_OUT_LED1 | GPIO_OUT_LED2 | GPIO_OUT_LED3);


		// PLIC
		if(0) {
			printf("Build %05d: irqs = %d, sys_cnt = %d, sbrk_heap_end = %p, "
				"audiodac0_samples_sent = %d, audiodac0_irqs = %d, vblank_irqs = %d\r\n",
				BUILD_NUMBER,
				reg_irq_counter, reg_sys_counter, sbrk_heap_end,
				audiodac0_samples_sent, audiodac0_irqs, cga_vblank_irqs);

			plic_print_stats();

		}


		// Tetris Game screen

		if(gameOver == 0) {

			static uint32_t colorfx_rainbow[] = {
				// Straight
				0x000000ff, 0x000055ff, 0x0000aaff, 0x0000ffff,
				0x0000ffaa, 0x0000ff2a, 0x002bff00, 0x0080ff00,
				0x00d4ff00, 0x00ffd400, 0x00ffaa00, 0x00ff5500,
				0x00ff0000, 0x00ff0055, 0x00ff00aa, 0x00ff00ff,
				// Reversed
				0x00ff00ff, 0x00ff00aa, 0x00ff0055, 0x00ff0000,
				0x00ff5500, 0x00ffaa00, 0x00ffd400, 0x00d4ff00,
				0x0080ff00, 0x002bff00, 0x0000ff2a, 0x0000ffaa,
				0x0000ffff, 0x0000aaff, 0x000055ff, 0x000000ff,
			};

			static int colorfx_offset = 6;

			int colorfx_idx = colorfx_offset;

			cga_wait_vblank_end();

			for(int i = 0; i < 480/2; i++) {
				while(!(CGA->CTRL & CGA_CTRL_HSYNC_FLAG));
				CGA->PALETTE[5] = colorfx_rainbow[colorfx_idx];
				colorfx_idx = (colorfx_idx + 1) % 32;
				while(CGA->CTRL & CGA_CTRL_HSYNC_FLAG);

				while(!(CGA->CTRL & CGA_CTRL_HSYNC_FLAG));
				while(CGA->CTRL & CGA_CTRL_HSYNC_FLAG);
			}

			if(keys) {

				printf("Inputs: last_keys = %04x, new_keys = %04x\r\n", last_keys, new_keys); 

				if(new_keys & GPIO_IN_KEY0)
					processInputs(TETRIS_EVENT_RIGHT);

				if(new_keys & GPIO_IN_KEY3)
					processInputs(TETRIS_EVENT_LEFT);

				if(new_keys & GPIO_IN_KEY1)
					processInputs(TETRIS_EVENT_UP);

				if(new_keys & GPIO_IN_KEY2)
					processInputs(TETRIS_EVENT_DOWN);

				last_keys = new_keys;

				colorfx_idx--;
			}

			uint64_t now = get_mtime();
			uint64_t elapsed = (now - lastTime) * 1000 / 1000000;

			if (elapsed >= targetFrameTime) {
				if (!moveDown()) {
					addToArena();
					checkLines();
					newTetromino();
				}
				lastTime = now;
			}

			drawArena();


		}


		// Game Over screen 
		
		if(gameOver == 1) {

			if(keys) {
				gameOver = 2;
				_scroll = 700;
				continue;
			}

			static uint32_t colorfx_rainbow[] = {
				// Straight
				0x000000ff, 0x000055ff, 0x0000aaff, 0x0000ffff,
				0x0000ffaa, 0x0000ff2a, 0x002bff00, 0x0080ff00,
				0x00d4ff00, 0x00ffd400, 0x00ffaa00, 0x00ff5500,
				0x00ff0000, 0x00ff0055, 0x00ff00aa, 0x00ff00ff,
				// Reversed
				0x00ff00ff, 0x00ff00aa, 0x00ff0055, 0x00ff0000,
				0x00ff5500, 0x00ffaa00, 0x00ffd400, 0x00d4ff00,
				0x0080ff00, 0x002bff00, 0x0000ff2a, 0x0000ffaa,
				0x0000ffff, 0x0000aaff, 0x000055ff, 0x000000ff,
			};

			static int colorfx_offset = 6;

			int colorfx_idx = colorfx_offset;

			cga_wait_vblank();
			cga_set_video_mode(CGA_MODE_TEXT);
			if(_scroll_dir & 1)
				cga_set_scroll(_scroll++);
			else
				cga_set_scroll(_scroll--);
			memcpy(CGA->FB, videobuf_for_text, 20*1024); 
			cga_wait_vblank_end();

			for(int i = 0; i < 480/2; i++) {
				while(!(CGA->CTRL & CGA_CTRL_HSYNC_FLAG));
				CGA->PALETTE[5] = colorfx_rainbow[colorfx_idx];
				colorfx_idx = (colorfx_idx + 1) % 32;
				while(CGA->CTRL & CGA_CTRL_HSYNC_FLAG);

				while(!(CGA->CTRL & CGA_CTRL_HSYNC_FLAG));
				while(CGA->CTRL & CGA_CTRL_HSYNC_FLAG);
			}

			if(_scroll == 16 || _scroll == -16)
				_scroll_dir++;

			memset(videobuf_for_text, 0, 20*1024);

			char *game_over_text =
				"\t ###     ##    #    #  ######     #####   #    #  ######  ##### \r\n"
				"\t#       #  #   ##  ##  #         #     #  #    #  #       #    #\r\n"
				"\t#  ##   ####   # ## #  ###       #     #  #    #  ###     ##### \r\n"
				"\t#   #  #    #  #    #  #         #     #   #  #   #       #   # \r\n" 
				"\t ####  #    #  #    #  ######     #####     ##    ######  #    #\r\n"
				;
			cga_text_print(videobuf_for_text, 0, 10, 5, 0, game_over_text);

			char score_text[32];
			snprintf(score_text, 32, ESC_FG "15;Last score: %u", score);
			cga_text_print(videobuf_for_text, 26, 16, 15, 0, score_text);
			cga_set_cursor_xy(45, 16);
		}


		// Greetings screen

		if(gameOver == 2) {

			if(keys) {
				cga_set_video_mode(CGA_MODE_TEXT);
				cga_fill_screen(0);
				_scroll = 0;
				memset(arena, 0, sizeof(arena[0][0]) * A_HEIGHT * A_WIDTH);
				newGame();
				continue;
			}

			static uint32_t colorfx_rainbow[] = {
				// Straight
				0x000000ff, 0x000055ff, 0x0000aaff, 0x0000ffff,
				0x0000ffaa, 0x0000ff2a, 0x002bff00, 0x0080ff00,
				0x00d4ff00, 0x00ffd400, 0x00ffaa00, 0x00ff5500,
				0x00ff0000, 0x00ff0055, 0x00ff00aa, 0x00ff00ff,
				// Reversed
				0x00ff00ff, 0x00ff00aa, 0x00ff0055, 0x00ff0000,
				0x00ff5500, 0x00ffaa00, 0x00ffd400, 0x00d4ff00,
				0x0080ff00, 0x002bff00, 0x0000ff2a, 0x0000ffaa,
				0x0000ffff, 0x0000aaff, 0x000055ff, 0x000000ff,
			};

			static int colorfx_offset = 6;

			int colorfx_idx = colorfx_offset;

			cga_wait_vblank();
			cga_set_video_mode(CGA_MODE_TEXT);
			cga_set_scroll(_scroll++);
			memcpy(CGA->FB, videobuf_for_text, 20*1024); 
			cga_wait_vblank_end();

			for(int i = 0; i < 480/2; i++) {
				while(!(CGA->CTRL & CGA_CTRL_HSYNC_FLAG));
				CGA->PALETTE[5] = colorfx_rainbow[colorfx_idx];
				colorfx_idx = (colorfx_idx + 1) % 32;
				while(CGA->CTRL & CGA_CTRL_HSYNC_FLAG);

				while(!(CGA->CTRL & CGA_CTRL_HSYNC_FLAG));
				while(CGA->CTRL & CGA_CTRL_HSYNC_FLAG);
			}

			cga_wait_vblank();
			cga_set_video_mode(CGA_MODE_GRAPHICS1);
			cga_set_scroll(0);
			memcpy(CGA->FB, videobuf_for_graphics, 20*1024); 
			cga_wait_vblank_end();

			show_greetings();
		}


		// Audio

		#ifdef	AUDIO
		if(0) {
			int audio_idx = 0;

			for(int i = 0; i < 30000 / SINE_TABLE_SIZE; i++) {

				short audio_buf[SINE_TABLE_SIZE];
				int a;

				for(int j = 0; j < SINE_TABLE_SIZE; j++) {
					audio_buf[j] = sine_table[audio_idx];
					audio_idx++;
					audio_idx %= SINE_TABLE_SIZE;
				}

				if((a = audiodac0_submit_buffer(audio_buf, SINE_TABLE_SIZE, DAC_NOT_ISR)) != SINE_TABLE_SIZE) {
				       	//printf("main: audiodac0_submit_buffer partial: %d (%d), i = %d\r\n", a, SINE_TABLE_SIZE, i);
					break;
				}
			}
		}
		#endif

		reg_sys_counter++;

		GPIO->OUTPUT &= ~GPIO_OUT_LED0; // LED0 is OFF - clear error indicator
	}
}



void timerInterrupt(void) {
	// Not supported on this machine
}


void externalInterrupt(void){

	if(PLIC->PENDING & PLIC_IRQ_UART0) { // UART0 is pending
		GPIO->OUTPUT |= GPIO_OUT_LED3; // LED0 is ON
		//printf("UART0: ");
		while(uart_readOccupancy(UART0)) {
			char c = UART0->DATA;
			uart_write(UART0, c);
			if(c == '*') {
				uart_config_reset_counter ++;
			} else {
				uart_config_reset_counter = 0;
			}
		}
		PLIC->PENDING &= ~PLIC_IRQ_UART0;
	}


	if(PLIC->PENDING & PLIC_IRQ_I2C) { // I2C xmit complete 
		//print("I2C IRQ\r\n");
		PLIC->PENDING &= ~PLIC_IRQ_I2C;
	}


	if(PLIC->PENDING & PLIC_IRQ_UART1) { // UART1 is pending
		//printf("UART1: %02X (%c)\r\n", c, c);
		PLIC->PENDING &= ~PLIC_IRQ_UART1;
	}

	if(PLIC->PENDING & PLIC_IRQ_MAC) { // MAC is pending
		//print("MAC IRQ\r\n");
		PLIC->PENDING &= ~PLIC_IRQ_MAC;
	}

	if(PLIC->PENDING & PLIC_IRQ_TIMER0) { // Timer0 (for MAC) 
		//printf("TIMER0 IRQ\r\n");
		timer_run(TIMER0, 100000); // 100 ms timer
		PLIC->PENDING &= ~PLIC_IRQ_TIMER0;
	}

	if(PLIC->PENDING & PLIC_IRQ_TIMER1) { // Timer1 (for Modbus RTU) 
		//print("TIMER1 IRQ\r\n");
		timer_run(TIMER1, 50000); // 50 ms timer
		PLIC->PENDING &= ~PLIC_IRQ_TIMER1;
	}

	if(PLIC->PENDING & PLIC_IRQ_AUDIODAC0) { // AUDIODAC is pending
		//print("AUDIODAC IRQ\r\n");
		audiodac0_irqs++;
		audiodac0_samples_sent += audiodac0_isr();
		PLIC->PENDING &= ~PLIC_IRQ_AUDIODAC0;
	}

	if(PLIC->PENDING & PLIC_IRQ_CGA_VBLANK) { // CGA vertical blanking 
		//print("VBLANK IRQ\r\n");
		cga_vblank_irqs++;
		PLIC->PENDING &= ~PLIC_IRQ_CGA_VBLANK;
	}
}


static char crash_str[16];

void crash(int cause) {
	
	print("\r\n*** TRAP: ");
	to_hex(crash_str, cause);
	print(crash_str);
	print(" at ");

	to_hex(crash_str, csr_read(mepc));
	print(crash_str);
	print("\r\n");

}

void irqCallback() {

	// Interrupts are already disabled by machine

	reg_irq_counter++;

	int32_t mcause = csr_read(mcause);
	int32_t interrupt = mcause < 0;    //Interrupt if true, exception if false
	int32_t cause     = mcause & 0xF;
	if(interrupt){
		switch(cause) {
			case CAUSE_MACHINE_TIMER: timerInterrupt(); break;
			case CAUSE_MACHINE_EXTERNAL: externalInterrupt(); break;
			default: crash(2); break;
		}
	} else {
		crash(1);
	}
}

