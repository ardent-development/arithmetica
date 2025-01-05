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
#include <stdlib.h>
#include "pico/stdlib.h"
#include <inttypes.h>
#include <string.h>

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
	bool usb_prev_connected;
	uint32_t cursor_xpos;
	uint32_t entry_len;
	char *this_entry;
	uint32_t entry_mem_size;
} status;


void do_ui() {
	status tui; // "Text User Interface"
	tui.return_val = 0; // 0 in this case means "continue running"
	tui.prompt_active = 0; // initialize serial prompt on start
	tui.usb_prev_connected = stdio_usb_connected();
	tui.entry_mem_size = 2; // 1 char + null

	tui.this_entry = (char*)malloc(tui.entry_mem_size);
	if(tui.this_entry == NULL) {
		stdio_puts("FATAL #5: Out of RAM. Halting.");
		while(true) {}
	}
	/*this_entry[0] = 'a';
	this_entry[1] = '\0';
	printf(this_entry);*/

	int key;
	while(tui.return_val == 0) { // Core 0 will be handling UI.
		// Future LCD/keypad stuff goes here

		// If the user is connecting via USB, let them know that the prompt is 
		// active. This is unfortunately not possible with UART, and the user will
		// just have to start typing blindly into the prompt.
		if(tui.usb_prev_connected == 0 && stdio_usb_connected() == 1) {
			stdio_puts("Connected to Arithmetica serial terminal via USB. Ready.\n");
			tui.prompt_active = 0; // Reinitialize the prompt
		}
		tui.usb_prev_connected = stdio_usb_connected();

		// Serial I/O (USB and UART I/O specifics are both handled by the SDK)
		if(tui.prompt_active == 0) { // is this a new prompt/startup?
			stdio_put_string("> ",2,0,0);
			tui.prompt_active = 1;
			tui.cursor_xpos = 0; // initial x-position is always 0
			tui.entry_len = 0; // entry length is always 0 on startup/new entry
		}

		key = stdio_getchar_timeout_us(0);
		/*if(key != PICO_ERROR_TIMEOUT) {
			printf("%d\n",key);
		}*/

		/* TODO KEYS:
				- DEL:  27 91 51 126
				- INS:  27 91 50 126
				- HOME: 27 91 49 126
				- END:  27 91 52 126
				- PgUp: 27 91 53 126
				- PgDn: 27 91 54 126
		*/

		if(key > 0 && key < 127 && key != 27) { // a letter, number, symbol, etc.
			stdio_putchar_raw(key);
			tui.cursor_xpos++;
			tui.entry_len++;
			printf("A");

			// Keep track of this new input.
			if(tui.entry_len+2 > tui.entry_mem_size) { // (+ new char + null)
				tui.entry_mem_size *= 2; // Removes need to frequently realloc
				printf("B");
				char *new_entry = realloc(tui.this_entry,tui.entry_mem_size);
				printf("C");
				if(new_entry == NULL) {
					stdio_puts("FATAL #6: Out of RAM. Halting.");
					while(true) {}
				}
				tui.this_entry = new_entry;
				printf("D");
			}
			//memcpy(tui.this_entry,(char[]){key,0},2);
			tui.this_entry[tui.entry_len - 1] = key;
			tui.this_entry[tui.entry_len] = 0;
			printf("E");
			printf("\nContents: %s\n",tui.this_entry);

		} else if(key == 27) { // ESC: for escape keys like arrows, home/end, etc.
escape_key_detect:
			int key_extend[3] = {0,0,0}; // max of 3 chars come after ESC.
			for(uint8_t i = 0; i <= 2; i++) { // get each of the 3 possible next chars
				key_extend[i] = stdio_getchar_timeout_us(2000);
				// Waiting for 2000us pretty much ensures that we recieve the rest of
				// the characters in the sequence at 9600 baud. With wait times below 
				// 1500us, the MCU can "miss" the rest of the sequence after ESC. This
				// is not a problem with USB due to its much faster speed. This delay
				// time also does not affect editing speed noticeably.
			}
			// key_extend now contains int representations of each keycode of the
			// special key.

			if(key_extend[0] == 91 && key_extend[1] == 67) { // Right arrow
				if(tui.cursor_xpos < tui.entry_len) {
					stdio_put_string("\033[C",3,0,0); // move cursor right
					tui.cursor_xpos++;
				}
			} else if(key_extend[0] == 91 && key_extend[1] == 68) { // Left arrow
				if(tui.cursor_xpos > 0) {
					stdio_put_string("\033[D",3,0,0); // move cursor left
					tui.cursor_xpos--;
				}
			}

			if(key_extend[2] == 27) { // Is there another escape key to process?
				goto escape_key_detect; // If so, process it
			}

		} else if(key == 127) { // Backspace
			if(tui.cursor_xpos > 0) {
				// Move back a char, print a space to make the character look empty, and
				// move the cursor back again to produce functionality of a backspace.
				stdio_put_string("\b \b",3,0,0);
				tui.cursor_xpos--;
				tui.entry_len--;
			}
		}
	}
}