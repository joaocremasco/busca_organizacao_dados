#ifndef SEQ_LIST_H
#define SEQ_LIST_H

#include <stdbool.h>

typedef struct seq_list_ SEQ_LIST;

// o liberador pode ser nulo quando a estrutura nao for dona dos objetos
SEQ_LIST* seq_list_init(void (*destroy)(void*));
// o liberador recebe cada objeto ao destruir a estrutura
void seq_list_destroy(SEQ_LIST **sq);

bool seq_list_push_back(SEQ_LIST* sq, void *object);
// remove da estrutura sem liberar o objeto
void* seq_list_pop_back(SEQ_LIST *sq);

// desloca os itens a direita do indice
bool seq_list_insert(SEQ_LIST* sq, void *object, int idx);
// ao encolher, remove os objetos que ficam fora da capacidade
bool seq_list_resize(SEQ_LIST *sq, int capacity);
// puxa os itens da direita pra esquerda, o objeto continua vivo
void* seq_list_erase(SEQ_LIST *sq, int idx);

void* seq_list_get(SEQ_LIST *sq, int idx);
// troca o objeto e devolve a referencia anterior
void* seq_list_set(SEQ_LIST *sq, void *object, int idx);
// devolve o indice ou -1
int seq_list_find(SEQ_LIST *sq, void *object, int (*cmp)(void*, void*));

int seq_list_size(SEQ_LIST *sq);
bool seq_list_empty(SEQ_LIST *sq);

// interrompe o percurso quando a funcao devolve falso
bool seq_list_visit(SEQ_LIST *sq, bool (*visitor)(void *object, void *context), void *context);

#endif
