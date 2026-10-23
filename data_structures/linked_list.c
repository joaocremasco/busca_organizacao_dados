#include "linked_list.h"

#include <stdlib.h>

typedef struct linked_list_node LINKED_LIST_NODE;

struct linked_list_node {
  void *data;
  LINKED_LIST_NODE *next;
};

struct linked_list {
  LINKED_LIST_NODE *start;
  LINKED_LIST_NODE *end;
  size_t size;
};

LINKED_LIST *linked_list_create(void) {
  LINKED_LIST *list = malloc(sizeof(*list));
  if (list == NULL)
    return NULL;

  list->start = NULL;
  list->end = NULL;
  list->size = 0;
  return list;
}

void linked_list_destroy(LINKED_LIST **list) {
  if (list == NULL || *list == NULL)
    return;

  LINKED_LIST_NODE *p = (*list)->start;
  while (p != NULL) {
    LINKED_LIST_NODE *next = p->next;
    free(p);                 /* só o nó, o data é do store */
    p = next;
  }

  free(*list);
  *list = NULL;
}

bool linked_list_insert(LINKED_LIST *list, void *data) {
  if (list == NULL || data == NULL)
    return false;

  LINKED_LIST_NODE *node = malloc(sizeof(*node));
  if (node == NULL)
    return false;

  node->data = data;
  node->next = NULL;

  if (list->start == NULL)
    list->start = node;
  else
    list->end->next = node;

  list->end = node;
  list->size++;
  return true;
}

void *linked_list_remove(LINKED_LIST *list, const void *data) {
  if (list == NULL || list->start == NULL)
    return NULL;

  LINKED_LIST_NODE *prev = NULL;
  LINKED_LIST_NODE *actual = list->start;

  while (actual != NULL && actual->data != data) {
    prev = actual;
    actual = actual->next;
  }

  if (actual == NULL)
    return NULL;

  if (prev == NULL)
    list->start = actual->next;
  else
    prev->next = actual->next;

  if (actual == list->end)
    list->end = prev;

  void *item = actual->data;
  free(actual);
  list->size--;

  return item;
}

size_t linked_list_size(const LINKED_LIST *list) {
  return list != NULL ? list->size : 0;
}

bool linked_list_is_empty(const LINKED_LIST *list) {
  return list == NULL || list->size == 0;
}

bool linked_list_visit(const LINKED_LIST *list,
                       bool (*visitor)(void *data, void *context),
                       void *context)
{
  if (list == NULL || visitor == NULL)
    return false;

  for (const LINKED_LIST_NODE *p = list->start; p != NULL; p = p->next) {
    if (!visitor(p->data, context))
      break;
  }
  return true;
}
