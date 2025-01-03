/*** main.c *******************************************************************\
 * Contributors: twisted_nematic57                                            *
 * Copyright:    GNU GPLv3 or later (see LICENSE)                             *
 * Description:   - Initializes hardware                                      *
 *                - Sets up second core, queues, and FIFOs                    *
 * Date Created: December 31, 2024                                            *
\******************************************************************************/


// Includes
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "pico/time.h"
#include "pico/cyw43_arch.h"

#include <stdio.h>

#include "common.h"
#include "compute.h"
#include "queue_utils.h"

// Functions
void init_basic_hw();
void init_second_core();
void init_multicore_queues();


int main() {
  init_basic_hw();
  init_second_core();
  init_multicore_queues();

	return 0;
}

void init_basic_hw() {
  set_sys_clock_hz(250*1000*1000, true); // need for speed
  stdio_init_all(); // USB CDC + UART (8n1,115200)
  stdio_puts("\n\n\nINIT: Boot Arithmetica-FW v" VERSION " | Copyright 2024 "
             "twisted_nematic57. Released under the GPLv3 or later.\n");

  if (cyw43_arch_init()) { // Try to init CYW43439 (LED + Wireless chip)
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
  queue_init(&entry_queue,sizeof(uint32_t),QUEUE_SIZE); // all data will be cut
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