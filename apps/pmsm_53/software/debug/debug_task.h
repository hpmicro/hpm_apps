#ifndef __DEBUG_TASK_H
#define __DEBUG_TASK_H

#include <stdint.h>

typedef int (*debug_channel_cb)(uint8_t *src, uint16_t length);

void debug_init(void);

void debug_register(debug_channel_cb output_cb);

/*
* result: < 0, the data incomplete; == 0, parse success; > 0, parse error;
*/
int debug_src_process(uint8_t *data, uint32_t length);

void debug_handle(void);

#endif //__DEBUG_TASK_H