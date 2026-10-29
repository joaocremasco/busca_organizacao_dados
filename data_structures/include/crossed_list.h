#ifndef CROSSED_LIST_H
#define CROSSED_LIST_H

#include <stdbool.h>

typedef struct crossed_list_ CROSSED_LIST;

/* Cria e inicializa a estrutura; retorna NULL se a alocação falhar. A função opcional destroy é
   chamada para os objetos na destruição. */
CROSSED_LIST* crossed_list_init(void (*destroy)(void*));
/* Libera a estrutura e seus nós ou vetores; chama destroy para os objetos se configurado. No
   projeto, ITEM_STORE é dono dos ITEMs. */
void crossed_list_destroy(CROSSED_LIST **list);

/* Devolve sem remover o objeto na posição indicada; retorna NULL se a posição ou estrutura forem
   inválidas. Não transfere sua propriedade. */
void* crossed_list_get(CROSSED_LIST *list, int i, int j);
/* Insere o objeto na coordenada (i, j); retorna false para coordenada inválida, ocupada ou falha
   de alocação. Não copia o objeto. */
bool crossed_list_insert(CROSSED_LIST *list, int i, int j, void *object);

/* Procura conforme o comparador, grava a coordenada em i e j e devolve o objeto; retorna NULL se
   não encontrar ou se os argumentos forem inválidos. Não transfere a propriedade. */
void* crossed_list_find(CROSSED_LIST *list, void *target, 
		int (*cmp)(void*, void*), int *i, int *j);

/* Remove e devolve o objeto; retorna NULL se não encontrar ou se os argumentos forem inválidos.
   Não libera a memória do objeto. */
void* crossed_list_remove(CROSSED_LIST *list, int i, int j);

/* Remove a linha ou coluna e seus nós; retorna false se a estrutura ou o índice forem inválidos.
   No projeto, os ITEMs continuam no ITEM_STORE. */
bool crossed_list_remove_line(CROSSED_LIST *list, int index, bool isRow);

/* Redimensiona a capacidade; retorna false para capacidade inválida ou falha de alocação. Ao
   encolher, remove as posições excedentes. */
bool crossed_list_resize(CROSSED_LIST *list, int capacity, bool isRow);
/* Devolve a quantidade de elementos; retorna 0 se a estrutura for NULL. */
int crossed_list_size(CROSSED_LIST *list);
/* Devolve a capacidade de linhas ou colunas; retorna 0 se a estrutura for NULL. */
int crossed_list_capacity(CROSSED_LIST *list, bool isRow);
/* Retorna true se a estrutura estiver vazia ou for NULL; false se contiver elementos. */
bool crossed_list_empty(CROSSED_LIST *list);

/* Percorre os elementos; retorna true se visitar tudo e false para argumentos inválidos ou
   parada da função de visita. */
bool crossed_list_visit(CROSSED_LIST *list, 
		bool (*visitor)(int i, int j, void *object, void *context), void *context);

/* Percorre a linha ou coluna; retorna true se visitar tudo e false para índice inválido ou
   parada da função de visita. */
bool crossed_list_visit_line(CROSSED_LIST *list, int index, 
		bool isRow, bool (*visitor)(int i, int j, void *object, void *context), void *context);

#endif
