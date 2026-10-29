#ifndef CROSSED_LIST_H
#define CROSSED_LIST_H

#include <stdbool.h>

typedef struct crossed_list_ CROSSED_LIST;

// destroy e opcional, passa NULL se nao precisar
CROSSED_LIST* crossed_list_init(void (*destroy)(void*));
// se tiver destroy chama em cada objeto, mas aqui quem libera os itens e o ITEM_STORE
void crossed_list_destroy(CROSSED_LIST **list);

void* crossed_list_get(CROSSED_LIST *list, int i, int j);
// falha se a posicao ja estiver ocupada
bool crossed_list_insert(CROSSED_LIST *list, int i, int j, void *object);

// devolve tambem a posicao em i e j
void* crossed_list_find(CROSSED_LIST *list, void *target, 
		int (*cmp)(void*, void*), int *i, int *j);

// o objeto continua vivo
void* crossed_list_remove(CROSSED_LIST *list, int i, int j);

// isRow escolhe entre linha e coluna
bool crossed_list_remove_line(CROSSED_LIST *list, int index, bool isRow);

// isRow escolhe entre linha e coluna, se encolher perde o que sobrar
bool crossed_list_resize(CROSSED_LIST *list, int capacity, bool isRow);
int crossed_list_size(CROSSED_LIST *list);
// isRow escolhe entre linha e coluna
int crossed_list_capacity(CROSSED_LIST *list, bool isRow);
bool crossed_list_empty(CROSSED_LIST *list);

// se o visitor devolver false ja para por ali
bool crossed_list_visit(CROSSED_LIST *list, 
		bool (*visitor)(int i, int j, void *object, void *context), void *context);

// isRow escolhe entre linha e coluna
bool crossed_list_visit_line(CROSSED_LIST *list, int index, 
		bool isRow, bool (*visitor)(int i, int j, void *object, void *context), void *context);

#endif
