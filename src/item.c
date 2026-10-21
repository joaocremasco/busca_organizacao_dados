#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
    (void)store; /* só para evitar warning */

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
    int new_cap = item->capacity == 0 ? 4 : item->capacity * 2;
    FIELD *temp = realloc(item->fields, new_cap * sizeof(FIELD));
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


