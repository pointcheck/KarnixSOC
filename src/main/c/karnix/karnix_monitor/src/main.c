#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "lwip/netif.h"
#include "soc.h"
#include "riscv.h"
#include "plic.h"
#include "mac.h"
#include "i2c.h"
#include "uart.h"
#include "sram.h"
#include "utils.h"
#include "modbus.h"
#include "modbus_udp.h"
#include "modbus_rtu.h"
#include "eeprom.h"
#include "config.h"
#include "hub.h"
#include "cga.h"
#include "crc32.h"
#include "qspi.h"
#include "context.h"
#include "cli.h"

#define	WELCOME_TEXT "Welcome to Karnix SoC Monitor. Copyright (C) 2024-2025, Fabmicro, LLC.\r\nBuild #%04u at %s %s. Main addr: %p\r\n\r\n"

extern void __sinit(void *);
extern unsigned int _IMPURE_DATA;

extern struct netif default_netif;

volatile uint32_t console_config_reset_counter = 0;
volatile uint32_t events_mac_poll = 0;
volatile uint32_t events_modbus_rtu_poll = 0;
volatile uint32_t events_console_poll = 0;
volatile uint32_t reg_sys_counter = 0;
volatile uint32_t reg_irq_counter = 0;
volatile uint32_t reg_sys_print_stats = 3;
volatile uint32_t reg_cga_vblank_irqs = 0;

#if(RESET_ON_SOFT_START)
__attribute__ ((section (".noinit"))) uint32_t deadbeef;	// If equal to 0xdeadbeef - we are in soft-start mode
#endif


void process_and_wait(uint32_t us) {

	context_save();

	if(context.trap_flag) {
		context_restore();
		cli_prompt();
		goto skip;
	}

	unsigned long long t0, t;
	static unsigned int skip = 0;

	t0 = MTIME;

	while(1) {
		t = MTIME;

		if(t - t0 >= us)
			break;

		// Process events, save execution context priorily


		if(events_console_poll) {
			events_console_poll = 0;
			console_poll(); 
		}

		if(events_modbus_rtu_poll) {
			events_modbus_rtu_poll = 0;
			modbus_rtu_poll(); 
		}

		if(events_mac_poll) {
			events_mac_poll = 0;
			mac_poll();
		}

	}

	skip:;
}


void main() {

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init

	delay_us(2000000); // Wait for FCLK to settle

	#if(RESET_ON_SOFT_START)
	if(deadbeef == 0xdeadbeef) {
		printk("Soft-start, performing hard reset!\r\n");
		delay_us(200000);
		hard_reboot();
	} else {
		deadbeef = 0xdeadbeef;
	}
	#endif

	init_sbrk(NULL, 0); // Initialize heap for malloc to use on-chip RAM
	__sinit(&_IMPURE_DATA); // Init LIBC impure_data structure

	printf(WELCOME_TEXT, BUILD_NUMBER, __DATE__, __TIME__, &main);

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

	// Test SRAM and initialize heap for malloc to use SRAM if tested OK
	printf("Testing SRAM at: %p, size: %u\r\n", SRAM_ADDR_BEGIN, SRAM_SIZE);
	if(sram_test_write_random_ints(3) == 0) {
		printf("SRAM %s!\r\n", "enabled"); 
		//init_sbrk((unsigned int*)SRAM_ADDR_BEGIN, SRAM_SIZE);
		//printf("Using SRAM as heap\r\n");
		// If this prints, we are running with new heap all right
		// Note, that some garbage can be printed along, that's ok!
	} else {
		printf("SRAM %s!\r\n", "disabled"); 
	}

	// Perform CGA video RAM test
	printf("Testing CGA\r\n");
	cga_ram_test(1);

	// Setup CGA: set text mode and load color palette
	cga_set_video_mode(CGA_MODE_TEXT);

	static const uint32_t rgb_palette[16] = {
			0x00000000, 0x000000f0, 0x0000f000, 0x00f00000,
			0x0000f0f0, 0x00f000f0, 0x00f0f000, 0x00f0f0f0,
			0x000f0f0f, 0x000f0fff, 0x000fff0f, 0x00ff0f0f,
			0x000fffff, 0x00ff0fff, 0x00ffff0f, 0x00ffffff,
			};
	cga_set_palette((uint32_t*)rgb_palette);

	// Clear video framebuffer
	memset(CGA->FB, 0, CGA_FRAMEBUFFER_SIZE);
	cga_set_cursor_xy(0, 0);
	cga_set_scroll(0);

	// Print Welcome test to CGA
	{
		char str[256];
		sprintf(str, WELCOME_TEXT, BUILD_NUMBER, __DATE__, __TIME__, &main);
		cga_text_print(CGA->FB, -1, -1, 15, 0, str);
	}

	// Enable writes to EEPROM
	GPIO->OUTPUT &= ~GPIO_OUT_EEPROM_WP; 

	// Configure UART0 IRQ sources: bit(0) - TX interrupts, bit(1) - RX interrupts 
	UART0->STATUS = (UART0->STATUS | UART_STATUS_RX_IRQ_EN); // Allow only RX interrupts 
	printf("UART0 RX IRQ enabled\r\n");

	// Configure UART1 IRQ sources: bit(0) - TX interrupts, bit(1) - RX interrupts 
	UART1->STATUS = (UART1->STATUS | UART_STATUS_RX_IRQ_EN); // Allow only RX interrupts 
	printf("UART1 RX IRQ enabled\r\n");

	// Intialize and configure HUB controller
	//hub_init(active_config.hub_type);

	// Configure network stuff 
	mac_lwip_init(); // Initialize MAC controller and LWIP stack, also add network interface

	// Init and bind Modbus/UDP module to UDP port
	modbus_udp_init();

	// Setup serial paramenters for Modbus/RTU module
	modbus_rtu_init();
	
	// Setup TIMER0 to 100 ms timer for Mac: 25 MHz / 25 / 10000
	timer_prescaler(TIMER0, SYSTEM_CLOCK_HZ / 1000000);
	timer_run(TIMER0, 100000);
	printf("TIMER0 set to 100 ms\r\n");

	// Setup TIMER0 to 50 ms timer for Modbus: 25 MHz / 25 / 10000
	timer_prescaler(TIMER1, SYSTEM_CLOCK_HZ / 1000000);
	timer_run(TIMER1, 50000);
	printf("TIMER1 set to 25 ms\r\n");

	// Setup interrupt controller 
	PLIC->POLARITY = 0x0000007f; // Configure PLIC IRQ polarity for UART0, UART1, MAC, I2C and AUDIODAC0 to Active High 
	PLIC->EDGE = 0xfffffff8; // MAC, UARTs and TIMERs are Fixed Level IRQs
	PLIC->PENDING = 0; // Clear all pending IRQs
	PLIC->ENABLE = 0x0000001f; // Enable IRQ lines for: UART0, UART1, TIMER0, TIMER1 and MAC
	printf("PLIC configred: ENABLE: %p, POLARITY: %p, EDGE: %p\r\n",
			PLIC->ENABLE, PLIC->POLARITY, PLIC->EDGE);

	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

       	// GPIO LEDs are OFF - all things do well 
	GPIO->OUTPUT &= ~(GPIO_OUT_LED0 | GPIO_OUT_LED1 | GPIO_OUT_LED2 | GPIO_OUT_LED3);

	printf("=== Hardware init done ===\r\n\r\n");

	while(1) {

	       	// Clear: LED2 - MAC/MODBUS Error, LED3 - UART I/O
		GPIO->OUTPUT &= ~(GPIO_OUT_LED2 | GPIO_OUT_LED3);

    		process_and_wait(5000); 

		if(reg_sys_counter % 100 == 0) { // 0.5 Hz
			GPIO->OUTPUT ^= GPIO_OUT_LED1; // Toggle LED1 - ready
		}

		if(reg_sys_print_stats &&
			reg_sys_counter % (200*reg_sys_print_stats) == 0) { // T=1 sec * reg_sys_print_stats

			printf("\rSTATS: build %05d: irqs = %d, sys_cnt = %d, scratch = %p, sbrk_heap_end = %p, "
					"console_rx_buf_len = %d\r\n",
				BUILD_NUMBER,
				reg_irq_counter, reg_sys_counter, reg_scratch, sbrk_heap_end,
				console_rx_buf_len);

			plic_print_stats();

			mac_print_stats();

			cli_prompt();

		}

		reg_sys_counter++;

		if(reg_config_write)
			reg_config_write--;

	}
}



void timerInterrupt(void) {
	// Not supported on this machine
}


void externalInterrupt(void){

	if(PLIC->PENDING & PLIC_IRQ_UART0) { // UART0 is pending
		GPIO->OUTPUT |= GPIO_OUT_LED3; // LED0 is ON
		//printf("UART0: ");

		console_rx();
		events_console_poll = 1;

		PLIC->PENDING &= ~PLIC_IRQ_UART0;
	}


	if(PLIC->PENDING & PLIC_IRQ_I2C) { // I2C xmit complete 
		//printk("I2C IRQ\r\n");
		PLIC->PENDING &= ~PLIC_IRQ_I2C;
	}


	if(PLIC->PENDING & PLIC_IRQ_UART1) { // UART1 is pending
		//printf("UART1: %02X (%c)\r\n", c, c);
		modbus_rtu_rx();
		events_modbus_rtu_poll = 1;
		PLIC->PENDING &= ~PLIC_IRQ_UART1;
	}

	if(PLIC->PENDING & PLIC_IRQ_MAC) { // MAC is pending
		//printk("MAC IRQ\r\n");
		mac_rx();
		PLIC->PENDING &= ~PLIC_IRQ_MAC;
	}

	if(PLIC->PENDING & PLIC_IRQ_TIMER0) { // Timer0 (for MAC) 
		//printf("TIMER0 IRQ\r\n");
		timer_run(TIMER0, 100000); // 100 ms timer
		events_mac_poll = 1;
		PLIC->PENDING &= ~PLIC_IRQ_TIMER0;
	}

	if(PLIC->PENDING & PLIC_IRQ_TIMER1) { // Timer1 (for Modbus RTU) 
		//printk("TIMER1 IRQ\r\n");
		timer_run(TIMER1, 50000); // 50 ms timer
		events_modbus_rtu_poll = 1;
		PLIC->PENDING &= ~PLIC_IRQ_TIMER1;
	}


	if(PLIC->PENDING & PLIC_IRQ_CGA_VBLANK) { // CGA vertical blanking 
		//printk("VBLANK IRQ\r\n");
		reg_cga_vblank_irqs++;
		PLIC->PENDING &= ~PLIC_IRQ_CGA_VBLANK;
	}
}


void crash(int cause) {
	
	context.trap_flag = 1;

	context.cur_pc = csr_read(mepc);
	context.mtval = csr_read(mtval);

	printk("\r\n*** TRAP: %p at %p = %p, mtval = %p\r\n",
		cause, context.cur_pc, *(uint32_t*)(context.cur_pc & 0xfffffffc), context.mtval);

	printk("\r*** SAVED: gp = %p, tp = %p, ra = %p, sp = %p, pc = %p\r\n",
		context.gp, context.tp, context.ra, context.sp, context.pc);

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


	//if((reg_irq_counter & 0xff) == 0) { 
	//	printf("IRQ COUNTER: %d\r\n", reg_irq_counter);
	//}

	
	if(context.trap_flag) // Restore saved return address if trap
		csr_write(mepc, context.ra);

	// Interrupt state will be restored by MRET further in trap_entry.
}


