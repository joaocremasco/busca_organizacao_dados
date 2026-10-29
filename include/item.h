#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stddef.h>

typedef struct item_ ITEM;
typedef struct item_store ITEM_STORE;

// o store e dono dos itens, as colecoes so guardam ponteiro
ITEM_STORE *item_store_create(void);
// junto leva todos os itens
void item_store_destroy(ITEM_STORE **store);
// field_capacity e so o tamanho inicial, item_set cresce se precisar
ITEM *item_create(ITEM_STORE *store, size_t field_capacity) ;
// copia o nome e o valor
bool item_set(ITEM *item, const char *name, const char *raw_value);
// NULL se o campo nao existir
const char *item_get(const ITEM *item, const char *name);
// false se o campo nao existir ou nao for numero
bool item_get_number(const ITEM *item, const char *name, double *value);
size_t item_field_count(const ITEM *item);
const char *item_field_name(const ITEM *item, size_t index);
const char *item_field_value(const ITEM *item, size_t index);

#endif
