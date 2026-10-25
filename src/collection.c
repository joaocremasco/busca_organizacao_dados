
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "data_structures/stack.h"
#include "data_structures/seq_list.h"
#include "data_structures/queue.h"
#include "data_structures/ord_list.h"
#include "data_structures/linked_list.h"
#include "data_structures/gen_list.h"
#include "data_structures/dn_linked_list.h"

typedef enum {
  COLLECTION_STACK,
  COLLECTION_QUEUE,
  COLLECTION_ARRAY_LIST,
  COLLECTION_LINKED_LIST,
  COLLECTION_SENTINEL_LIST,
  COLLECTION_SORTED_LIST,
  COLLECTION_GROUPED_LIST,
  COLLECTION_CROSS_LIST
} COLLECTION_TYPE;

typedef struct collection COLLECTION;

typedef bool (*COLLECTION_VISITOR)(ITEM *item, void *context);

struct collection {
  COLLECTION_TYPE type;
  void *impl;
};

typedef struct {
    const char *name;
    COLLECTION_TYPE type;
} TYPE_ENTRY;

static const TYPE_ENTRY type_table[] = {
    {"pilha", COLLECTION_STACK},
    {"fila", COLLECTION_QUEUE},
    {"lista_sequencial", COLLECTION_ARRAY_LIST},
    {"lista_encadeada", COLLECTION_LINKED_LIST},
    {"lista_sent(inela)", COLLECTION_SENTINEL_LIST},
    {"lista_ordenada", COLLECTION_SORTED_LIST},
    {"lista_agrupada", COLLECTION_GROUPED_LIST},
    {"lista_cruzada", COLLECTION_CROSS_LIST}
};

#define TYPE_COUNT (sizeof(type_table) / sizeof(type_table[0]))

COLLECTION *collection_create(COLLECTION_TYPE type,
                              const COLLECTION_OPTIONS *options) {
  COLLECTION *collection = malloc(sizeof(*collection));

  if (collection == NULL)
    return NULL;

  collection->type = type;
  collection->impl = NULL;

  switch (type) {
    case COLLECTION_STACK:
      collection->impl = stack_init();
      break;

    case COLLECTION_QUEUE:
      collection->impl = queue_init();
      break;

    case COLLECTION_ARRAY_LIST:
      collection->impl = seq_list_init();
      break;

    case COLLECTION_LINKED_LIST:
      collection->impl = linked_list_init();
      break;

    case COLLECTION_SORTED_LIST:
      collection->impl = ord_list_init();
      break;

    case COLLECTION_SENTINEL_LIST:
      collection->impl = dn_linked_list_init();
      break;

    case COLLECTION_CROSS_LIST:
      collection->impl = cross_list_init();
      break;

    case COLLECTION_GROUPED_LIST:
      /* Implementar quando houver suporte a essa estrutura. */
      free(collection);
      return NULL;

    default:
      free(collection);
      return NULL;
  }

  if (collection->impl == NULL) {
    free(collection);
    return NULL;
  }

  (void)options;
  return collection;
}

void collection_destroy(COLLECTION **collection) {
  if (collection == NULL || *collection == NULL)
    return;

  switch ((*collection)->type) {
    case COLLECTION_STACK:
      stack_clear((*collection)->impl);
      break;

    case COLLECTION_QUEUE:
      queue_clear((*collection)->impl);
      break;

    case COLLECTION_ARRAY_LIST:
      seq_list_clear((*collection)->impl);
      break;

    case COLLECTION_LINKED_LIST:
      linked_list_clear((*collection)->impl);
      break;

    case COLLECTION_SORTED_LIST:
      ord_list_clear((*collection)->impl);
      break;

    case COLLECTION_SENTINEL_LIST:
      dn_linked_list_clear((*collection)->impl);
      break;

    case COLLECTION_CROSS_LIST:
      cross_list_clear((*collection)->impl);
      break;

    case COLLECTION_GROUPED_LIST:
    default:
      break;
  }

  free((*collection)->impl);
  free(*collection);
  *collection = NULL;
}

bool collection_insert(COLLECTION *collection, ITEM *item) {
  if (collection == NULL || item == NULL)
    return false;

  switch (collection->type) {
    case COLLECTION_STACK:
      return stack_push(collection->impl, item);

    case COLLECTION_QUEUE:
      return queue_insert(collection->impl, item);

    case COLLECTION_ARRAY_LIST:
      return seq_list_insert(collection->impl, item);

    case COLLECTION_LINKED_LIST:
      return linked_list_insert(collection->impl, item);

    case COLLECTION_SORTED_LIST:
      return ord_list_insert(collection->impl, item);

    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_insert(collection->impl, item);

    case COLLECTION_CROSS_LIST:
      return cross_list_insert(collection->impl, item);

    case COLLECTION_GROUPED_LIST:
    default:
      return false;
  }
}

bool collection_remove(COLLECTION *collection, ITEM *item) {
  if (collection == NULL || item == NULL)
    return false;

  switch (collection->type) {
    case COLLECTION_STACK:
      return stack_pop(collection->impl, item);

    case COLLECTION_QUEUE:
      return queue_remove(collection->impl, item);

    case COLLECTION_ARRAY_LIST:
      return seq_list_remove(collection->impl, item);

    case COLLECTION_LINKED_LIST:
      return linked_list_remove(collection->impl, item);

    case COLLECTION_SORTED_LIST:
      return ord_list_remove(collection->impl, item);

    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_remove(collection->impl, item);

    case COLLECTION_CROSS_LIST:
      return cross_list_remove(collection->impl, item);

    case COLLECTION_GROUPED_LIST:
    default:
      return false;
  }
}

size_t collection_size(const COLLECTION *collection) {
  if (collection == NULL)
    return 0;

  switch (collection->type) {
    case COLLECTION_STACK:
      return stack_size(collection->impl);

    case COLLECTION_QUEUE:
      return queue_size(collection->impl);

    case COLLECTION_ARRAY_LIST:
      return seq_list_size(collection->impl);

    case COLLECTION_LINKED_LIST:
      return linked_list_size(collection->impl);

    case COLLECTION_SORTED_LIST:
      return ord_list_size(collection->impl);

    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_size(collection->impl);

    case COLLECTION_CROSS_LIST:
      return cross_list_size(collection->impl);

    case COLLECTION_GROUPED_LIST:
    default:
      return 0;
  }
}

bool collection_visit(const COLLECTION *collection,
                      COLLECTION_VISITOR visitor,
                      void *context){

}

bool collection_visit_cross_index(const COLLECTION *collection,
                                  bool second_index,
                                  COLLECTION_VISITOR visitor,
                                  void *context);

bool collection_type_parse(const char *name, COLLECTION_TYPE *type)
{
  if (name == NULL || type == NULL)
    return false;

  for (size_t i = 0; i < TYPE_COUNT; i++) {
    if (strcmp(type_table[i].name, name) == 0) {
      *type = type_table[i].type;
      return true;
    }
  }
  return false;
}

const char *collection_type_name(COLLECTION_TYPE type)
{
  for (size_t i = 0; i < TYPE_COUNT; i++) {
    if (type_table[i].type == type)
      return type_table[i].name;
  }
  return "desconhecido";
}
