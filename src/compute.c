/*** compute.c ****************************************************************\
 * Contributors: twisted_nematic57                                            *
 * Copyright:    GNU GPLv3 or later (see LICENSE)                             *
 * Description:   - Confirms Core 1 is working correctly                      *
 *                - Handles entries from Core 0 (+,-,×,/,%,√)                 *
 * Date Created: January 1, 2025                                              *
\******************************************************************************/


// Includes
#include "pico/stdlib.h"

#include "pico/multicore.h"
#include "pico/time.h"

#include <stdio.h>

#include "common.h"
#include "compute.h"
#include "queue_utils.h"

// Functions
void verify_fifo();
void verify_queue();

void core1_entry() {
  verify_fifo();
  verify_queue();
}

void verify_fifo() {
  multicore_fifo_push_blocking(MC_TEST_1); // signal core 1 started to 0

  uint32_t mc1rx = multicore_fifo_pop_blocking();
  if(mc1rx == MC_TEST_0) { // is the recieved data intact?
    stdio_puts("INIT: Core0->Core1 FIFO works");
  } else {
    stdio_puts("FATAL #2: Core0->Core1 FIFO is not working. Halting.\n");
    while(1) {}
  }
}

void verify_queue() {
  queue_init(&result_queue,sizeof(uint32_t),QUEUE_SIZE); // all data will be cut
                                                         // into 32-bit limbs

  uint32_t data1; // begin testing functionality of the entry queue
  queue_remove_blocking(&entry_queue,&data1); // get Core 0's test data
  if(data1 == MC_TEST_0) { // is the recieved data intact?
    stdio_puts("INIT: entry queue works");
  } else {
    stdio_puts("FATAL #3: entry queue is not working. Halting.\n");
    while(1) {}
  }

  data1 = MC_TEST_1; // begin testing functionality of the result queue
  queue_add_blocking(&result_queue, &data1); // send test data down entry queue
}