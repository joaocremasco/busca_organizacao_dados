#ifndef SEQ_LIST_H
#define SEQ_LIST_H

#include <stdbool.h>

typedef struct seq_list_ SEQ_LIST;

// destroy pode ser nulo quando os objetos forem administrados por outro módulo.
SEQ_LIST* seq_list_init(void (*destroy)(void*));
// Chama destroy para cada objeto, se o callback tiver sido informado.
void seq_list_destroy(SEQ_LIST **sq);

bool seq_list_push_back(SEQ_LIST* sq, void *object);
// Remove e devolve o objeto sem chamar destroy.
void* seq_list_pop_back(SEQ_LIST *sq);

// Abre uma posição em idx, deslocando os objetos seguintes para a direita.
bool seq_list_insert(SEQ_LIST* sq, void *object, int idx);
// Ao encolher, descarta os objetos que ficam fora da capacidade.
bool seq_list_resize(SEQ_LIST *sq, int capacity);
// Compacta o vetor após a remoção; o objeto devolvido continua válido.
void* seq_list_erase(SEQ_LIST *sq, int idx);

void* seq_list_get(SEQ_LIST *sq, int idx);
// Substitui o objeto e devolve a referência anterior.
void* seq_list_set(SEQ_LIST *sq, void *object, int idx);
// Devolve o índice encontrado ou -1 se não houver correspondência.
int seq_list_find(SEQ_LIST *sq, void *object, int (*cmp)(void*, void*));

int seq_list_size(SEQ_LIST *sq);
bool seq_list_empty(SEQ_LIST *sq);

// Para e devolve false se o visitor devolver false.
bool seq_list_visit(SEQ_LIST *sq, bool (*visitor)(void *object, void *context), void *context);

#endif
