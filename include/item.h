#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stddef.h>

typedef struct item_ ITEM;
typedef struct item_store ITEM_STORE;

// o armazenamento e dono dos itens e as colecoes guardam referencias
ITEM_STORE *item_store_create(void);
// libera todos os itens registrados
void item_store_destroy(ITEM_STORE **store);
// a capacidade inicial cresce quando faltam campos
ITEM *item_create(ITEM_STORE *store, size_t field_capacity) ;
// copia o nome e o valor para o item
bool item_set(ITEM *item, const char *name, const char *raw_value);
// devolve nulo quando o campo nao existe
const char *item_get(const ITEM *item, const char *name);
// falha se o campo nao existir ou nao for numero
bool item_get_number(const ITEM *item, const char *name, double *value);
size_t item_field_count(const ITEM *item);
const char *item_field_name(const ITEM *item, size_t index);
const char *item_field_value(const ITEM *item, size_t index);

#endif
