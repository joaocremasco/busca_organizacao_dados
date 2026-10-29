#ifndef GEN_LIST_H
#define GEN_LIST_H
	
#include <stdbool.h>

typedef struct gen_list_ GEN_LIST;

/* Cria e inicializa a estrutura; retorna NULL se a alocação falhar. A função opcional destroy é
   chamada para os objetos na destruição. */
GEN_LIST* gen_list_init(void (*destroy)(void*));
/* Libera a estrutura e seus nós ou vetores; chama destroy para os objetos se configurado. No
   projeto, ITEM_STORE é dono dos ITEMs. */
void gen_list_destroy(GEN_LIST **list);

/* Insere o objeto no fim e registra se é átomo ou sublista; retorna false para argumento
   inválido ou falha de alocação. Armazena o ponteiro sem cópia. */
bool gen_list_push_back(GEN_LIST* list, void *object, bool atom);
/* Remove e devolve o objeto do fim e informa seu tipo em isAtom; retorna NULL se a lista estiver
   vazia ou inválida. No projeto, ITEM_STORE é dono dos ITEMs. */
void* gen_list_pop_back(GEN_LIST *list, bool *isAtom);
/* Devolve sem remover o objeto do fim e informa seu tipo em isAtom; retorna NULL se a lista
   estiver vazia ou inválida. Não transfere sua propriedade. */
void* gen_list_back(GEN_LIST *list, bool *isAtom);

/* Insere o objeto no início e registra se é átomo ou sublista; retorna false para argumento
   inválido ou falha de alocação. Armazena o ponteiro sem cópia. */
bool gen_list_push_front(GEN_LIST* list, void *object, bool atom);
/* Remove e devolve o objeto do início e informa seu tipo em isAtom; retorna NULL se a lista
   estiver vazia ou inválida. No projeto, ITEM_STORE é dono dos ITEMs. */
void* gen_list_pop_front(GEN_LIST* list, bool *isAtom);
/* Devolve sem remover o objeto do início e informa seu tipo em isAtom; retorna NULL se a lista
   estiver vazia ou inválida. Não transfere sua propriedade. */
void* gen_list_front(GEN_LIST *list, bool *isAtom);

/* Procura objeto do tipo indicado por atom; informa o tipo em isAtom e retorna NULL se não
   encontrar ou houver argumento inválido. Não transfere a propriedade. */
void* gen_list_find(GEN_LIST *list, void *object, 
		bool atom, int (*cmp)(void*, void*), bool *isAtom);

/* Remove e devolve o objeto do tipo indicado por atom; informa o tipo em isAtom e retorna NULL se
   não encontrar. No projeto, ITEM_STORE continua dono dos ITEMs. */
void* gen_list_erase(GEN_LIST *list, void *object, 
		bool atom, int (*cmp)(void*, void*), bool *isAtom);

/* Substitui o alvo do tipo targ_atom e devolve o objeto anterior; retorna NULL se não encontrar.
   Informa o tipo anterior em isAtom e não libera os objetos. */
void* gen_list_set(GEN_LIST *list, void *object, 
		bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*), bool *isAtom);

/* Insere o objeto antes do alvo; retorna false se argumentos, alvo ou alocação forem inválidos.
   Registra também se os objetos são átomos ou sublistas. Armazena os ponteiros sem cópia. */
bool gen_list_insert(GEN_LIST *list, void *object, 
		bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*));

/* Devolve a quantidade de elementos; retorna 0 se a estrutura for NULL. */
int gen_list_size(GEN_LIST *list);
/* Retorna true se a estrutura estiver vazia ou for NULL; false se contiver elementos. */
bool gen_list_empty(GEN_LIST *list);

/* Percorre os elementos; retorna true se visitar tudo e false para argumentos inválidos ou
   parada da função de visita. */
bool gen_list_visit(GEN_LIST *list, 
		bool (*visitor)(void *object, void *context), void *context);

#endif
