#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../include/item.h"


typedef struct {
  char *name;
  char *value;
} FIELD;

struct item_ {
  FIELD *fields;    /* vetor de pares */
  size_t count;     /* pares em uso */
  size_t capacity;  /* pares que cabem sem realloc */
};

struct item_store {
  ITEM **items;     /* vetor de ponteiros para itens */
  size_t count;     /* quantos itens foram registrados */
  size_t capacity;  /* quantos cabem sem realloc */
};

static char *copy_string(const char *s)
{
  size_t n = strlen(s) + 1;          /* +1 para o '\0' */
  char *p = malloc(n);
  if (p != NULL)
    memcpy(p, s, n);
  return p;
}

ITEM *item_create(ITEM_STORE *store, size_t field_capacity)
{
  ITEM *item = malloc(sizeof(*item));
  if (item == NULL)
    return NULL;

  item->count = 0;
  item->fields = NULL;
  item->capacity = 0;

  if (field_capacity > 0) {
    item->fields = malloc(sizeof(*item->fields) * field_capacity);
    if (item->fields == NULL) {
      free(item);
      return NULL;
    }
    item->capacity = field_capacity;
  }

  if(!store_add(store, item)){
    free(item);
    return NULL;
  }

  return item;

  /*if (field_capacity > 0) {
    item->fields = malloc(sizeof(*item->fields) * field_capacity);
    if (item->fields == NULL) {
      free(item); 
      return NULL; 
    }
    item->capacity = field_capacity;
} else {
    item->fields = NULL;
    item->capacity = 0;
}
Ver qual mais te agrada em questão de legibilidade - prefiro a opção 1*/
}

bool item_set(ITEM *item, const char *name, const char *raw_value){
  if (item == NULL || name == NULL || raw_value == NULL)
    return false;

  if(item->count == item->capacity){
    size_t new_cap = item->capacity == 0 ? 4 : item->capacity * 2;
    FIELD *temp = realloc(item->fields, new_cap * sizeof(*temp));
    if(temp == NULL)
      return false;
    item->fields = temp;
    item->capacity = new_cap;
  }

  char *n = copy_string(name);
  char *v = copy_string(raw_value);
  if (n == NULL || v == NULL) {
    free(n);
    free(v);
    return false;
  }

  item->fields[item->count].name = n;
  item->fields[item->count].value = v;
  item->count++;

  /*item->fields[item->count].name = copy_string(name);
    if (item->fields[item->count].name == NULL)
      return false;

    item->fields[item->count].value = copy_string(raw_value);
    if (item->fields[item->count].value == NULL) {
      free(item->fields[item->count].name);
    return false;
    }

    item->count++; a definir qual é mais legível */

  return true;
}

const char *item_get(const ITEM *item, const char *name){
  if(item == NULL || name == NULL)
    return NULL;

  for (int i = 0; i < item->count; i++) {
    if(strcmp(item->fields[i].name, name) == 0)
      return item->fields[i].value;
  }
  return NULL;
}

bool item_get_number(const ITEM *item, const char *name, double *value){
  if(value == NULL)
    return false;

  const char *texto = item_get(item, name);
  if(texto == NULL)
    return false;

  char *fim;
  double x = strtod(texto, &fim);
  if (fim == texto)
    return false;            /* nada foi convertido */

  while (isspace((unsigned char)*fim))
    fim++;

  if (*fim != '\0')
    return false;      /* sobrou lixo */

  *value = x;
  return true;
}

size_t item_field_count(const ITEM *item){
  return item == NULL ? 0 : item->count;
}

const char *item_field_name(const ITEM *item, size_t index){
  return (item == NULL || index >= item->count) ? NULL : item->fields[index].name;
}
const char *item_field_value(const ITEM *item, size_t index){
  return (item == NULL || index >= item->count) ? NULL : item->fields[index].value;
}

static bool store_add(ITEM_STORE *store, ITEM *item){
  if(store == NULL || item == NULL)
    return false;

  if(store->count == store->capacity){
    size_t new_cap = store->capacity == 0 ? 4 : store->capacity * 2;
    ITEM **temp = realloc(store->items, new_cap * sizeof(*temp));
    if(temp == NULL)
      return false;
    store->items = temp;
    store->capacity = new_cap;
  }
  store->items[store->count] = item;
  store->count++;

  return true;
}

ITEM_STORE *item_store_create(void){
  ITEM_STORE *store = malloc(sizeof(*store));
  if(store == NULL)
    return NULL;

  store->count = 0;
  store->items = NULL;
  store->capacity = 0;

  return store;
}

void item_store_destroy(ITEM_STORE **store)
{
  if (store == NULL || *store == NULL)
    return;

  for (size_t i = 0; i < (*store)->count; i++) {
    ITEM *item = (*store)->items[i];

    for (size_t j = 0; j < item->count; j++) {
      free(item->fields[j].name);
      free(item->fields[j].value);
    }

    free(item->fields);
    free(item);
  }

  free((*store)->items);
  free(*store);
  *store = NULL;
}
