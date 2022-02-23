#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>

#include "util.h"

typedef struct list list;
typedef struct list_node list_node;

struct list {
	int size;
	list_node *head;
	list_node *tail;
	void (*destructor)(void *);
	int (*compare)(void *, void *);
};

struct list_node {
	void *item;
	list_node *next;
};

/*
 * list_init()
 *
 * Takes an ALREADY allocated list structure, and initializes its methods to point to
 * argument passed functions.
 */
extern void list_init(list *x, void (*destructor)(void *), int (*compare)(void *, void *));

/*list_delete()
 *
 * deletes all nodes in the list and their contents.  Does NOT free the list pointer argument.
 */
extern void list_delete(list *x);
extern list_node *list_append(list *x, void *item);
extern void *list_search(list *x, void *item);

#endif /* _LIST_H_ */