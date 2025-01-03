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

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "pico/time.h"

#include "common.h"
#include "queue_utils.h"
#include "ui.h"


void do_ui() {

}
