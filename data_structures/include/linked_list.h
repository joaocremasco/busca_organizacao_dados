#ifndef LINKED_LIST_H
#define LINKED_LIST_H
	
#include <stdbool.h>

typedef struct linked_list_ LINKED_LIST;

/* Cria e inicializa a estrutura; retorna NULL se a alocação falhar. A função opcional destroy é
   chamada para os objetos na destruição. */
LINKED_LIST* linked_list_init(void (*destroy)(void*));
/* Libera a estrutura e seus nós ou vetores; chama destroy para os objetos se configurado. No
   projeto, ITEM_STORE é dono dos ITEMs. */
void linked_list_destroy(LINKED_LIST **list);

/* Insere o ponteiro do objeto no fim; retorna false para argumento inválido ou falha de
   alocação. No projeto, ITEM_STORE continua dono do ITEM. */
bool linked_list_push_back(LINKED_LIST* list, void *object);
/* Remove e devolve o objeto do fim; retorna NULL se a estrutura estiver vazia ou inválida. Não
   libera o objeto armazenado. */
void* linked_list_pop_back(LINKED_LIST *list);
/* Devolve sem remover o objeto do fim; retorna NULL se a estrutura estiver vazia ou inválida.
   Não transfere sua propriedade. */
void* linked_list_back(LINKED_LIST *list);

/* Insere o ponteiro do objeto no início; retorna false para argumento inválido ou falha de
   alocação. No projeto, ITEM_STORE continua dono do ITEM. */
bool linked_list_push_front(LINKED_LIST* list, void *object);
/* Remove e devolve o objeto do início; retorna NULL se a estrutura estiver vazia ou inválida.
   Não libera o objeto armazenado. */
void* linked_list_pop_front(LINKED_LIST* list);
/* Devolve sem remover o objeto do início; retorna NULL se a estrutura estiver vazia ou inválida.
   Não transfere sua propriedade. */
void* linked_list_front(LINKED_LIST *list);

/* Procura conforme o comparador e devolve o objeto encontrado; retorna NULL se não encontrar ou
   se os argumentos forem inválidos. Não transfere a propriedade. */
void* linked_list_find(LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

/* Remove e devolve o objeto; retorna NULL se não encontrar ou se os argumentos forem inválidos.
   Não libera a memória do objeto. */
void* linked_list_erase(LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

/* Substitui o objeto encontrado e devolve o anterior; retorna NULL se o alvo não existir ou os
   argumentos forem inválidos. Não libera nenhum objeto. */
void* linked_list_set(LINKED_LIST *list, void *object, 
		void *target, int (*cmp)(void*, void*));

/* Insere o objeto antes do alvo; retorna false se argumentos, alvo ou alocação forem inválidos.
   Armazena os ponteiros sem cópia. */
bool linked_list_insert(LINKED_LIST *list, void *object, 
		void *target, int (*cmp)(void*, void*));

/* Devolve a quantidade de elementos; retorna 0 se a estrutura for NULL. */
int linked_list_size(LINKED_LIST *list);
/* Retorna true se a estrutura estiver vazia ou for NULL; false se contiver elementos. */
bool linked_list_empty(LINKED_LIST *list);

/* Percorre os elementos; retorna true se visitar tudo e false para argumentos inválidos ou
   parada da função de visita. */
bool linked_list_visit(LINKED_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
