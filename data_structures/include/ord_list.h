#ifndef ORD_LIST_H
#define ORD_LIST_H

#include <stdbool.h>

typedef struct ord_list_ ORD_LIST;
typedef int (*ORD_LIST_COMPARE)(void *left, void *right, void *context);

/* Cria uma lista ordenada com o comparador indicado; retorna NULL se o comparador ou a alocação
   forem inválidos. */
ORD_LIST* ord_list_init(void (*destroy)(void*), int (*cmp)(void*, void*));
/* Cria uma lista ordenada com o comparador indicado; retorna NULL se o comparador ou a alocação
   forem inválidos. */
ORD_LIST* ord_list_init_with_context(void (*destroy)(void*),
                                     ORD_LIST_COMPARE cmp, void *context);
/* Libera a estrutura e seus nós ou vetores; chama destroy para os objetos se configurado. No
   projeto, ITEM_STORE é dono dos ITEMs. */
void ord_list_destroy(ORD_LIST **sq);

/* Insere o objeto mantendo a ordem do comparador; retorna false para lista inválida ou falha de
   alocação. Armazena o ponteiro sem copiar o ITEM. */
bool ord_list_insert(ORD_LIST *sq, void *object);
/* Remove e devolve o objeto; retorna NULL se não encontrar ou se os argumentos forem inválidos.
   Não libera a memória do objeto. */
void* ord_list_erase(ORD_LIST *sq, void *target);

/* Redimensiona a capacidade; retorna false para capacidade inválida ou falha de alocação. Ao
   encolher, remove as posições excedentes. */
bool ord_list_resize(ORD_LIST *sq, int capacity);
/* Devolve sem remover o objeto na posição indicada; retorna NULL se a posição ou estrutura forem
   inválidas. Não transfere sua propriedade. */
void* ord_list_get(ORD_LIST *sq, int idx);

/* Procura o objeto pela chave ordenada e devolve seu índice; retorna -1 se não encontrar ou se
   os argumentos forem inválidos. */
int ord_list_find(ORD_LIST *sq, void *target);
/* Devolve o primeiro índice cuja chave é maior que a consultada; retorna -1 se os argumentos
   forem inválidos. */
int ord_list_upper_bound(ORD_LIST *sq, void *target);
/* Devolve o primeiro índice cuja chave não é menor que a consultada; retorna -1 se os argumentos
   forem inválidos. */
int ord_list_lower_bound(ORD_LIST *sq, void *target);

/* Devolve a quantidade de elementos; retorna 0 se a estrutura for NULL. */
int ord_list_size(ORD_LIST *sq);
/* Retorna true se a estrutura estiver vazia ou for NULL; false se contiver elementos. */
bool ord_list_empty(ORD_LIST *sq);

/* Percorre os elementos; retorna true se visitar tudo e false para argumentos inválidos ou
   parada da função de visita. */
bool ord_list_visit(ORD_LIST *sq, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
