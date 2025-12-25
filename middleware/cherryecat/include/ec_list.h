/*
 * Copyright (c) 2025, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EC_LIST_H
#define EC_LIST_H

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ec_container_of - return the member address of ptr, if the type of ptr is the
 * struct type.
 */
#define ec_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/**
 * Single List structure
 */
struct ec_slist_node {
    struct ec_slist_node *next; /**< point to next node. */
};
typedef struct ec_slist_node ec_slist_t; /**< Type for single list. */

/**
 * @brief initialize a single list
 *
 * @param l the single list to be initialized
 */
static inline void ec_slist_init(ec_slist_t *l)
{
    l->next = NULL;
}

static inline void ec_slist_add_head(ec_slist_t *l, ec_slist_t *n)
{
    n->next = l->next;
    l->next = n;
}

static inline void ec_slist_add_tail(ec_slist_t *l, ec_slist_t *n)
{
    ec_slist_t *tmp = l;

    while (tmp->next) {
        tmp = tmp->next;
    }

    /* append the node to the tail */
    tmp->next = n;
    n->next = NULL;
}

static inline void ec_slist_insert(ec_slist_t *l, ec_slist_t *next, ec_slist_t *n)
{
    if (!next) {
        ec_slist_add_tail(next, l);
        return;
    }

    while (l->next) {
        if (l->next == next) {
            l->next = n;
            n->next = next;
        }

        l = l->next;
    }
}

static inline ec_slist_t *ec_slist_remove(ec_slist_t *l, ec_slist_t *n)
{
    ec_slist_t *tmp = l;
    /* remove slist head */
    while (tmp->next && tmp->next != n) {
        tmp = tmp->next;
    }

    /* remove node */
    if (tmp->next != (ec_slist_t *)0) {
        tmp->next = tmp->next->next;
    }

    return l;
}

static inline unsigned int ec_slist_len(const ec_slist_t *l)
{
    unsigned int len = 0;
    const ec_slist_t *list = l->next;

    while (list != NULL) {
        list = list->next;
        len++;
    }

    return len;
}

static inline unsigned int ec_slist_contains(ec_slist_t *l, ec_slist_t *n)
{
    while (l->next) {
        if (l->next == n) {
            return 0;
        }

        l = l->next;
    }

    return 1;
}

static inline ec_slist_t *ec_slist_head(ec_slist_t *l)
{
    return l->next;
}

static inline ec_slist_t *ec_slist_tail(ec_slist_t *l)
{
    while (l->next) {
        l = l->next;
    }

    return l;
}

static inline ec_slist_t *ec_slist_next(ec_slist_t *n)
{
    return n->next;
}

static inline int ec_slist_isempty(ec_slist_t *l)
{
    return l->next == NULL;
}

/**
 * @brief initialize a slist object
 */
#define EC_SLIST_OBJESCT_INIT(object) \
    {                                 \
        NULL                          \
    }

/**
 * @brief initialize a slist object
 */
#define EC_SLIST_DEFINE(slist) \
    ec_slist_t slist = { NULL }

/**
 * @brief get the struct for this single list node
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define ec_slist_entry(node, type, member) \
    ec_container_of(node, type, member)

/**
 * ec_slist_first_entry - get the first element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define ec_slist_first_entry(ptr, type, member) \
    ec_slist_entry((ptr)->next, type, member)

/**
 * ec_slist_tail_entry - get the tail element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define ec_slist_tail_entry(ptr, type, member) \
    ec_slist_entry(ec_slist_tail(ptr), type, member)

/**
 * ec_slist_first_entry_or_null - get the first element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define ec_slist_first_entry_or_null(ptr, type, member) \
    (ec_slist_isempty(ptr) ? NULL : ec_slist_first_entry(ptr, type, member))

/**
 * ec_slist_for_each - iterate over a single list
 * @pos:    the ec_slist_t * to use as a loop cursor.
 * @head:   the head for your single list.
 */
#define ec_slist_for_each(pos, head) \
    for (pos = (head)->next; pos != NULL; pos = pos->next)

#define ec_slist_for_each_safe(pos, next, head)    \
    for (pos = (head)->next, next = pos->next; pos; \
         pos = next, next = pos->next)

/**
 * ec_slist_for_each_entry  -   iterate over single list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your single list.
 * @member: the name of the list_struct within the struct.
 */
#define ec_slist_for_each_entry(pos, head, member)                 \
    for (pos = ec_slist_entry((head)->next, typeof(*pos), member); \
         &pos->member != (NULL);                                    \
         pos = ec_slist_entry(pos->member.next, typeof(*pos), member))

#define ec_slist_for_each_entry_safe(pos, n, head, member)          \
    for (pos = ec_slist_entry((head)->next, typeof(*pos), member),  \
        n = ec_slist_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (NULL);                                     \
         pos = n, n = ec_slist_entry(pos->member.next, typeof(*pos), member))

/**
 * Double List structure
 */
struct ec_dlist_node {
    struct ec_dlist_node *next; /**< point to next node. */
    struct ec_dlist_node *prev; /**< point to prev node. */
};
typedef struct ec_dlist_node ec_dlist_t; /**< Type for lists. */

/**
 * @brief initialize a list
 *
 * @param l list to be initialized
 */
static inline void ec_dlist_init(ec_dlist_t *l)
{
    l->next = l->prev = l;
}

/**
 * @brief insert a node after a list
 *
 * @param l list to insert it
 * @param n new node to be inserted
 */
static inline void ec_dlist_add_head(ec_dlist_t *l, ec_dlist_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/**
 * @brief insert a node before a list
 *
 * @param n new node to be inserted
 * @param l list to insert it
 */
static inline void ec_dlist_add_tail(ec_dlist_t *l, ec_dlist_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

/**
 * @brief remove node from list.
 * @param n the node to remove from the list.
 */
static inline void ec_dlist_remove(ec_dlist_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

/**
 * @brief move node from list.
 * @param n the node to remove from the list.
 */
static inline void ec_dlist_move_head(ec_dlist_t *l, ec_dlist_t *n)
{
    ec_dlist_remove(n);
    ec_dlist_add_head(l, n);
}

/**
 * @brief move node from list.
 * @param n the node to remove from the list.
 */
static inline void ec_dlist_move_tail(ec_dlist_t *l, ec_dlist_t *n)
{
    ec_dlist_remove(n);
    ec_dlist_add_tail(l, n);
}

/**
 * @brief tests whether a list is empty
 * @param l the list to test.
 */
static inline int ec_dlist_isempty(const ec_dlist_t *l)
{
    return l->next == l;
}

/**
 * @brief get the list length
 * @param l the list to get.
 */
static inline unsigned int ec_dlist_len(const ec_dlist_t *l)
{
    unsigned int len = 0;
    const ec_dlist_t *p = l;

    while (p->next != l) {
        p = p->next;
        len++;
    }

    return len;
}

/**
 * @brief remove and init list
 * @param n the list to get.
 */
static inline void ec_dlist_del_init(ec_dlist_t *n)
{
    ec_dlist_remove(n);
}

/**
 * @brief initialize a dlist object
 */
#define EC_DLIST_OBJESCT_INIT(object) \
    {                                 \
        &(object), &(object)          \
    }
/**
 * @brief initialize a dlist object
 */
#define EC_DLIST_DEFINE(list) \
    ec_dlist_t list = { &(list), &(list) }

/**
 * @brief get the struct for this entry
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define ec_dlist_entry(node, type, member) \
    ec_container_of(node, type, member)

/**
 * dlist_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define ec_dlist_first_entry(ptr, type, member) \
    ec_dlist_entry((ptr)->next, type, member)
/**
 * dlist_first_entry_or_null - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define ec_dlist_first_entry_or_null(ptr, type, member) \
    (ec_dlist_isempty(ptr) ? NULL : ec_dlist_first_entry(ptr, type, member))

/**
 * ec_dlist_for_each - iterate over a list
 * @pos:    the ec_dlist_t * to use as a loop cursor.
 * @head:   the head for your list.
 */
#define ec_dlist_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * ec_dlist_for_each_prev - iterate over a list
 * @pos:    the dlist_t * to use as a loop cursor.
 * @head:   the head for your list.
 */
#define ec_dlist_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * ec_dlist_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:    the dlist_t * to use as a loop cursor.
 * @n:      another dlist_t * to use as temporary storage
 * @head:   the head for your list.
 */
#define ec_dlist_for_each_safe(pos, n, head)              \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)

#define ec_dlist_for_each_prev_safe(pos, n, head)         \
    for (pos = (head)->prev, n = pos->prev; pos != (head); \
         pos = n, n = pos->prev)
/**
 * ec_dlist_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define ec_dlist_for_each_entry(pos, head, member)                 \
    for (pos = ec_dlist_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head);                                    \
         pos = ec_dlist_entry(pos->member.next, typeof(*pos), member))

/**
 * ec_dlist_for_each_entry_reverse  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define ec_dlist_for_each_entry_reverse(pos, head, member)         \
    for (pos = ec_dlist_entry((head)->prev, typeof(*pos), member); \
         &pos->member != (head);                                    \
         pos = ec_dlist_entry(pos->member.prev, typeof(*pos), member))

/**
 * ec_dlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define ec_dlist_for_each_entry_safe(pos, n, head, member)          \
    for (pos = ec_dlist_entry((head)->next, typeof(*pos), member),  \
        n = ec_dlist_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head);                                     \
         pos = n, n = ec_dlist_entry(n->member.next, typeof(*n), member))

/**
 * ec_dlist_for_each_entry_safe_reverse - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define ec_dlist_for_each_entry_safe_reverse(pos, n, head, member)  \
    for (pos = ec_dlist_entry((head)->prev, typeof(*pos), field),   \
        n = ec_dlist_entry(pos->member.prev, typeof(*pos), member); \
         &pos->member != (head);                                     \
         pos = n, n = ec_dlist_entry(pos->member.prev, typeof(*pos), member))

#ifdef __cplusplus
}
#endif

#endif /* EC_LIST_H */
