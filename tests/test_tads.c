#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include "crossed_list.h"
#include "dn_linked_list.h"
#include "gen_list.h"
#include "linked_list.h"
#include "ord_list.h"
#include "queue.h"
#include "seq_list.h"
#include "stack.h"
#include "collection.h"
#include "item.h"

static void *v(int n) { return (void *)(intptr_t)n; }
static int cmp(void *a, void *b) { return (intptr_t)a - (intptr_t)b; }
static bool visit_count(void *object, void *context)
{
    assert(object != NULL);
    (*(int *)context)++;
    return true;
}
typedef struct { const int *values; size_t count; size_t index; } ORDER_CHECK;
static bool visit_order(void *object, void *context)
{
    ORDER_CHECK *check = context;
    assert(check->index < check->count);
    assert((int)(intptr_t)object == check->values[check->index++]);
    return true;
}
static bool visit_cross(int i, int j, void *object, void *context)
{
    assert(i >= 0 && j >= 0 && object != NULL);
    (*(int *)context)++;
    return true;
}
static bool visit_cross_order(int i, int j, void *object, void *context)
{
    (void)i;
    (void)j;
    return visit_order(object, context);
}
static bool visit_item(ITEM *item, void *context)
{
    assert(item != NULL);
    (*(int *)context)++;
    return true;
}
static bool visit_expected_item(ITEM *item, void *context)
{
    assert(item == context);
    return true;
}

static void test_seq(void)
{
    SEQ_LIST *x = seq_list_init(NULL);
    assert(x && seq_list_empty(x));
    for (int i = 1; i <= 12; ++i) assert(seq_list_push_back(x, v(i)));
    assert(seq_list_insert(x, v(99), 3));
    assert(seq_list_get(x, 3) == v(99));
    assert(seq_list_erase(x, 3) == v(99));
    assert(seq_list_find(x, v(7), cmp) == 6);
    assert(seq_list_resize(x, 5) && seq_list_size(x) == 5);
    int n = 0; assert(seq_list_visit(x, visit_count, &n) && n == 5);
    const int expected[] = {1, 2, 3, 4, 5};
    ORDER_CHECK order = {expected, 5, 0};
    assert(seq_list_visit(x, visit_order, &order) && order.index == 5);
    assert(seq_list_pop_back(x) == v(5));
    seq_list_destroy(&x); assert(!x);
}

static void test_ord(void)
{
    ORD_LIST *x = ord_list_init(NULL, cmp);
    assert(x);
    for (int i = 12; i >= 1; --i) assert(ord_list_insert(x, v(i)));
    assert(ord_list_get(x, 0) == v(1) && ord_list_get(x, 11) == v(12));
    assert(ord_list_lower_bound(x, v(5)) == 4);
    assert(ord_list_upper_bound(x, v(5)) == 5);
    assert(ord_list_find(x, v(8)) == 7);
    assert(ord_list_erase(x, v(8)) == v(8));
    assert(ord_list_resize(x, 4) && ord_list_size(x) == 4);
    int n = 0; assert(ord_list_visit(x, visit_count, &n) && n == 4);
    const int expected[] = {1, 2, 3, 4};
    ORDER_CHECK order = {expected, 4, 0};
    assert(ord_list_visit(x, visit_order, &order) && order.index == 4);
    ord_list_destroy(&x); assert(!x);
}

static void test_cross(void)
{
    CROSSED_LIST *x = crossed_list_init(NULL);
    assert(x && crossed_list_resize(x, 4, true) && crossed_list_resize(x, 5, false));
    assert(crossed_list_insert(x, 2, 3, v(23)));
    assert(crossed_list_insert(x, 0, 3, v(3)));
    assert(crossed_list_insert(x, 2, 1, v(21)));
    assert(!crossed_list_insert(x, 2, 3, v(99)));
    assert(crossed_list_get(x, 2, 1) == v(21));
    int i = -1, j = -1;
    assert(crossed_list_find(x, v(23), cmp, &i, &j) == v(23) && i == 2 && j == 3);
    int n = 0; assert(crossed_list_visit_line(x, 3, false, visit_cross, &n) && n == 2);
    const int expected[] = {3, 21, 23};
    ORDER_CHECK order = {expected, 3, 0};
    assert(crossed_list_visit(x, visit_cross_order, &order) && order.index == 3);
    assert(crossed_list_remove(x, 2, 1) == v(21));
    assert(crossed_list_resize(x, 2, true) && crossed_list_size(x) == 1);
    crossed_list_destroy(&x); assert(!x);
}

static void test_gen(void)
{
    GEN_LIST *x = gen_list_init(NULL), *sub = gen_list_init(NULL);
    bool atom = false;
    assert(x && sub && gen_list_push_back(sub, v(2), true));
    assert(gen_list_push_back(x, sub, false));
    assert(gen_list_push_front(x, v(1), true));
    assert(gen_list_insert(x, v(3), true, sub, false, cmp));
    assert(gen_list_size(x) == 3);
    int n = 0; assert(gen_list_visit(x, visit_count, &n) && n == 3);
    const int expected[] = {1, 3, 2};
    ORDER_CHECK order = {expected, 3, 0};
    assert(gen_list_visit(x, visit_order, &order) && order.index == 3);
    assert(gen_list_find(x, sub, false, cmp, &atom) == sub && !atom);
    assert(gen_list_set(x, v(4), true, v(3), true, cmp, &atom) == v(3) && atom);
    assert(gen_list_erase(x, v(4), true, cmp, &atom) == v(4) && atom);
    assert(gen_list_pop_front(x, &atom) == v(1) && atom);
    gen_list_destroy(&x); assert(!x);
}

#define TEST_LINKED(name, type) \
static void test_##name(void) { \
    type *x = name##_init(NULL); assert(x && name##_empty(x)); \
    assert(name##_push_back(x, v(2)) && name##_push_front(x, v(1))); \
    assert(name##_front(x) == v(1) && name##_back(x) == v(2)); \
    assert(name##_insert(x, v(3), v(2), cmp)); \
    assert(name##_find(x, v(3), cmp) == v(3)); \
    assert(name##_set(x, v(4), v(3), cmp) == v(3)); \
    int n = 0; assert(name##_visit(x, visit_count, &n) && n == 3); \
    const int expected[] = {1, 4, 2}; \
    ORDER_CHECK order = {expected, 3, 0}; \
    assert(name##_visit(x, visit_order, &order) && order.index == 3); \
    assert(name##_erase(x, v(4), cmp) == v(4)); \
    assert(name##_pop_back(x) == v(2) && name##_pop_front(x) == v(1)); \
    assert(name##_empty(x)); name##_destroy(&x); assert(!x); }

TEST_LINKED(linked_list, LINKED_LIST)
TEST_LINKED(dn_linked_list, DN_LINKED_LIST)

static void test_queue(void)
{
    QUEUE *x = queue_init(NULL); assert(x && queue_empty(x));
    assert(queue_push(x, v(1)) && queue_push(x, v(2)));
    assert(queue_front(x) == v(1) && queue_back(x) == v(2));
    int n = 0; assert(queue_visit(x, visit_count, &n) && n == 2);
    const int expected[] = {1, 2};
    ORDER_CHECK order = {expected, 2, 0};
    assert(queue_visit(x, visit_order, &order) && order.index == 2);
    assert(queue_pop(x) == v(1) && queue_pop(x) == v(2) && queue_empty(x));
    queue_destroy(&x); assert(!x);
}

static void test_stack(void)
{
    STACK *x = stack_init(NULL); assert(x && stack_empty(x));
    assert(stack_push(x, v(1)) && stack_push(x, v(2)) && stack_top(x) == v(2));
    int n = 0; assert(stack_visit(x, visit_count, &n) && n == 2);
    const int expected[] = {2, 1};
    ORDER_CHECK order = {expected, 2, 0};
    assert(stack_visit(x, visit_order, &order) && order.index == 2);
    assert(stack_pop(x) == v(2) && stack_pop(x) == v(1) && stack_empty(x));
    stack_destroy(&x); assert(!x);
}

static void test_collection_cross_cells(void)
{
    ITEM_STORE *store = item_store_create();
    COLLECTION_OPTIONS options = {0};
    COLLECTION *collection;
    ITEM *a, *b, *c;
    int visited = 0;
    options.cross_field = "group";
    options.cross_second_field = "year";
    collection = collection_create(COLLECTION_CROSS_LIST, &options);
    assert(store && collection);
    a = item_create(store, 2); b = item_create(store, 2); c = item_create(store, 2);
    assert(a && b && c);
    assert(item_set(a, "group", "near") && item_set(a, "year", "2020"));
    assert(item_set(b, "group", "near") && item_set(b, "year", "2020"));
    assert(item_set(c, "group", "far") && item_set(c, "year", "2021"));
    assert(collection_insert(collection, a));
    assert(collection_insert(collection, b));
    assert(collection_insert(collection, c));
    assert(collection_size(collection) == 3);
    assert(collection_visit(collection, visit_item, &visited) && visited == 3);
    assert(collection_remove(collection, b) && collection_size(collection) == 2);
    visited = 0;
    assert(collection_visit_cross_index(collection, false, visit_item, &visited));
    assert(visited == 2);
    assert(collection_remove(collection, a) && collection_remove(collection, c));
    assert(collection_size(collection) == 0);
    collection_destroy(&collection);

    options.order_field = "order";
    collection = collection_create(COLLECTION_SORTED_LIST, &options);
    assert(collection);
    a = item_create(store, 1); b = item_create(store, 1);
    assert(a && b && item_set(a, "order", "same") && item_set(b, "order", "same"));
    assert(collection_insert(collection, a) && collection_insert(collection, b));
    assert(collection_remove(collection, b));
    assert(collection_size(collection) == 1);
    assert(collection_visit(collection, visit_expected_item, a));
    collection_destroy(&collection);
    item_store_destroy(&store);
}

int main(void)
{
    test_seq(); test_ord(); test_cross(); test_gen();
    test_linked_list(); test_dn_linked_list(); test_queue(); test_stack();
    test_collection_cross_cells();
    return 0;
}
