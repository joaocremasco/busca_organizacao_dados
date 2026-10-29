#ifndef ORD_LIST_H
#define ORD_LIST_H

#include <stdbool.h>

typedef struct ord_list_ ORD_LIST;
typedef int (*ORD_LIST_COMPARE)(void *left, void *right, void *context);

ORD_LIST* ord_list_init(void (*destroy)(void*), int (*cmp)(void*, void*));
// passa o contexto para a funcao de comparacao
ORD_LIST* ord_list_init_with_context(void (*destroy)(void*),
                                     ORD_LIST_COMPARE cmp, void *context);
// o liberador recebe cada objeto ao destruir a estrutura
void ord_list_destroy(ORD_LIST **sq);

bool ord_list_insert(ORD_LIST *sq, void *object);
// so tira da lista, o objeto continua vivo
void* ord_list_erase(ORD_LIST *sq, void *target);
// remove pelo indice sem liberar o objeto
void* ord_list_erase_at(ORD_LIST *sq, int idx);

// ao encolher, remove os objetos que ficam fora da capacidade
bool ord_list_resize(ORD_LIST *sq, int capacity);
void* ord_list_get(ORD_LIST *sq, int idx);

// busca binaria pelo valor de comparacao
int ord_list_find(ORD_LIST *sq, void *target);
// devolve a primeira posicao depois das chaves iguais
int ord_list_upper_bound(ORD_LIST *sq, void *target);
// devolve a primeira posicao com chave igual ou maior
int ord_list_lower_bound(ORD_LIST *sq, void *target);

int ord_list_size(ORD_LIST *sq);
bool ord_list_empty(ORD_LIST *sq);

// interrompe o percurso quando a funcao devolve falso
bool ord_list_visit(ORD_LIST *sq, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
