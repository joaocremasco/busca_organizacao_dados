#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stddef.h>

typedef struct item_ ITEM;
typedef struct item_store ITEM_STORE;

/* ITEM_STORE owns its items. Collections only borrow ITEM pointers. */
ITEM_STORE *item_store_create(void);
void item_store_destroy(ITEM_STORE **store);
ITEM *item_create(ITEM_STORE *store, size_t field_capacity) ;
bool item_set(ITEM *item, const char *name, const char *raw_value);
const char *item_get(const ITEM *item, const char *name);
bool item_get_number(const ITEM *item, const char *name, double *value);
size_t item_field_count(const ITEM *item);
const char *item_field_name(const ITEM *item, size_t index);
const char *item_field_value(const ITEM *item, size_t index);

#endif
