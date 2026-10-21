#ifndef TRABALHO_COLLECTION_H
#define TRABALHO_COLLECTION_H

#include "trabalho/item.h"
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    COLLECTION_STACK, COLLECTION_QUEUE, COLLECTION_ARRAY_LIST, COLLECTION_LINKED_LIST,
    COLLECTION_SENTINEL_LIST, COLLECTION_SORTED_LIST, COLLECTION_GROUPED_LIST,
    COLLECTION_CROSS_LIST
} COLLECTION_TYPE;

typedef struct collection COLLECTION;
typedef bool (*COLLECTION_VISITOR)(ITEM *item, void *context);

/* Options are borrowed strings and must outlive the collection. */
typedef struct {
    const char *order_field;
    bool descending;
    const char *group_field;
    const char *subgroup_field;
    const char *cross_field;
    const char *cross_second_field;
} COLLECTION_OPTIONS;

COLLECTION *collection_create(COLLECTION_TYPE type, const COLLECTION_OPTIONS *options);
void collection_destroy(COLLECTION **collection);
bool collection_insert(COLLECTION *collection, ITEM *item);
bool collection_remove(COLLECTION *collection, ITEM *item);
size_t collection_size(const COLLECTION *collection);
bool collection_visit(const COLLECTION *collection, COLLECTION_VISITOR visitor, void *context);
bool collection_visit_cross_index(const COLLECTION *collection, bool second_index,
                                  COLLECTION_VISITOR visitor, void *context);
bool collection_type_parse(const char *name, COLLECTION_TYPE *type);
const char *collection_type_name(COLLECTION_TYPE type);

#endif
