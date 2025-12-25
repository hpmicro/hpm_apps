#ifndef __MONITOR_MEM_H
#define __MONITOR_MEM_H

void monitor_mem_reset(void);

void *monitor_mem_alloc(size_t size);

void monitor_mem_free(void *ptr);

#endif //__MONITOR_MEM_H