#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sequential_list.h"

struct sequential_list{ 
  void **data;
  size_t size;
  size_t capacity; 
};

SEQ_LIST *list_create(void){
  SEQ_LIST *list = malloc(sizeof(*list));

  if(list == NULL)
    return NULL;

  list->data = NULL;
  list->capacity = 0;
  list->size = 0;

  return list;
}

void list_delete(SEQ_LIST **list){
  if(list == NULL || *list == NULL)
    return;
  
  while((list)->){
    free((*list)->data[(*list)->size - 1];
    (*list)->size--;
  }
  free(*list);
  *list == NULL;
}

bool list_insert(SEQ_LIST *list, void *data){
  if(list == NULL)
    return false;

  if(list->size == list->capacity){
    size_t new_cap = list->capacity == 0 ? 4 : list->capacity * 2;
    void **temp = realloc(list->data, new_cap * sizeof(*list->data));
    if(temp == NULL)
      return false;
    list->data = temp;
    list->capacity = new_cap;
  }

  list->data[list->size] = data;
  list->size++;
  return true;
}
size_t list_size(const SEQ_LIST *list);
bool list_is_empty(const SEQ_LIST *list);
