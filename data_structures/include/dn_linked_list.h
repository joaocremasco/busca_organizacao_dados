#ifndef DN_DN_LINKED_LIST_H
#define DN_DN_LINKED_LIST_H
	
#include <stdbool.h>

typedef struct dn_linked_list_ DN_LINKED_LIST;

/* Cria e inicializa a estrutura; retorna NULL se a alocação falhar. A função opcional destroy é
   chamada para os objetos na destruição. */
DN_LINKED_LIST* dn_linked_list_init(void (*destroy)(void*));
/* Libera a estrutura e seus nós ou vetores; chama destroy para os objetos se configurado. No
   projeto, ITEM_STORE é dono dos ITEMs. */
void dn_linked_list_destroy(DN_LINKED_LIST **list);

/* Insere o ponteiro do objeto no fim; retorna false para argumento inválido ou falha de
   alocação. No projeto, ITEM_STORE continua dono do ITEM. */
bool dn_linked_list_push_back(DN_LINKED_LIST* list, void *object);
/* Remove e devolve o objeto do fim; retorna NULL se a estrutura estiver vazia ou inválida. Não
   libera o objeto armazenado. */
void* dn_linked_list_pop_back(DN_LINKED_LIST *list);
/* Devolve sem remover o objeto do fim; retorna NULL se a estrutura estiver vazia ou inválida.
   Não transfere sua propriedade. */
void* dn_linked_list_back(DN_LINKED_LIST *list);

/* Insere o ponteiro do objeto no início; retorna false para argumento inválido ou falha de
   alocação. No projeto, ITEM_STORE continua dono do ITEM. */
bool dn_linked_list_push_front(DN_LINKED_LIST* list, void *object);
/* Remove e devolve o objeto do início; retorna NULL se a estrutura estiver vazia ou inválida.
   Não libera o objeto armazenado. */
void* dn_linked_list_pop_front(DN_LINKED_LIST* list);
/* Devolve sem remover o objeto do início; retorna NULL se a estrutura estiver vazia ou inválida.
   Não transfere sua propriedade. */
void* dn_linked_list_front(DN_LINKED_LIST *list);

/* Procura conforme o comparador e devolve o objeto encontrado; retorna NULL se não encontrar ou
   se os argumentos forem inválidos. Não transfere a propriedade. */
void* dn_linked_list_find(DN_LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

/* Remove e devolve o objeto; retorna NULL se não encontrar ou se os argumentos forem inválidos.
   Não libera a memória do objeto. */
void* dn_linked_list_erase(DN_LINKED_LIST *list, 
		void *target, int (*cmp)(void*, void*));

/* Substitui o objeto encontrado e devolve o anterior; retorna NULL se o alvo não existir ou os
   argumentos forem inválidos. Não libera nenhum objeto. */
void* dn_linked_list_set(DN_LINKED_LIST *list, 
		void *object, void *target, int (*cmp)(void*, void*));

/* Insere o objeto antes do alvo; retorna false se argumentos, alvo ou alocação forem inválidos.
   Armazena os ponteiros sem cópia. */
bool dn_linked_list_insert(DN_LINKED_LIST *list, 
		void *object, void *target, int (*cmp)(void*, void*));

/* Devolve a quantidade de elementos; retorna 0 se a estrutura for NULL. */
int dn_linked_list_size(DN_LINKED_LIST *list);
/* Retorna true se a estrutura estiver vazia ou for NULL; false se contiver elementos. */
bool dn_linked_list_empty(DN_LINKED_LIST *list);

/* Percorre os elementos; retorna true se visitar tudo e false para argumentos inválidos ou
   parada da função de visita. */
bool dn_linked_list_visit(DN_LINKED_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
