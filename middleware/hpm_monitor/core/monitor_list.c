#include "monitor_list.h"

void init_static_data_list(StaticDataLinkedList *list)
{
    for (int i = 0; i < MONITOR_DATA_LIST_MAXCOUNT - 1; i++)
    {
        list->nodes[i].next = &list->nodes[i + 1];
        list->nodes[i].used = false;
    }
    list->nodes[MONITOR_DATA_LIST_MAXCOUNT - 1].next = NULL;
    list->free_list = (ListDataNode*)&list->nodes;
    list->active_list = NULL;
}

ListDataNode *data_allocate_node(StaticDataLinkedList *list)
{
    if (!list->free_list)
        return NULL;

    ListDataNode *node = list->free_list;
    list->free_list = node->next;
    node->used = true;
    node->next = NULL;
    return node;
}

bool data_list_add(StaticDataLinkedList *list, uint32_t addr, uint32_t len, int result, int ch_or_sample, uint32_t tick_us)
{
    ListDataNode *new_node = data_allocate_node(list);
    if (!new_node)
        return false;

    new_node->addr = addr;
    new_node->len = len;
    new_node->result = result;
    new_node->ch_or_sample = ch_or_sample;
    new_node->tick_us = tick_us;

    new_node->next = list->active_list;
    list->active_list = new_node;
    return true;
}

bool data_list_remove_of_addr(StaticDataLinkedList *list, uint32_t addr)
{
    ListDataNode **ptr = &list->active_list;
    while (*ptr)
    {
        ListDataNode *entry = *ptr;
        if (entry->addr == addr)
        {
            *ptr = entry->next;
            entry->next = list->free_list;
            entry->used = false;
            list->free_list = entry;
            return true;
        }
        ptr = &entry->next;
    }
    return false;
}

bool data_list_remove_node(StaticDataLinkedList *list, ListDataNode *node)
{
    ListDataNode **ptr = &list->active_list;
    while (*ptr)
    {
        ListDataNode *entry = *ptr;
        if(entry == node)
        {
            *ptr = entry->next;
            entry->next = list->free_list;
            entry->used = false;
            list->free_list = entry;
            return true;
        }
        ptr = &entry->next;
    }
    return false;
}

ListDataNode *data_list_get_of_addr(StaticDataLinkedList *list, uint32_t addr)
{
    ListDataNode *current = list->active_list;
    while (current)
    {
        if (current->addr == addr)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

ListDataNode *data_list_get_valid(StaticDataLinkedList *list)
{
    ListDataNode *current = list->active_list;
    if (current)
    {
        return current;
    }
    return NULL;
}