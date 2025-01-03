/*** main.c *******************************************************************\
 * Contributors: twisted_nematic57                                            *
 * Copyright:    GNU GPLv3 or later (see LICENSE)                             *
 * Description:   - Initializes hardware                                      *
 *                - Sets up second core, queues, and FIFOs                    *
 * Date Created: December 31, 2024                                            *
\******************************************************************************/


// Includes
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "pico/time.h"
#include "pico/cyw43_arch.h"

#include "common.h"
#include "compute.h"
#include "queue_utils.h"
#include "ui.h"

// Functions
void init_basic_hw();
void init_second_core();
void init_multicore_queues();


int main() {
	init_basic_hw(); // USB, UART, high-speed system clock, onboard LED setup
	init_second_core(); // FIFO testing
	init_multicore_queues(); // Queue testing

	do_ui(); // User Interfacing - serial only, for now

	return 0;
}

void init_basic_hw() {
	set_sys_clock_hz(225*1000*1000, false); // ~225MHz should be acceptable in all
																					// conditions. Plus, more speed.

	stdio_usb_init(); // start serial interface over USB CDC
	stdio_puts("INIT: USB serial initialized. Now initting UART...");

	gpio_set_function(UART_TX_PIN,GPIO_FUNC_UART);
	gpio_set_function(UART_RX_PIN,GPIO_FUNC_UART);
	stdio_uart_init_full(UART_ID,BAUD_RATE,UART_TX_PIN,UART_RX_PIN);
	uart_set_fifo_enabled(UART_ID,true);
	uart_set_format(UART_ID,DATA_BITS,STOP_BITS,PARITY);
	uint actual_uart_baudrate = uart_set_baudrate(UART_ID,BAUD_RATE);
	//^ we repeatedly set the baud rate because we have to get the actual rate at
	//  the end. This information is printed later


	stdio_puts("\n\n\n");
	stdio_puts("INIT: Booting Arithmetica-FW v" VERSION " | Copyright "
						 "2024-2025 Ardent Development. Released under the GPLv3 or "
						 "later.\n");

	// Clock/baud information
	printf("INIT: CPUs clocked at ~%dMHz\n",clock_get_hz(clk_sys)/1000/1000);
	printf("INIT: UART0 operates at %u baud (true rate)\n",actual_uart_baudrate);

	if (cyw43_arch_init()) { // Try to init CYW43439 (to access onboard LED)
		stdio_puts("ERROR: CYW43439 init failed. Onboard LED capabilities shall be "
							 "disabled.\n");
	} else {
		stdio_puts("INIT: CYW43439 initialized. Infodump below."); // the firmware
																									// automatically does the dump

		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1); // Enable onboard LED (power
																									 // indicator)
		stdio_puts("[End of CYW43439 infodump]\n");
	}
}

void init_second_core() {
	stdio_puts("INIT: Launching Core 1...");
	multicore_launch_core1(core1_main);

	stdio_puts("INIT: Testing FIFOs...");
	uint32_t mc0rx = multicore_fifo_pop_blocking(); // wait for core 1 to start
	if(mc0rx == MC_TEST_1) { // is the recieved data (Core1->Core0) intact?
		stdio_puts("INIT: Core1->Core0 FIFO works");
		multicore_fifo_push_blocking(MC_TEST_0); // begin testing Core0->Core1
	} else {
		stdio_puts("FATAL #1: Core1->Core0 FIFO is not working. Halting.\n");
		while(1) {}
	}
}

void init_multicore_queues() {
	stdio_puts("INIT: Attempting to set up entry/result queueing...");

	// Queue defined in common.h
	queue_init(&entry_queue,sizeof(entry),QUEUE_SIZE); // all data will be cut
																										 // into 32-bit limbs

	uint32_t data0 = MC_TEST_0; // begin testing functionality of the entry queue
	queue_add_blocking(&entry_queue, &data0); // send test data down entry queue

	// begin testing functionality of the result queue
	queue_remove_blocking(&result_queue,&data0); // get Core 1's test data
	if(data0 == MC_TEST_1) { // is the recieved data intact?
		stdio_puts("INIT: result queue works");
	} else {
		stdio_puts("FATAL #4: result queue is not working. Halting.\n");
		while(1) {}
	}

	stdio_puts("INIT: queues and FIFOs work, now initializing interfaces...\n");
}