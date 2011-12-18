/**
 * @file list.c Linked-lists Testcode
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <string.h>
#include <re.h>
#include "test.h"


#define DEBUG_MODULE "testlist"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


struct node {
	struct le le;
};

int test_list(void)
{
	struct node node1, node2;
	struct list list;
	int err = EINVAL;

	list_init(&list);

	memset(&node1, 0, sizeof(node1));
	memset(&node2, 0, sizeof(node2));

	DEBUG_INFO("test_list\n");

	/* Test empty list */
	if (0 != list_count(&list)) {
		DEBUG_WARNING("test_list: failed line %u\n", __LINE__);
		goto out;
	}

	/* Test with one node */
	list_append(&list, &node1.le, &node1);
	if (1 != list_count(&list)) {
		DEBUG_WARNING("test_list: failed line %u\n", __LINE__);
		goto out;
	}

	list_unlink(&node1.le);

	if (0 != list_count(&list)) {
		DEBUG_WARNING("test_list: failed line %u\n", __LINE__);
		goto out;
	}

	/* Test with two nodes */
	list_append(&list, &node1.le, &node1);
	if (1 != list_count(&list)) {
		DEBUG_WARNING("test_list: failed line %u\n", __LINE__);
		goto out;
	}

	list_append(&list, &node2.le, &node2);
	if (2 != list_count(&list)) {
		DEBUG_WARNING("test_list: failed line %u\n", __LINE__);
		goto out;
	}

	list_unlink(&node1.le);

	if (1 != list_count(&list)) {
		DEBUG_WARNING("test_list: failed line %u\n", __LINE__);
		goto out;
	}

	list_unlink(&node2.le);

	/* Test empty list */
	if (0 != list_count(&list)) {
		DEBUG_WARNING("test_list: failed line %u\n", __LINE__);
		goto out;
	}

	err = 0;

 out:
	if (err) {
		DEBUG_WARNING("test_list failed (%s)\n", strerror(err));
	}

	return err;
}


static void node_destructor(void *arg)
{
	struct node *node = arg;

	if (node->le.prev || node->le.next || node->le.list || node->le.data) {
		DEBUG_WARNING("le: prev=%p next=%p data=%p\n",
			      node->le.prev, node->le.next, node->le.data);
	}

	list_unlink(&node->le);
}


/**
 * Test linked list with external reference to objects
 */
int test_list_ref(void)
{
	struct list list;
	struct node *node, *node2;
	int err = 0;

	list_init(&list);

	node = mem_zalloc(sizeof(*node), node_destructor);
	node2 = mem_zalloc(sizeof(*node2), node_destructor);
	if (!node || !node2) {
		err = ENOMEM;
		goto out;
	}

	mem_ref(node);

	list_append(&list, &node->le, node);
	list_append(&list, &node2->le, node2);

 out:
	list_flush(&list);
	memset(&list, 0xa5, sizeof(list));  /* mark as deleted */

	/* note: done after list_flush() */
	mem_deref(node);

	return err;
}
