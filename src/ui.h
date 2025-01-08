/*** ui.h *********************************************************************\
 * Contributors: twisted_nematic57                                            *
 * Copyright:    GNU GPLv3 or later (see LICENSE)                             *
 *   Description:   - Defines functions in ui.c                               *
 * Date Created: January 2, 2024                                              *
\******************************************************************************/

#ifndef UI_H
#define UI_H

// Structs
typedef struct {
	uint8_t return_val;
	bool prompt_active;
	bool usb_prev_connected;
	uint16_t cursor_xpos;
	uint16_t entry_len;
	bool insert;
	char *this_entry;
} user_interface;

// Functions
void do_ui();
void update_entry_lines(user_interface *ui);

#endif /* UI_H */