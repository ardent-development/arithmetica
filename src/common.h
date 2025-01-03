/*** common.h *****************************************************************\
 * Contributors: twisted_nematic57                                            *
 * Copyright:    GNU GPLv3 or later (see LICENSE)                             *
 * Description:   - Defines miscellaneous macros used throughout the firmware *
 * Date Created: January 2, 2024                                              *
\******************************************************************************/

#ifndef COMMON_H
#define COMMON_H


// Defines
#define VERSION "0.0.1.0"

#define MC_TEST_0 12345 // Used to verify integrity of inter-core communication
#define MC_TEST_1 54321 // Can be any uint32_t, really.

#define false 0 // Either VS Code is dumb or I'm dumber.
#define true 1

// Hardcoded UART parameters - feel free to edit and recompile
#define UART_ID uart0
#define BAUD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1


#endif /* COMMON_H */
