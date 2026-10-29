#ifndef CROSSED_LIST_H
#define CROSSED_LIST_H

#include <stdbool.h>

typedef struct crossed_list_ CROSSED_LIST;

// o liberador pode ser nulo quando a estrutura nao for dona dos objetos
CROSSED_LIST* crossed_list_init(void (*destroy)(void*));
// o liberador recebe cada objeto ao destruir a estrutura
void crossed_list_destroy(CROSSED_LIST **list);

void* crossed_list_get(CROSSED_LIST *list, int i, int j);
// nao aceita duas insercoes na mesma coordenada
bool crossed_list_insert(CROSSED_LIST *list, int i, int j, void *object);

// devolve tambem as coordenadas encontradas
void* crossed_list_find(CROSSED_LIST *list, void *target, 
		int (*cmp)(void*, void*), int *i, int *j);

// remove o objeto da estrutura sem libera-lo
void* crossed_list_remove(CROSSED_LIST *list, int i, int j);

// escolhe entre percorrer linhas ou colunas
bool crossed_list_remove_line(CROSSED_LIST *list, int index, bool isRow);

// escolhe entre percorrer linhas ou colunas, se encolher perde o que sobrar
bool crossed_list_resize(CROSSED_LIST *list, int capacity, bool isRow);
int crossed_list_size(CROSSED_LIST *list);
// escolhe entre percorrer linhas ou colunas
int crossed_list_capacity(CROSSED_LIST *list, bool isRow);
bool crossed_list_empty(CROSSED_LIST *list);

// interrompe o percurso quando a funcao devolve falso
bool crossed_list_visit(CROSSED_LIST *list, 
		bool (*visitor)(int i, int j, void *object, void *context), void *context);

// escolhe entre percorrer linhas ou colunas
bool crossed_list_visit_line(CROSSED_LIST *list, int index, 
		bool isRow, bool (*visitor)(int i, int j, void *object, void *context), void *context);

#endif
