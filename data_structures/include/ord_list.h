#ifndef ORD_LIST_H
#define ORD_LIST_H

#include <stdbool.h>

typedef struct ord_list_ ORD_LIST;
typedef int (*ORD_LIST_COMPARE)(void *left, void *right, void *context);

ORD_LIST* ord_list_init(void (*destroy)(void*), int (*cmp)(void*, void*));
// o contexto e repassado pro comparador
ORD_LIST* ord_list_init_with_context(void (*destroy)(void*),
                                     ORD_LIST_COMPARE cmp, void *context);
// se tiver destroy chama em cada objeto, mas aqui quem libera os itens e o ITEM_STORE
void ord_list_destroy(ORD_LIST **sq);

bool ord_list_insert(ORD_LIST *sq, void *object);
// so tira da lista, o objeto continua vivo
void* ord_list_erase(ORD_LIST *sq, void *target);

// se encolher perde os itens do fim
bool ord_list_resize(ORD_LIST *sq, int capacity);
void* ord_list_get(ORD_LIST *sq, int idx);

// busca binaria, devolve o indice ou -1
int ord_list_find(ORD_LIST *sq, void *target);
// primeiro indice com chave maior que a do alvo
int ord_list_upper_bound(ORD_LIST *sq, void *target);
// primeiro indice com chave nao menor que a do alvo
int ord_list_lower_bound(ORD_LIST *sq, void *target);

int ord_list_size(ORD_LIST *sq);
bool ord_list_empty(ORD_LIST *sq);

// se o visitor devolver false ja para por ali
bool ord_list_visit(ORD_LIST *sq, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
