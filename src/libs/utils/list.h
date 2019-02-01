/*
 * Simple double linked list implementation
 *
 * This implementation is not thread safe, it is up to the user to make
 * sure that concurrent accesses to the same list are safe. Also, the
 * implementation does minimal checks regarding null pointers. It's also
 * up to the user to deal with it...
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 */
 */
#ifndef	LIST_H
#define LIST_H

#include <assert.h>
#include "common.h"

struct list {
	struct list *next, *prev;
};

#define list_declare(name)	\
	struct list name = {&name, &name}

/*
 * @param head list
 *
 * @brief Initialize a list
 */
static inline void list_init(struct list *head)
{
	assert(head != NULL);
	head->next = head;
	head->prev = head;
}
/*
 * @param new New element in the list
 * @param prev Previous element in the list
 * @param next Next element in the list
 *
 * @brief Insert an element between prev and next
 */
static inline void list_add(struct list *new, struct list *prev,
			      struct list *next)
{
	assert(next != NULL && prev != NULL);
	new->next = next;
	new->prev = prev;
	next->prev = new;
	prev->next = new;
}
/*
 * @param next Next element in the list
 * @param prev Previous element in the list
 *
 * @Brief Delete the element between prev and next
 *	  Meant for internal use
 */
static inline void __list_del(struct list *next,
			      struct list *prev)
{
	assert(next != NULL && prev != NULL);
	prev->next = next;
	next->prev = prev;
}
/*
 * @param new New element in the list
 * @param head List head
 *
 * @brief Add an element after @head.
 *	  Usefull for stacks (LIFO)
 */
static inline void list_add_head(struct list *new,
				 struct list *head)
{
	assert(new != NULL && head != NULL);
	list_add(new, head, head->next);
}
/*
 * @param new New element in the list
 * @param head List head
 *
 * @brief Add an element before @head.
 *	  Usefull for queues (FIFO)
 */
static inline void list_add_tail(struct list *new,
				 struct list *head)
{
	assert(new != NULL && head != NULL);
	list_add(new, head->prev, head);
}
/*
 * @param head List head
 *
 * @return int 0 if not empty, 1 otherwise
 */
static inline int list_empty(struct list *head)
{
	assert(head != NULL);
	return (head == head->next);
}
/*
 * @param entry List element
 *
 * @brief Delete @entry from the list
 */
static inline void list_del(struct list *entry)
{
	assert(entry != NULL);
	__list_del(entry->next, entry->prev);
	entry->next = NULL;
	entry->prev = NULL;
}
/*
 * @param new New list element
 * @param old Old list element
 *
 * @brief Replace old with new
 */
static inline void list_replace(struct list *new,
				struct list *old)
{
	assert(new != NULL && old != NULL);
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}
/*
 * @param entry List entry
 * @param head List head
 *
 * @brief Move entry to the first element of @head
 *        This function is meant for moving entries from one list to another,
 *        although it can be used on the same list
 */
static inline void list_move_head(struct list *entry,
				  struct list *head)
{
	list_del(entry);
	list_add_head(entry, head);
}
/*
 * @param entry List entry
 * @param head List head
 *
 * @brief Move entry to the last element of @head
 *        This function is meant for moving entries from one list to another,
 *        although it can be used on the same list
 */
static inline void list_move_tail(struct list *entry,
				  struct list *head)
{
	list_del(entry);
	list_add_tail(entry, head);
}
/*
 * @param list List head
 * @param prev Previous element
 * @param next Next element
 *
 * @brief Add the complete @list between @prev and @next
 */
static inline void __list_splice(struct list *list, struct list *prev,
				 struct list *next)
{
	assert(list != NULL && prev != NULL && next != NULL);
	prev->next = list->next;
	list->next->prev = prev;

	next->prev = list->prev;
	list->prev->next = next;
}
/*
 * @param list List head
 * @param head New List
 *
 * @brief Add @list to the begining of @head concatenating both lists.
 *	  After the operation @list is re-initialized
 */
static inline void list_splice_head(struct list *list,
				    struct list *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
	}
	/*re-initialize*/
	list_init(list);
}
/*
 * @param list List head
 * @param head New List head
 *
 * @brief Add @list to the end of @head concatenating both lists.
 *	  After the operation @list is re-initialized
 */
static inline void list_splice_tail(struct list *list,
				    struct list *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head->prev, head);
	}
	/*re-initialize*/
	list_init(list);
}

/* @pos Ptr to the list node embedded in the struct
 * @type The type of the struct this is embedded in
 * @member The name of the struct list within the struct
 *
 * Retrieve the struct of this entry
 */
#define list_entry(pos, type, member)		\
	container_of(pos, type, member)

/* @entry Ptr to the struct object
 * @member The name of the struct list within the struct
 *
 * Retrieve the next struct object given the current entry
 */
#define list_entry_next(entry, member)	\
	list_entry((entry)->member.next, typeof(*(entry)), member)

/* @entry Ptr to the struct object
 * @member The name of the struct list within the struct
 *
 * Retrieve the previous struct object given the current entry
 */
#define list_entry_prev(entry, member)	\
	list_entry((entry)->member.prev, typeof(*(entry)), member)

/* @head Pointer to the list head
 * @type The type of the struct this is embedded in
 * @member The name of the struct list within the struct
 *
 * Retrieve the first struct entry of this list
 */
#define list_first_entry(head, type, member)	\
	list_entry((head)->next, type, member)

/* @head Pointer to the list head
 * @type The type of the struct this is embedded in
 * @member The name of the struct list within the struct
 *
 * Retrieve the last struct entry of this list
 */
#define list_last_entry(head, type, member)	\
	list_entry((head)->prev, type, member)

/* @pos Ptr to the list node embedded in the struct
 * @head Ptr to the list head
 *
 * Iterate over the list given by head
 */
#define list_for_each(pos, head)	\
	for (pos = (head)->next; pos != (head); pos = pos->next)

/* @pos Ptr to the list node embedded in the struct
 * @safe Ptr to another list node used as temporary storage
 * @head Ptr to the list head
 *
 * Iterate over the list given by head in a safe mode so that,
 * one can delete list member's in each iteration
 */
#define list_for_each_safe(pos, safe, head)	\
	for (pos = (head)->next, safe = pos->next; pos != (head); \
		pos = safe, safe = pos->next)

/* @pos Ptr to the list node embedded in the struct
 * @head Ptr to the list head
 *
 * Same as @list_for_each in the reverse direction
 */
#define list_for_each_prev(pos, head)	\
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/* @pos Ptr to the list node embedded in the struct
 * @safe Ptr to another list node used as temporary storage
 * @head Ptr to the list head
 *
 * Same as @list_for_each_safe but in the reverse direction
 */
#define list_for_each_prev_safe(pos, safe, head)	\
	for (pos = (head)->prev, safe = pos->prev; pos != (head); \
		pos = safe, safe = pos->prev)

/* @pos Ptr to the list node embedded in the struct
 * @head Ptr to the list head
 *
 * Iterate over the list starting on the current @pos
 */
#define list_for_each_from(pos, head)	\
	for (; pos != (head); pos = pos->next)

/* @pos Ptr to the list node embedded in the struct
 * @safe Ptr to another list node used as temporary storage
 * @head Ptr to the list head
 *
 * Iterate over the list starting on the current @pos in a safe mode so that,
 * one can delete list member's in each iteration
 */
#define list_for_each_from_safe(pos, safe, head)	\
	for (safe = pos->next; pos != (head); pos = safe, safe = pos->next)

/* @pos Ptr to the list node embedded in the struct
 * @head Ptr to the list head
 *
 * Same as @list_for_each_from in the reverse direction
 */
#define list_for_each_from_reverse(pos, head)	\
	for (; pos != (head); pos = pos->prev)

/* @pos Ptr to the list node embedded in the struct
 * @safe Ptr to another list node used as temporary storage
 * @head Ptr to the list head
 *
 * Same as @list_for_each_from_safe in the reverse direction
 */
#define list_for_each_from_reverse_safe(pos, safe, head)	\
	for (safe = pos->prev; pos != (head); pos = safe, safe = pos->prev)

/* @entry Ptr to the struct object
 * @head Ptr to the list head
 * @member The name of the struct list within the struct
 *
 * Iterate over the list given by head retrieving the actual struct object
 */
#define list_for_each_entry(entry, head, member)	\
	for (entry = list_first_entry(head, typeof(*entry), member); \
		&entry->member != head; entry = list_entry_next(entry, member))

/* @entry Ptr to the struct object
 * @safe Ptr to another struct object used as temporary storage
 * @head Ptr to the list head
 * @member The name of the struct list within the struct
 *
 * Iterate over the list given by head retrieving the actual struct object
 * in a safe mode so that, one can delete list member's in each iteration
 */
#define list_for_each_entry_safe(entry, safe, head, member)	\
	for (entry = list_first_entry(head, typeof(*entry), member), \
	    safe = list_entry_next(entry, member); &entry->member != head; \
	    entry = safe, safe = list_entry_next(entry, member))

/* @entry Ptr to the struct object
 * @head Ptr to the list head
 * @member The name of the struct list within the struct
 *
 * Same as @list_for_each_entry in the reverse direction
 */
#define list_for_each_entry_prev(entry, head, member)	\
	for (entry = list_last_entry(head, typeof(*entry), member); \
		&entry->member != head; entry = list_entry_prev(entry, member))

/* @entry Ptr to the struct object
 * @safe Ptr to another struct object used as temporary storage
 * @head Ptr to the list head
 * @member The name of the struct list within the struct
 *
 * Same as @list_for_each_entry_safe in the reverse direction
 */
#define list_for_each_entry_prev_safe(entry, safe, head, member)	\
	for (entry = list_last_entry(head, typeof(*entry), member), \
	    safe = list_entry_prev(entry, member); &entry->member != head; \
	    entry = safe, safe = list_entry_prev(entry, member))

/* @entry Ptr to the struct object
 * @head Ptr to the list head
 * @member The name of the struct list within the struct
 *
 * Iterate over the list starting on the current @entry
 */
#define list_for_each_entry_from(entry, head, member)	\
	for (; &entry->member != head; entry = list_entry_next(entry, member))

/* @entry Ptr to the struct object
 * @safe Ptr to another struct object used as temporary storage
 * @head Ptr to the list head
 * @member The name of the struct list within the struct
 *
 * Iterate over the list starting on the current @entry in a safe mode so that,
 * one can delete list member's in each iteration
 */
#define list_for_each_entry_from_safe(entry, safe, head, member)	\
	for (safe = list_entry_next(entry, member); &(entry)->member != head; \
	     entry = safe, safe = list_entry_next(entry, member))

/* @entry Ptr to the struct object
 * @head Ptr to the list head
 * @member The name of the struct list within the struct
 *
 * Same as @list_for_each_entry_from in the reverse direction
 */
#define list_for_each_entry_from_reverse(entry, head, member)	\
	for (; &entry->member != head; entry = list_entry_prev(entry, member))

/* @entry Ptr to the struct object
 * @safe Ptr to another struct object used as temporary storage
 * @head Ptr to the list head
 * @member The name of the struct list within the struct
 *
 * Same as @list_for_each_entry_from_safe in the reverse direction
 */
#define list_for_each_entry_from_reverse_safe(entry, safe, head, member) \
	for (safe = list_entry_prev(entry, member); &entry->member != head; \
	     entry = safe, safe = list_entry_prev(entry, member))
#endif
