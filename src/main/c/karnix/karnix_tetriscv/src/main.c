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
#include "sram.h"
#include "utils.h"
#include "eeprom.h"
#include "cga.h"
#include "tetris.h"
#include "config.h"

#define	AUDIO_ENABLED	1
#define	USE_SRAM	1	// If set, test and use SRAM for Heap, otherwise use RAM.
//#define	PRINT_STATS	1	// If set, prints IRQ and buffer statistics once a second

// LIBC stuff
extern void __sinit(void *);
extern unsigned int _IMPURE_DATA;

volatile uint32_t uart_config_reset_counter = 0;
volatile uint32_t reg_irq_counter = 0;
volatile uint32_t reg_sys_counter = 0;
volatile uint64_t reg_sys_timestamp = 0;

#if(AUDIO_ENABLED)
#define	AUDIO_RING_BUFFER_SIZE	(2048*2)		// Audio playback ring buffer
volatile uint32_t audiodac0_irqs = 0;
volatile uint32_t audiodac0_samples_sent = 0;
#endif

volatile uint32_t cga_vblank_irqs = 0;

uint32_t console_config_reset_counter = 0;

#if(RESET_ON_SOFT_START)
__attribute__ ((section (".noinit"))) uint32_t deadbeef;	// If equal to 0xdeadbeef - we are in soft-start mode
#endif

#define	VIDEO_BUFFER_SIZE	(20*1024)		// Size of double buffer
static unsigned char *videobuf_for_text = 0;		// Double buffer for text
static unsigned char *videobuf_for_graphics = 0;	// Double buffer for graphics

static const uint32_t colorfx_rainbow[] = {
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

#define	PI	3.1415926535
float my_sine(float x)
{
	
	x = ((int)(x * 57.2957795147) % 360) * 0.0174532925194; // fmod(x, 2*PI)

	float res = 0, pow = x, fact = 1;

	for(int i = 0; i < 5; ++i) { // increase to 9 for smooth sine wave
		res+=pow/fact;
		pow*=-1*x*x;
		fact*=(2*(i+1))*(2*(i+1)+1);
	}
	return res;
}

#include "sprites.h"
int _sprite_idx;

void show_greetings(void) {

	const char *greetings = 
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
		"\t\tBy @checkpoint <rz@fabmicro.ru> // Tyumen, 2025\r\n"
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
	       
	memset(videobuf_for_text, 0, VIDEO_BUFFER_SIZE);
	cga_text_print(videobuf_for_text, 0,  0, 15, 0, (char*)greetings);
	cga_set_cursor_xy(48, 22);

	memset(videobuf_for_graphics, 0, VIDEO_BUFFER_SIZE);

	for(int y = 0; y < 240; y += 48)
		for(int x = 0; x < 320; x += 48)
			cga_bitblit((uint8_t*)sprites[(_sprite_idx+y) % 11], videobuf_for_graphics,
				x + 64 * my_sine((x+_sprite_idx)/80.0),
				y + 64 * my_sine((x+_sprite_idx)/80.0),
				16, 16, CGA_VIDEO_WIDTH, CGA_VIDEO_HEIGHT);

	_sprite_idx++;
}

extern const struct __sFILE_fake __sf_fake_stdin;
extern const struct __sFILE_fake __sf_fake_stdout;
extern const struct __sFILE_fake __sf_fake_stderr;
extern unsigned int _IMPURE_DATA; /* reference to .data.impure_data section */

int main(void) {

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init

	#if(RESET_ON_SOFT_START)
	if(deadbeef == 0xdeadbeef) {
		printk("Soft-start, performing hard reset!\r\n");
		delay_us(200000);
		hard_reboot();
	} else {
		deadbeef = 0xdeadbeef;
	}
	#endif

        /* Initialize heap for malloc to use free RAM right above the stack */
        printk("\r\n*** Init heap:\r\n");
        init_sbrk(NULL, 0);
        printk("init_sbrk done!\r\n");

        printk("heap_start: %p, heap_end: %p, sbrk_heap_end: %p\r\n",
                (unsigned int)heap_start, (unsigned int)heap_end,
                (unsigned int)sbrk_heap_end);


        printk("\r\n*** Adjusting global REENT structure:\r\n");

        *(uint32_t*)&_impure_ptr = (uint32_t)&_IMPURE_DATA;
        *(uint32_t*)&_global_impure_ptr = (uint32_t)_impure_ptr;
        _impure_ptr->_stdin = (__FILE *)&__sf_fake_stdin;
        _impure_ptr->_stdout = (__FILE *)&__sf_fake_stdout;
        _impure_ptr->_stderr = (__FILE *)&__sf_fake_stderr;

        printk("_impure_ptr: %p, _global_impure_ptr: %p, fake_stdout: %p\r\n",
                (unsigned int)_impure_ptr, (unsigned int)_global_impure_ptr,
                (unsigned int)(_impure_ptr->_stdout));

	// Can use printf() from here

	printf("\r\n"
		"TetRISC-V for Karnix SoC. Build %05d on " __DATE__ " at " __TIME__ "\r\n"
		"Copyright (C) 2024-2025 Fabmicro, LLC., Tyumen, Russia.\r\n\r\n",
		BUILD_NUMBER
	);

	GPIO->OUTPUT |= GPIO_OUT_LED0; // LED0 is ON - indicate we are not yet ready

	// Enable I2C for EEPROM
	i2c_init(I2C0);

        printf("=== Configuring ===\r\n");

        // Reset to Factory Defaults if "***" sequence received from UART0
        fpurge(stdout);
        printf("\r\nPress '*' to reset config");
        fflush(stdout);

        console_config_reset_counter = 0;

        for(int i = 0; i < 5; i++) {
                if(uart_readOccupancy(UART0)) {
                        char c = UART0->DATA;

                        if(c == '*') {
                                uart_write(UART0, c);
                                console_config_reset_counter++;
                        } else {
                                console_config_reset_counter = 0;
                        }
                }

                if(console_config_reset_counter > 2) // has user typed *** on the console ?
                        break;

                if((GPIO->INPUT & GPIO_IN_CONFIG_PIN) == 0) // is CONFIG pin tied to ground ?
                        break;

                putchar('.');
                fflush(stdout);
                delay_us(500000);
        }

        printf("\r\n");

        active_config = default_config;

        if(console_config_reset_counter > 2) {
                console_config_reset_counter = 0;
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

        printf("=== Hardware init ===\r\n");

	#if(USE_SRAM)
	// Test SRAM and initialize heap for malloc to use SRAM if tested OK
	if(sram_test_write_random_ints(1) == 0) {
		printf("Enabling SRAM...\r\n");
		init_sbrk((unsigned int*)SRAM_ADDR_BEGIN, SRAM_SIZE);
		printf("SRAM at %p is %s!\r\n", SRAM_ADDR_BEGIN, "enabled"); 
		// If this prints, we are running with new heap all right
		// Note, that some garbage can be printed along, that's ok!
	} else {
		printf("SRAM at %p is %s!\r\n", SRAM_ADDR_BEGIN, "disabled"); 
	}
	#endif


	// Init CGA: enable graphics mode and load color palette
	cga_set_video_mode(CGA_MODE_GRAPHICS1);

	static const uint32_t rgb_palette[16] = {
			0x00000000, 0x000000f0, 0x0000f000, 0x00f00000,
			0x0000f0f0, 0x00f000f0, 0x00f0f000, 0x00f0f0f0,
			0x000f0f0f, 0x000f0fff, 0x000fff0f, 0x00ff0f0f,
			0x000fffff, 0x00ff0fff, 0x00ffff0f, 0x00ffffff,
			};
	cga_set_palette((uint32_t*)rgb_palette);

	printf("CGA init done\r\n");

	// Reset PLIC interrupt controller and disable all IRQ lines 
	PLIC->ENABLE = 0;
	PLIC->POLARITY = 0;
	PLIC->EDGE = 0;
	PLIC->PENDING = 0;

	// Configure UART0 IRQ sources: bit(0) - TX interrupts, bit(1) - RX interrupts 
	UART0->STATUS |= UART_STATUS_RX_IRQ_EN; // Allow only RX interrupts 
	PLIC->EDGE &= ~PLIC_IRQ_UART0;
	PLIC->POLARITY |= PLIC_IRQ_UART0;
	PLIC->ENABLE |= PLIC_IRQ_UART0;
		
	printf("UART0 init done\r\n");

	// Setup TIMER0 to 100 ms timer for Mac: 25 MHz / 25 / 10000
	timer_prescaler(TIMER0, SYSTEM_CLOCK_HZ / 1000000);
	timer_run(TIMER0, 100000);
	PLIC->EDGE |= PLIC_IRQ_TIMER0;
	PLIC->POLARITY |= PLIC_IRQ_TIMER0;
	PLIC->ENABLE |= PLIC_IRQ_TIMER0;

	printf("TIMER0 init done\r\n");

	// Setup TIMER0 to 50 ms timer for Modbus: 25 MHz / 25 / 10000
	timer_prescaler(TIMER1, SYSTEM_CLOCK_HZ / 1000000);
	timer_run(TIMER1, 50000);
	PLIC->EDGE |= PLIC_IRQ_TIMER1;
	PLIC->POLARITY |= PLIC_IRQ_TIMER1;
	PLIC->ENABLE |= PLIC_IRQ_TIMER1;

	printf("TIMER1 init done\r\n");

	// AUDIO DAC
	#if(AUDIO_ENABLED)
	audiodac_init(AUDIODAC0, 8000);

	short *audio_ring_buffer = (short *)malloc(AUDIO_RING_BUFFER_SIZE);

	if(audio_ring_buffer) {
		audiodac0_start_playback(audio_ring_buffer, AUDIO_RING_BUFFER_SIZE/2);
	} else {
		printf("Failed to allocate %d bytes for audio_ring_buffer, sbrk_heap_end = %p\r\n",
			AUDIO_RING_BUFFER_SIZE, sbrk_heap_end);
		return -1;
	}

	// Test pattern: SAW
	static int q = 0;
	for(int i = 0; i < AUDIO_RING_BUFFER_SIZE/2; i++) {
		audio_ring_buffer[i] = q;
		q = (q + 64) & 0xffff;
	}

	// Configure AUDIOCAD0 IRQ source
	//PLIC->EDGE |= PLIC_IRQ_AUDIODAC0;
	PLIC->EDGE &= ~PLIC_IRQ_AUDIODAC0;
	PLIC->POLARITY |= PLIC_IRQ_AUDIODAC0;
	PLIC->ENABLE |= PLIC_IRQ_AUDIODAC0;

	printf("AUDIODAC0 init done\r\n");
	#endif


	// Init video double buffers
	videobuf_for_text = malloc(VIDEO_BUFFER_SIZE);

	if(videobuf_for_text == NULL) {
		printf("Failed to allocate %d bytes for videobuf_for_text, sbrk_heap_end = %p\r\n",
			VIDEO_BUFFER_SIZE, sbrk_heap_end);
		return -1;
	}
	
	videobuf_for_graphics = malloc(VIDEO_BUFFER_SIZE);

	if(videobuf_for_graphics == NULL) {
		printf("Failed to allocate %d bytes for videobuf_for_graphics, sbrk_heap_end = %p\r\n",
			VIDEO_BUFFER_SIZE, sbrk_heap_end);
		return -1;
	}
	
	printf("Video double-buffers allocated\r\n");

	// Enable writes to EEPROM
	GPIO->OUTPUT &= ~GPIO_OUT_EEPROM_WP; 

	GPIO->OUTPUT &= ~(GPIO_OUT_LED0 | GPIO_OUT_LED1 | GPIO_OUT_LED2 | GPIO_OUT_LED3);

	printf("=== Hardware init done ===\r\n");

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

		// Print resource usage statistics 
		#if(PRINT_STATS)
		uint64_t timestamp = get_mtime();
		if(timestamp - reg_sys_timestamp >= 1000000) {

			printf("Build %05d: irqs = %d, sys_cnt = %d, sbrk_heap_end = %p, "
				"audiodac0_samples_sent = %d, audiodac0_irqs = %d, audiodac0_tx_fifo_empty = %d, "
				"audiodac0_tx_buffer_empty = %d, vblank_irqs = %d\r\n",
				BUILD_NUMBER,
				reg_irq_counter, reg_sys_counter, sbrk_heap_end,
				audiodac0_samples_sent, audiodac0_irqs,
				audiodac0_tx_fifo_empty, audiodac0_tx_buffer_empty,
				cga_vblank_irqs);

			//plic_print_stats();

			reg_sys_timestamp = timestamp;
		}
		#endif

		// Tetris Game screen

		if(gameOver == 0) {

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

			const char *game_over_text =
				"\t ###     ##    #    #  ######     #####   #    #  ######  ##### \r\n"
				"\t#       #  #   ##  ##  #         #     #  #    #  #       #    #\r\n"
				"\t#  ##   ####   # ## #  ###       #     #  #    #  ###     ##### \r\n"
				"\t#   #  #    #  #    #  #         #     #   #  #   #       #   # \r\n" 
				"\t ####  #    #  #    #  ######     #####     ##    ######  #    #\r\n"
				;
			cga_text_print(videobuf_for_text, 0, 10, 5, 0, (char*)game_over_text);

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
			memcpy(CGA->FB, videobuf_for_graphics, VIDEO_BUFFER_SIZE); 
			cga_wait_vblank_end();

			show_greetings();
		}


		// Audio

		#if(AUDIO_ENABLED_2)
		{
			csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init
			if(audiodac0_tx_ring_buffer_playback_ptr > 0)
				audiodac0_tx_ring_buffer_fill_ptr = audiodac0_tx_ring_buffer_playback_ptr - 1;
			else
				audiodac0_tx_ring_buffer_fill_ptr = audiodac0_tx_ring_buffer_size - 1;
			csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts
		}
		#endif

		#if(AUDIO_ENABLED)
		{
			static unsigned int t = 0;
			short buf[128];

			int processed;

			int a1 = audiodac0_samples_filled();
			int samples_left_to_send = audiodac0_samples_available();

			// Synthesize audio effect using formulae:
			
			while(samples_left_to_send) {

				int samples_to_send = MIN(128, samples_left_to_send);

				for(int i = 0; i < samples_to_send; i++) {

					// 560Hz sine wave
					//buf[i] = (my_sine(t*560.0*6.28318530698/8000.0) * 32767);
					//t = (t + 1) % 360;

					if(gameOver == 1 || gameOver == 2)
						buf[i] = t*(t^t+(t>>15|1)^(t-1280^t)>>10) << 8; 
					else
						buf[i] = ((t*9&t>>4|t*5&t>>7|t*3&t/1024)-1) << 8;
					t++;
				}

				processed = audiodac0_submit_buffer(buf, samples_to_send, DAC_NOT_ISR);

				if(processed != samples_to_send) {
			      		printk("audiodac0_submit_buffer partial: %d of %d\r\n", processed, samples_to_send);
					t -= (samples_to_send - processed); // rollback t for a number of unprocessed samples 
					break;
				}

				samples_left_to_send -= samples_to_send;
			}

			int a2 = audiodac0_samples_filled();

			if(a1 < 20)
				printk("audiodac0 samples available: %d -> %d\r\n", a1, a2);

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

	#if(AUDIO_ENABLED)
	if(PLIC->PENDING & PLIC_IRQ_AUDIODAC0) { // AUDIODAC is pending
		//print("AUDIODAC IRQ\r\n");
		audiodac0_irqs++;
		audiodac0_samples_sent += audiodac0_isr();
		PLIC->PENDING &= ~PLIC_IRQ_AUDIODAC0;
	}
	#endif

	if(PLIC->PENDING & PLIC_IRQ_CGA_VBLANK) { // CGA vertical blanking 
		//print("VBLANK IRQ\r\n");
		cga_vblank_irqs++;
		PLIC->PENDING &= ~PLIC_IRQ_CGA_VBLANK;
	}
}


void crash(int cause) {
	
	printk("\r\n*** TRAP: %p at %p, mtval: %p\r\n", cause, csr_read(mepc), csr_read(mtval));

	for(;;);

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

