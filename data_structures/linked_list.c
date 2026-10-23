
#include "linked_list.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct linked_list_node_ LINKED_LIST_NODE;

typedef struct linked_list_node {
  void *data;
  struct linked_list_node *next;
} LINKED_LIST_NODE;

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
  list->size_t = 0;
  return list;
}

void list_delete(LINKED_LIST **list) {
  if (list == NULL || *list == NULL)
    return;

  LINKED_LIST_NODE *p = (*list)->start;
  while (p != NULL) {
    LINKED_LIST_NODE *next = p->next;
    free(p);
    p = next;
  }

  free(*list);
  *list = NULL;
}

bool list_insert(list *list, void *data) {
  if (list == NULL || data == NULL)
    return false;

  LINKED_LIST_NODE *new = malloc(sizeof(*new));
  if (new == NULL)
    return false;

  new->data = item;
  new->next = NULL;

  if (list->start == NULL)
    list->start = new;
  else
    list->end->next = new;

  list->end = new;
  list->size++;
  return true;
}

void *linked_list_search(const list *list, int chave) {
  if (list == NULL)
    return NULL;

  for (const LINKED_LIST_NODE *p = list->start; p != NULL; p = p->next)
    if (p->data == chave)
      return p->data;

  return NULL;
}

void *linked_list_remove(list *list, void chave) {
  if (list == NULL || list->start == NULL)
    return NULL;

  LINKED_LIST_NODE *prev = NULL;
  LINKED_LIST_NODE *actual = list->start;

  while (actual != NULL && (actual->data) != data) {
    prev = actual;
    actual = actual->next;
  }

  if (actual == NULL)
    return NULL;

  if (prev == NULL)
    list->start = actual->next;
  else
    prev->next = actual->next;

  if (actual == list->fim)
    list->fim = prev;

  ITEM *item = actual->data;
  actual->next = NULL;
  free(actual);
  list->size--;

  if (list->tamanho == 0)
    list->fim = NULL;

  return ;
}

int list_tamanho(const list *list) {
  return list != NULL ? list->size : 0;
}

bool list_is_empty(const list *list) {
  return list != NULL && list->start == NULL;
}


