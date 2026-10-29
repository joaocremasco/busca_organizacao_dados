#ifndef SEQ_LIST_H
#define SEQ_LIST_H

#include <stdbool.h>

typedef struct seq_list_ SEQ_LIST;

// destroy e opcional, passa NULL se nao precisar
SEQ_LIST* seq_list_init(void (*destroy)(void*));
// se tiver destroy chama em cada objeto, mas aqui quem libera os itens e o ITEM_STORE
void seq_list_destroy(SEQ_LIST **sq);

bool seq_list_push_back(SEQ_LIST* sq, void *object);
// so tira da estrutura, o objeto continua vivo
void* seq_list_pop_back(SEQ_LIST *sq);

// desloca os itens a direita do indice
bool seq_list_insert(SEQ_LIST* sq, void *object, int idx);
// se encolher perde os itens do fim
bool seq_list_resize(SEQ_LIST *sq, int capacity);
// puxa os itens da direita pra esquerda, o objeto continua vivo
void* seq_list_erase(SEQ_LIST *sq, int idx);

void* seq_list_get(SEQ_LIST *sq, int idx);
// devolve o objeto que estava no lugar
void* seq_list_set(SEQ_LIST *sq, void *object, int idx);
// devolve o indice ou -1
int seq_list_find(SEQ_LIST *sq, void *object, int (*cmp)(void*, void*));

int seq_list_size(SEQ_LIST *sq);
bool seq_list_empty(SEQ_LIST *sq);

// se o visitor devolver false ja para por ali
bool seq_list_visit(SEQ_LIST *sq, bool (*visitor)(void *object, void *context), void *context);

#endif
