#ifndef __MONITOR_LIST_H
#define __MONITOR_LIST_H

#include <stdint.h>
#include <stdbool.h>
#include "monitor_kconfig.h"
#include "monitor_profile.h"

typedef struct ListDataNode {
    uint32_t addr;
    uint32_t len;
    int result;
    int ch_or_sample; //sample=-1, ch >= 0
    uint32_t tick_us;
    struct ListDataNode* next;
    bool used;
} ListDataNode;

typedef struct ListCustomNode {
    uint32_t addr;
    uint32_t len;
    uint32_t data_addr;
    uint32_t data_len;
    int ch;
    struct ListCustomNode* next;
    bool used;
} ListCustomNode;

typedef struct {
    ListDataNode nodes[MONITOR_DATA_LIST_MAXCOUNT];
    ListDataNode* free_list;
    ListDataNode* active_list;
} StaticDataLinkedList;

void init_static_data_list(StaticDataLinkedList *list);

bool data_list_add(StaticDataLinkedList *list, uint32_t addr, uint32_t len, int result, int ch_or_sample, uint32_t tick_us);

bool data_list_remove_of_addr(StaticDataLinkedList *list, uint32_t addr);

bool data_list_remove_node(StaticDataLinkedList *list, ListDataNode *node);

ListDataNode *data_list_get_of_addr(StaticDataLinkedList *list, uint32_t addr);

ListDataNode *data_list_get_valid(StaticDataLinkedList *list);

#endif //__MONITOR_LIST_H