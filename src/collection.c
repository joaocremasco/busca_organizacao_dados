
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/collection.h"
#include "../data_structures/include/stack.h"
#include "../data_structures/include/seq_list.h"
#include "../data_structures/include/queue.h"
#include "../data_structures/include/ord_list.h"
#include "../data_structures/include/linked_list.h"
#include "../data_structures/include/gen_list.h"
#include "../data_structures/include/dn_linked_list.h"
#include "../data_structures/include/crossed_list.h"

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

static int compare_item_pointers(void *left, void *right) {
  uintptr_t a = (uintptr_t)left;
  uintptr_t b = (uintptr_t)right;
  return (a > b) - (a < b);
}

COLLECTION *collection_create(COLLECTION_TYPE type,
                              const COLLECTION_OPTIONS *options) {
  COLLECTION *collection = malloc(sizeof(*collection));

  if (collection == NULL)
    return NULL;

  collection->type = type;
  collection->impl = NULL;

  switch (type) {
    case COLLECTION_STACK:
      collection->impl = stack_init(NULL);
      break;

    case COLLECTION_QUEUE:
      collection->impl = queue_init(NULL);
      break;

    case COLLECTION_ARRAY_LIST:
      collection->impl = seq_list_init(NULL);
      break;

    case COLLECTION_LINKED_LIST:
      collection->impl = linked_list_init(NULL);
      break;

    case COLLECTION_SORTED_LIST:
      collection->impl = ord_list_init(NULL, compare_item_pointers);
      break;

    case COLLECTION_SENTINEL_LIST:
      collection->impl = dn_linked_list_init(NULL);
      break;

    case COLLECTION_CROSS_LIST:
      collection->impl = crossed_list_init(NULL);
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
    case COLLECTION_STACK: {
      STACK *impl = (*collection)->impl;
      stack_destroy(&impl);
      break;
    }

    case COLLECTION_QUEUE: {
      QUEUE *impl = (*collection)->impl;
      queue_destroy(&impl);
      break;
    }

    case COLLECTION_ARRAY_LIST: {
      SEQ_LIST *impl = (*collection)->impl;
      seq_list_destroy(&impl);
      break;
    }

    case COLLECTION_LINKED_LIST: {
      LINKED_LIST *impl = (*collection)->impl;
      linked_list_destroy(&impl);
      break;
    }

    case COLLECTION_SORTED_LIST: {
      ORD_LIST *impl = (*collection)->impl;
      ord_list_destroy(&impl);
      break;
    }

    case COLLECTION_SENTINEL_LIST: {
      DN_LINKED_LIST *impl = (*collection)->impl;
      dn_linked_list_destroy(&impl);
      break;
    }

    case COLLECTION_CROSS_LIST: {
      CROSSED_LIST *impl = (*collection)->impl;
      crossed_list_destroy(&impl);
      break;
    }

    case COLLECTION_GROUPED_LIST:
    default:
      break;
  }

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
      return queue_push(collection->impl, item);

    case COLLECTION_ARRAY_LIST:
      return seq_list_push_back(collection->impl, item);

    case COLLECTION_LINKED_LIST:
      return linked_list_push_back(collection->impl, item);

    case COLLECTION_SORTED_LIST:
      return ord_list_insert(collection->impl, item);

    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_push_back(collection->impl, item);

    case COLLECTION_CROSS_LIST:
      {
        CROSSED_LIST *crossed = collection->impl;
        int row = crossed_list_size(crossed);
        if (!crossed_list_resize(crossed, row + 1, true) ||
            !crossed_list_resize(crossed, 1, false))
          return false;
        return crossed_list_insert(crossed, row, 0, item);
      }

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
      if (stack_top(collection->impl) != item) return false;
      return stack_pop(collection->impl) == item;

    case COLLECTION_QUEUE:
      if (queue_front(collection->impl) != item) return false;
      return queue_pop(collection->impl) == item;

    case COLLECTION_ARRAY_LIST:
      {
        int index = seq_list_find(collection->impl, item, compare_item_pointers);
        return index >= 0 && seq_list_erase(collection->impl, index) == item;
      }

    case COLLECTION_LINKED_LIST:
      return linked_list_erase(collection->impl, item, compare_item_pointers) == item;

    case COLLECTION_SORTED_LIST:
      return ord_list_erase(collection->impl, item) == item;

    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_erase(collection->impl, item, compare_item_pointers) == item;

    case COLLECTION_CROSS_LIST:
      {
        CROSSED_LIST *crossed = collection->impl;
        int row, column;
        if (crossed_list_find(crossed, item, compare_item_pointers, &row, &column) != item)
          return false;
        return crossed_list_remove(crossed, row, column) == item;
      }

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
      return crossed_list_size(collection->impl);

    case COLLECTION_GROUPED_LIST:
    default:
      return 0;
  }
}

typedef struct {
  COLLECTION_VISITOR visitor;
  void *context;
} COLLECTION_VISIT_CONTEXT;

static bool collection_cross_visit_adapter(int i, int j, void *item, void *context);

bool collection_visit(const COLLECTION *collection,
                      COLLECTION_VISITOR visitor,
                      void *context){
  if (collection == NULL || visitor == NULL)
    return false;

  switch (collection->type) {
    case COLLECTION_STACK:
      return stack_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_QUEUE:
      return queue_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_ARRAY_LIST:
      return seq_list_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_LINKED_LIST:
      return linked_list_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_SENTINEL_LIST:
      return dn_linked_list_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_SORTED_LIST:
      return ord_list_visit(collection->impl, (bool (*)(void *, void *))visitor, context);
    case COLLECTION_CROSS_LIST: {
      COLLECTION_VISIT_CONTEXT visit_context = {visitor, context};
      return crossed_list_visit(collection->impl, collection_cross_visit_adapter,
                                &visit_context);
    }
    case COLLECTION_GROUPED_LIST:
    default:
      return false;
  }
}

static bool collection_cross_visit_adapter(int i, int j, void *item, void *context) {
  (void)i;
  (void)j;
  COLLECTION_VISITOR visitor = ((COLLECTION_VISIT_CONTEXT *)context)->visitor;
  return visitor(item, ((COLLECTION_VISIT_CONTEXT *)context)->context);
}

bool collection_visit_cross_index(const COLLECTION *collection,
                                  bool second_index,
                                  COLLECTION_VISITOR visitor,
                                  void *context) {
  if (collection == NULL || visitor == NULL || collection->type != COLLECTION_CROSS_LIST)
    return false;

  COLLECTION_VISIT_CONTEXT visit_context = {visitor, context};
  CROSSED_LIST *crossed = collection->impl;
  bool is_row = !second_index;
  int line_count = crossed_list_capacity(crossed, is_row);
  for (int index = 0; index < line_count; index++) {
    if (!crossed_list_visit_line(crossed, index, is_row,
                                 collection_cross_visit_adapter, &visit_context))
      return false;
  }
  return true;
}

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
