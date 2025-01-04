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
	bool usb_connected;
	uint32_t cursor_xpos;
	uint32_t entry_len;
} status;


void do_ui() {
	status serial_ui;
	serial_ui.return_val = 0; // 0 in this case means "continue running"
	serial_ui.prompt_active = 0; // initialize serial prompt on start
	serial_ui.usb_connected = stdio_usb_connected();

	int key;
	while(serial_ui.return_val == 0) { // Core 0 will be handling UI.
		// Future LCD/keypad stuff goes here

		// Serial I/O (USB and UART specifics are both handled by the SDK)
		if(serial_ui.prompt_active == 0) { // is this a new prompt/startup?
			stdio_put_string("> ",2,0,0);
			serial_ui.prompt_active = 1;
			serial_ui.cursor_xpos = 0; // initial x-position is always 0
			serial_ui.entry_len = 0; // entry length is always 0 on startup/new entry
		}

		key = stdio_getchar_timeout_us(100000);
		/*if(key != PICO_ERROR_TIMEOUT) {
			printf("%d\n",key);
		}*/

		if(key > 50 && key < 127 && key != 27) { // a letter, number, symbol, etc.
			stdio_putchar_raw(key);
			serial_ui.cursor_xpos++;
			serial_ui.entry_len++;

		} /*else if(key == 27) { // ESC: for escape keys like arrows, home/end, etc.
			int key_extend[3] = {0,0,0}; // max of 3 chars come after ESC.
			for(uint8_t i = 0; i <= 2; ++i) { // get each of the 3 possible next chars
				key_extend[i] = stdio_getchar_timeout_us(0);
			}

			// key_extend now contains int representations of each keycode of the
			// special key.

			if(key_extend[0] == 91 && key_extend[1] == 67) { // Right arrow
				if(serial_ui.cursor_xpos < serial_ui.entry_len) {
					//printf("\033[1C");
					stdio_putchar_raw((int)'\033');
					stdio_putchar_raw((int)'[');
					//stdio_putchar_raw((int)'1');
					stdio_putchar_raw((int)'C');
					serial_ui.cursor_xpos++;
				}
			} else if(key_extend[0] == 91 && key_extend[1] == 68) { // Left arrow
				if(serial_ui.cursor_xpos > 0) {
					//printf("\033[1D");
					stdio_putchar_raw((int)'\033');
					stdio_putchar_raw((int)'[');
					//stdio_putchar_raw((int)'1');
					stdio_putchar_raw((int)'D');
					serial_ui.cursor_xpos--;
				}
			}

		}*/ else if(key == 127) { // Backspace
			if(serial_ui.cursor_xpos > 0) {
				// Move back a char, print a space to make the character look empty, and
				// move the cursor back again to produce functionality of a backspace.
				stdio_put_string("\b \b",3,0,0);
				serial_ui.cursor_xpos--;
				serial_ui.entry_len--;
			}
		} else if(key == 49) {
				printf("\033[1D");
		} else if(key == 50) {
				printf("\033[1C");
		}
	}
}