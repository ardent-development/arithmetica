/*** main.c *******************************************************************\
 * Contributors: twisted_nematic57                                            *
 * Copyright:    GNU GPLv3 or later (see LICENSE                              *
 * Description:  Initializes hardware and set up external interfaces; then,   *
 *               runs the main loop                                           *
\******************************************************************************/


// Includes
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/clocks.h"
#include "pico/time.h"
#include "pico/cyw43_arch.h"

#include <stdio.h>
#include <stdbool.h>

// Defines


#define true 1 // VSCode problem fixing

int main() {
  set_sys_clock_hz(250*1000*1000, true); // need for speed
  stdio_init_all(); // USB + UART (8n1,115200)
  stdio_puts("\n\n\nINIT: Boot Arithmetica-FW v0.0.1.0 | Copyright 2024 "
             "twisted_nematic57. Released under the GPLv3 or later.\n");

  if (cyw43_arch_init()) { // Try to init CYW43439
    stdio_puts("ERROR: CYW43439 init failed. Onboard LED capabilities shall be"
               " disabled.");
  } else {
    stdio_puts("INIT: CYW43439 initialized. Infodump below.");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1); /* Enable onboard LED (power
                                                      indicator) */
		stdio_puts("\n");
  }

	return 0;
}