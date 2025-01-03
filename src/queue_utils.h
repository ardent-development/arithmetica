/*** queue_utils.h ************************************************************\
 * Contributors: twisted_nematic57                                            *
 * Copyright:    GNU GPLv3 or later (see LICENSE)                             *
 * Description:   - Defines commonly used queue structures                    *
 * Date Created: January 2, 2025                                              *
\******************************************************************************/

#ifndef QUEUE_UTILS_H
#define QUEUE_UTILS_H


// Includes
#include "pico/util/queue.h"

// Defines
#define QUEUE_SIZE 4 // 4 entries at a time, max.

// Structs
typedef struct {
	uint32_t data;
} entry;

// Variables
queue_t entry_queue; // input
queue_t result_queue; // output


#endif /* QUEUE_UTILS_H */