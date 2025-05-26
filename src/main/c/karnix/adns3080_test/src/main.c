#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "soc.h"
#include "riscv.h"
#include "plic.h"
#include "uart.h"
#include "utils.h"
#include "cga.h"
#include "adns3080.h"

static unsigned char *videobuf = 0;
static uint32_t reg_sys_counter = 0;
static uint32_t reg_irq_counter = 0;
static uint32_t reg_sys_timestamp = 0;
static uint8_t adns3080_frame_buffer[900];

int main(void) {
	int ret;

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init

        /* Initialize heap for malloc to use free RAM right above the stack */
        printk("\r\n*** Init heap:\r\n");
        init_sbrk(NULL, 0);
        printk("init_sbrk done!\r\n");

        printk("heap_start: %p, heap_end: %p, sbrk_heap_end: %p\r\n",
                (unsigned int)heap_start, (unsigned int)heap_end,
                (unsigned int)sbrk_heap_end);


        printk("\r\n*** Adjusting global REENT structure:\r\n");

	__sinit(&_IMPURE_DATA); // Init LIBC impure_data structure

        printk("_impure_ptr: %p, stdout: %p\r\n",
                (unsigned int)_impure_ptr, (unsigned int)(_impure_ptr->_stdout));

	// Can use printf() from here

	printf("\r\n"
		"ADNS-3080 for Karnix SoC. Build %05d on " __DATE__ " at " __TIME__ "\r\n"
		"Copyright (C) 2025 Fabmicro, LLC., Tyumen, Russia.\r\n\r\n",
		BUILD_NUMBER
	);

	GPIO->OUTPUT |= GPIO_OUT_LED0; // LED0 is ON - indicate we are not yet ready

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
	cga_set_video_mode(CGA_MODE_TEXT);

	static const uint32_t grey_palette[16] = {
			0x00000000, 0x11111111, 0x22222222, 0x33333333,
			0x44444444, 0x55555555, 0x66666666, 0x77777777,
			0x88888888, 0x99999999, 0xaaaaaaaa, 0xbbbbbbbb,
			0xcccccccc, 0xdddddddd, 0xeeeeeeee, 0xffffffff,
			};
	cga_set_palette((uint32_t*)grey_palette);

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


	// Init ADNS-3080 on SS0
	ret = adns3080_init(SPI0, SPI_SS0, GPIO_PIN5, GPIO_PIN4, 1000000); // SPI_SCLK = 1MHz
	printf("adns3080_init: ret = 0x%04X\r\n", ret);

	printf("=== Hardware init done ===\r\n");

	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	cga_set_video_mode(CGA_MODE_TEXT);
	cga_fill_screen(0);

	delay_us(2000000); // Let video monitor to sync

	while(1) {

		GPIO->OUTPUT |= GPIO_OUT_LED1; // ON: LED1 - ready

		GPIO->OUTPUT &= ~(GPIO_OUT_LED1 | GPIO_OUT_LED2 | GPIO_OUT_LED3);

		uint32_t timestamp = get_mtime();

		// Print resource usage statistics 
		if(timestamp - reg_sys_timestamp >= 1000) {

			reg_sys_timestamp = timestamp;
			reg_sys_counter++;
		}

		if(reg_sys_counter % 1000 == 0) {

			GPIO->OUTPUT &= ~GPIO_OUT_LED0; // LED0 is OFF - clear error indicator

			printf("Build %05d: irqs = %d, sys_cnt = %d, sbrk_heap_end = %p\r\n",
				BUILD_NUMBER,
				reg_irq_counter, reg_sys_counter, sbrk_heap_end
			);

			//plic_print_stats();

			adns3080_capture_frame(SPI0, SPI_SS0, adns3080_frame_buffer); 

			uint8_t *img_ptr = adns3080_frame_buffer; 
			uint32_t *fb = (uint32_t*)CGA->FB;

			for(int y = 0; y < 30; y++) {
				for(int x = 0; x < 30; x++)
					*fb++ = ((((*img_ptr++) & 0x3f) >> 2) << 8) | 0x8d; 
				fb += 50;
			}

			adns3080_init(SPI0, SPI_SS0, GPIO_PIN5, GPIO_PIN4, 1000000); // SPI_SCLK = 1MHz

		}

		if(reg_sys_counter % 50 == 0) {
			uint8_t motion = adns3080_read_reg(SPI0, SPI_SS0, 0x02); // Motion
			int8_t delta_x = adns3080_read_reg(SPI0, SPI_SS0, 0x03); // Delta X 
			int8_t delta_y = adns3080_read_reg(SPI0, SPI_SS0, 0x04); // Delta Y 
			uint8_t squal = adns3080_read_reg(SPI0, SPI_SS0, 0x05); // SQUAL 

			printf("ADNS3080: motion = %02X, delta_x = %d, delta_y = %d, squal = %u\r\n",
				motion, delta_x, delta_y, squal);

			char text[32];
			snprintf(text, 32, "Motion:\t0x%02X\t", motion);
			cga_text_print(CGA->FB, 36, 0, 15, 0, text);
			snprintf(text, 32, "Squal:\t%u\t", squal);
			cga_text_print(CGA->FB, 36, 1, 15, 0, text);
			snprintf(text, 32, "Delta_X:\t%d\t", delta_x);
			cga_text_print(CGA->FB, 36, 2, 15, 0, text);
			snprintf(text, 32, "Delta_Y:\t%d\t", delta_y);
			cga_text_print(CGA->FB, 36, 3, 15, 0, text);
		}

	}
}



void timerInterrupt(void) {
	// Not supported on this machine
}


void externalInterrupt(void){

	if(PLIC->PENDING & PLIC_IRQ_UART0) { // UART0 is pending
		GPIO->OUTPUT |= GPIO_OUT_LED3; // LED0 is ON
		while(uart_readOccupancy(UART0)) {
			char c = UART0->DATA;
			uart_write(UART0, c);
		}
		PLIC->PENDING &= ~PLIC_IRQ_UART0;
	}


	if(PLIC->PENDING & PLIC_IRQ_TIMER0) { // Timer0 (for MAC) 
		//printf("TIMER0 IRQ\r\n");
		timer_run(TIMER0, 100000); // 100 ms timer
		PLIC->PENDING &= ~PLIC_IRQ_TIMER0;
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

