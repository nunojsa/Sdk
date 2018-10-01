#include <stdio.h>
#include "list.h"

struct list_tester {
	char *name;
	struct list node;
};

list_declare(list);
list_declare(list_2);

struct list_tester test[] = {
	{
		.name = "list_0",
	},
	{
		.name = "list_1",
	},
	{
		.name = "list_2",
	},
	{
		.name = "list_3",
	},
	{
		.name = "list_4",
	},
};

struct list_tester test_2[] = {
	{
		.name = "new_0",
	},
	{
		.name = "new_1",
	},
	{
		.name = "new_2",
	},
	{
		.name = "new_3",
	},
	{
		.name = "new_4",
	},
};

/*testing the API playing around with 2 lists...*/
static void list_test_1(void)
{
	u32 i = 0;
	struct list *pos, *safe;

	for (i = 0; i < ARRAY_SIZE(test); i++) {
		if (i > 2) {
			list_add_head(&test[i].node, &list);
		} else {
			list_add_tail(&test[i].node, &list);
		}
	}

	for (i = 0; i < ARRAY_SIZE(test_2); i++) {
		list_add_tail(&test_2[i].node, &list_2);
	}

	list_move_tail(&test[4].node, &list_2);

	list_for_each_safe(pos, safe, &list) {
		struct list_tester *cur = (struct list_tester *)
			list_entry(pos, struct list_tester, node);

		printf("List name \"%s\"\n", cur->name);
		if (pos->next == &list) {
			list_del(&test_2[0].node);
			list_replace(&test_2[0].node, pos);
		}
	}

	list_move_head(&test[0].node, &list_2);

	list_for_each_prev(pos, &list) {
		struct list_tester *cur = (struct list_tester *)
			list_entry(pos, struct list_tester, node);

		printf("After List name \"%s\"\n", cur->name);
	}

	list_for_each(pos, &list_2) {
		struct list_tester *cur = (struct list_tester *)
			list_entry(pos, struct list_tester, node);

		printf("List_2 name \"%s\"\n", cur->name);
	}

	list_splice_tail(&list, &list_2);

	list_add(&test[2].node, &test[3].node, &test[1].node);

	list_for_each_prev_safe(pos, safe, &list_2) {
		struct list_tester *cur = (struct list_tester *)
				list_entry(pos, struct list_tester, node);

		printf("List_2 after splice name \"%s\"\n", cur->name);
		list_del(pos);
	}
}
/*testing the XXX_entry_XXX macros playing around with 2 lists...*/
static void list_test_2(void)
{
	u32 i = 0;
	struct list_tester *pos, *safe;
	struct list_tester *cur = &test[2];

	for (i = 0; i < ARRAY_SIZE(test_2); i++) {
		list_add_tail(&test_2[i].node, &list_2);
	}

	list_for_each_entry(pos, &list_2, node) {
		printf("Entry in list_2: %s\n", pos->name);
	}

	list_for_each_entry_prev(pos, &list_2, node) {
		printf("Entry in list_2: %s\n", pos->name);
	}

	list_for_each_entry_safe(pos, safe, &list_2, node) {
		printf("Deleting...%s\n", pos->name);
		list_del(&pos->node);
	}

	for (i = 0; i < ARRAY_SIZE(test); i++) {
		list_add_head(&test[i].node, &list);
	}

	list_for_each_entry_from(cur, &list, node) {
		printf("Entry in list: %s\n", cur->name);
	}

	cur = &test[2];

	list_for_each_entry_from_reverse_safe(cur, safe, &list, node) {
		printf("Deleting Entry in list: %s\n", cur->name);
		list_del(&cur->node);
	}

	cur = &test[0];

	list_for_each_entry_from_safe(cur, safe, &list, node) {
		printf("Deleting Entry in list: %s\n", cur->name);
		list_del(&cur->node);
	}

	/*delete remaining entries...*/
	list_for_each_entry_prev_safe(pos, safe, &list, node) {
		printf("Deleting last entry...%s\n", pos->name);
		list_del(&pos->node);
	}
}


int main(void)
{
	list_test_1();

	if (!list_empty(&list) || !list_empty(&list_2)) {
		printf("Lists should be empty...\n");
		list_init(&list);
		list_init(&list_2);
	}

	list_test_2();

	return 0;
}
