/*** main.c *******************************************************************\
 * Contributors: twisted_nematic57                                            *
 * Copyright:    GNU GPLv3 or later (see LICENSE                              *
 * Description:   - Initializes hardware                                      *
 *                - Sets up second core                                       *
\******************************************************************************/


// Includes
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "pico/time.h"
#include "pico/cyw43_arch.h"

#include <stdio.h>
#include <stdbool.h>

// Defines
#define VERSION "0.0.1.0"

#define MC_FIFO_TEST_0 12345 // Used to verify integrity of inter-core
#define MC_FIFO_TEST_1 54321 // communication

#define true 1 // VSCode complains true is undefined without this...?

void core1_entry() {
  multicore_fifo_push_blocking(MC_FIFO_TEST_1); // signal core 1 started to 0

  uint32_t mc1rx = multicore_fifo_pop_blocking();
  if(mc1rx == MC_FIFO_TEST_0) { // is the recieved data intact?
    stdio_puts("INIT: Core0->Core1 works");
  } else {
    stdio_puts("FATAL #1: Core0->Core1 is not working.\n");
    while(1) {}
  }
}

int main() {
  set_sys_clock_hz(250*1000*1000, true); // need for speed
  stdio_init_all(); // USB + UART (8n1,115200)
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
    stdio_puts("");
  }

  multicore_launch_core1(core1_entry);
  uint32_t mc0rx = multicore_fifo_pop_blocking(); // wait for core 1 to start
  if(mc0rx == MC_FIFO_TEST_1) { // is the recieved data (Core1->Core0) intact?
    stdio_puts("INIT: Core1->Core0 works");
    multicore_fifo_push_blocking(MC_FIFO_TEST_0); // begin testing Core0->Core1
  } else {
    stdio_puts("FATAL #0: Core1->Core0 is not working.\n");
    return -1;
  }

	return 0;
}