#ifndef SEQ_LIST_H
#define SEQ_LIST_H

#include <stdbool.h>

typedef struct seq_list_ SEQ_LIST;

SEQ_LIST* seq_list_init(void (*destroy)(void*));
void seq_list_destroy(SEQ_LIST **sq);

bool seq_list_push_back(SEQ_LIST* sq, void *object);
void* seq_list_pop_back(SEQ_LIST *sq);

bool seq_list_insert(SEQ_LIST* sq, void *object, int idx);
bool seq_list_resize(SEQ_LIST *sq, int capacity);
void* seq_list_erase(SEQ_LIST *sq, int idx);

void* seq_list_get(SEQ_LIST *sq, int idx);
void* seq_list_set(SEQ_LIST *sq, void *object, int idx);
int seq_list_find(SEQ_LIST *sq, void *object, int (*cmp)(void*, void*));

int seq_list_size(SEQ_LIST *sq);
bool seq_list_empty(SEQ_LIST *sq);
/* Percorre na ordem natural. true = percorreu tudo; false = o visitor
   pediu para parar ou argumento invalido. Nao aloca nem libera nada. */
bool seq_list_visit(const SEQ_LIST *sq,
                    bool (*visitor)(void *data, void *context),
                    void *context);

#endif
