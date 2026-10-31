#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stddef.h>

typedef struct item_ ITEM;
typedef struct item_store ITEM_STORE;

// O ITEM_STORE é dono dos itens; as coleções guardam apenas referências.
ITEM_STORE *item_store_create(void);
// Libera todos os itens e seus campos registrados no armazenamento.
void item_store_destroy(ITEM_STORE **store);
// field_capacity define a capacidade inicial do vetor de campos.
ITEM *item_create(ITEM_STORE *store, size_t field_capacity) ;
// Copia name e raw_value para o ITEM.
bool item_set(ITEM *item, const char *name, const char *raw_value);
// Devolve NULL se o campo não existir.
const char *item_get(const ITEM *item, const char *name);
// Devolve false se o campo não existir ou não representar um número completo.
bool item_get_number(const ITEM *item, const char *name, double *value);
size_t item_field_count(const ITEM *item);
const char *item_field_name(const ITEM *item, size_t index);
const char *item_field_value(const ITEM *item, size_t index);

#endif
