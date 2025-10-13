#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "soc.h"
#include "plic.h"
#include "riscv.h"
#include "usb10.h"
#include "utils.h"

extern void __sinit(void *);
extern unsigned int _IMPURE_DATA;

const char *WELCOME_TEXT = "Welcome to Karnix Test. Copyright (C) 2024-2025, Fabmicro, LLC.\r\nBuild #%04u at %s %s. Main addr: %p\r\n\r\n";

uint32_t reg_sys_counter = 0;
uint32_t reg_usb_error_count = 0;


void main() {

	csr_clear(mstatus, MSTATUS_MIE); // Disable Machine interrupts during hardware init
	PLIC->ENABLE = 0; // Disable MicroPLIC interrupts

	delay_us(2000000); // Wait for FCLK to settle

	init_sbrk(NULL, 0); // Initialize heap for malloc to use on-chip RAM
	__sinit(&_IMPURE_DATA); // Init LIBC impure_data structure

	xprintf(WELCOME_TEXT, BUILD_NUMBER, __DATE__, __TIME__, &main);

	// Enable USB1
	USB1->CONTROL &= ~USB10_CONTROL_ENABLE_BIT;
	delay_us(1000);
	USB1->CONTROL |= USB10_CONTROL_RESET_DELAY_SET(1500000 / 1000 * 10); // Set reset duration to 11ms (num of ticks at 1.5 MHz
	USB1->CONTROL |= USB10_CONTROL_KEEPALIVE_BIT;
	USB1->CONTROL |= USB10_CONTROL_ENABLE_BIT;
	xprintf("USB1 enabled\r\n");

	csr_set(mstatus, MSTATUS_MIE); // Enable Machine interrupts

	while(1) {
		delay_us(1000);

		reg_sys_counter++; // This is 1ms counter

		if(usb10_device_address == 0 && reg_sys_counter % 1000 == 0) {
			// Perform USB bus scan each 1 sec 
			xprintf("USB bus scan: reg_sys_counter = %d... ", reg_sys_counter);

			// No devices connected at the moment, try to init
			// Note: usb10_device_address is gobal variable holding address of connected device

			int8_t ret;
			uint8_t new_device_address;

			if((ret = usb10_init(USB1, &new_device_address, NULL, NULL, NULL, NULL)) == 0) {

				// New device detected and initialized

				reg_usb_error_count = 0;

				xprintf("\r\nUSB1: new device addr = %d, VID/PID = 0x%04X/0x%04X, "
					"class/subclass/proto = %d/%d/%d, "
					"EndpointAddress = %d, Interval = %d, MaxPower = %d mA\r\n",
					new_device_address,
					usb10_device_descr.idVendor,
					usb10_device_descr.idProduct,
					usb10_interface_descr.bInterfaceClass,
					usb10_interface_descr.bInterfaceSubclass,
					usb10_interface_descr.bInterfaceProtocol,
					usb10_endpoint_descr.bEndpointAddress & 0x0f,
					usb10_endpoint_descr.bInterval,
					usb10_config_descr.bMaxPower * 2
				);
			} else {
				xprintf("failed with ret = %d\r\n", ret);
			}
		}


		if(usb10_device_address > 0 && reg_sys_counter % 20 == 0) {

			// Perform USB "Interrupt IN" transaction every 20ms 

			uint8_t endpoint = 1; // Commonly used 1, but should be usb10_endpoint_descr.bEndpointAddress & 0x0f
			uint8_t response_data[8] = {0};
			int response_size;
			int device_type;

			// Try to guess response data packet size using class info

			if(usb10_interface_descr.bInterfaceClass == 3 &&
				  usb10_interface_descr.bInterfaceSubclass == 1 &&
				  usb10_interface_descr.bInterfaceProtocol == 1) {

				// We have to check RX packet len (88 bits) to skip empty packets

				response_size = 8; // HID keyboard 
				device_type = 1;

			} else if(usb10_interface_descr.bInterfaceClass == 3 &&
			   usb10_interface_descr.bInterfaceSubclass == 1 &&
			   usb10_interface_descr.bInterfaceProtocol == 2) {

				response_size = 4; // HID mouse
				device_type = 2;

			} else if(usb10_interface_descr.bInterfaceClass == 3 &&
				  usb10_interface_descr.bInterfaceSubclass == 0 &&
			  	  usb10_interface_descr.bInterfaceProtocol == 0) {

				response_size = 8; // HID gamepad 
				device_type = 3;

			} else {
				response_size = 8; // Unknown
				device_type = 0;
			}
			
			// Poll Endpoint for new data
			int ret = usb10_in_request(USB1, usb10_device_address, endpoint,
					response_data, response_size);

			if(ret >= 0) {

				reg_usb_error_count = 0;

				xprintf("\rUSB1 (%d:%d) data received: ", usb10_device_address, endpoint);

				for(int i = 0; i < response_size; i++)
					xprintf("%02X ", response_data[i]);

				xprintf(", class = %d/%d/%d, RX_STATUS: 0x%08X, RX_STATUS2: 0x%08X, STATUS: 0x%08X\r\n",
					usb10_interface_descr.bInterfaceClass,
					usb10_interface_descr.bInterfaceSubclass, 
					usb10_interface_descr.bInterfaceProtocol,
					USB1->RX_STATUS, USB1->RX_STATUS2, USB1->STATUS);

			} else if(ret == USB10_IN_ENAK || ret == USB10_IN_ETIMEOUT) { // NAK or timeout (dupe) 
				// These are legitimate error codes for not ready device, do nothing
				goto usb_end;
			} else {
				if(++reg_usb_error_count > 3) { // More than 3 errors in a row means connection is broken
					xprintf("\rUSB1 (%d:%d) failed, ret = %d\r\n", usb10_device_address, endpoint, ret);
					usb10_device_address = 0; // flag USB as broken
					goto usb_end;
				}
			}

			// Some data processing code can be put here

			if(device_type == 1) { // keyboard
				if((reg_sys_counter & 0x3ff) == 0x100) { // Send HID all LEDs on
					usb10_hid_set_led(USB1, usb10_device_address, 1, 0xff);
				}

				if((reg_sys_counter & 0x3ff) == 0x200) { // Send HID all LEDs off
					usb10_hid_set_led(USB1, usb10_device_address, 1, 0x00);
				}
			}

			usb_end:;
		}

	}
}




void timerInterrupt(void) {
	// Not supported on this machine
}


void externalInterrupt(void){
	// Not supported on this machine
}


void crash(int cause) {
	
	xprintf("\r\n*** TRAP: cause = %p, mtval = %p, pc = %p\r\n",
		cause, csr_read(mtval), csr_read(mepc));

	for(;;);
}

void irqCallback() {

	// Interrupts are already disabled by machine

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

	// Interrupt state will be restored by MRET further in trap_entry.
}


