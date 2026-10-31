#ifndef ORD_LIST_H
#define ORD_LIST_H

#include <stdbool.h>

typedef struct ord_list_ ORD_LIST;
typedef int (*ORD_LIST_COMPARE)(void *left, void *right, void *context);

ORD_LIST* ord_list_init(void (*destroy)(void*), int (*cmp)(void*, void*));
// Passa context ao comparador em cada comparação entre objetos.
ORD_LIST* ord_list_init_with_context(void (*destroy)(void*),
                                     ORD_LIST_COMPARE cmp, void *context);
// Chama destroy para cada objeto, se o callback tiver sido informado.
void ord_list_destroy(ORD_LIST **sq);

bool ord_list_insert(ORD_LIST *sq, void *object);
// Remove e devolve o objeto sem chamar destroy.
void* ord_list_erase(ORD_LIST *sq, void *target);
// Remove pelo índice e devolve o objeto sem chamar destroy.
void* ord_list_erase_at(ORD_LIST *sq, int idx);

// Ao encolher, descarta os objetos que ficam fora da capacidade.
bool ord_list_resize(ORD_LIST *sq, int capacity);
void* ord_list_get(ORD_LIST *sq, int idx);

// Devolve o índice de uma chave igual ou -1 se não houver correspondência.
int ord_list_find(ORD_LIST *sq, void *target);
// Devolve a primeira posição após todas as chaves iguais.
int ord_list_upper_bound(ORD_LIST *sq, void *target);
// Devolve a primeira posição com chave igual ou maior.
int ord_list_lower_bound(ORD_LIST *sq, void *target);

int ord_list_size(ORD_LIST *sq);
bool ord_list_empty(ORD_LIST *sq);

// Para e devolve false se o visitor devolver false.
bool ord_list_visit(ORD_LIST *sq, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
