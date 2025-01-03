/*** ui.c *********************************************************************\
 * Contributors: twisted_nematic57                                            *
 * Copyright:    GNU GPLv3 or later (see LICENSE)                             *
 *   Description:   - Runs the user interfaces of the calculator: serial      *
 *                    (USB/UART) and someday might run the display and handle *
 *                    keypad input.                                           *
 * Date Created: January 2, 2024                                              *
\******************************************************************************/


// Includes
#include <stdio.h>
#include "pico/stdlib.h"
#include <inttypes.h>

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "pico/time.h"

#include "common.h"
#include "queue_utils.h"
#include "ui.h"

typedef struct {
	uint8_t return_val;
	bool prompt_active;
	uint32_t input_len;
} status;


void do_ui() {
	status ui_status;
	ui_status.return_val = 0; // 0 in this case means "continue running"
	ui_status.prompt_active = 0; // initialize serial prompt on start

	int key;

	while(ui_status.return_val == 0) { // Core 0 will be handling UI.
		// Future LCD/keypad stuff goes here

		// Serial I/O
		if(ui_status.prompt_active == 0) {
			stdio_put_string("> ",2,0,0);
			ui_status.input_len = 0;
			ui_status.prompt_active = 1;
		}

		key = stdio_getchar_timeout_us(0);
		if(key != PICO_ERROR_TIMEOUT) {
			ui_status.prompt_active = 0;
			ui_status.input_len++;
			printf("got char! input_len=%" PRIu32 " | key=%d | char=%c\n",
						 ui_status.input_len,key,(char)key);
		}
	}
}
