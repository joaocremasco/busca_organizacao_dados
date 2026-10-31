#ifndef CROSSED_LIST_H
#define CROSSED_LIST_H

#include <stdbool.h>

typedef struct crossed_list_ CROSSED_LIST;

// destroy pode ser nulo quando os objetos forem administrados por outro módulo.
CROSSED_LIST* crossed_list_init(void (*destroy)(void*));
// Chama destroy para cada objeto, se o callback tiver sido informado.
void crossed_list_destroy(CROSSED_LIST **list);

void* crossed_list_get(CROSSED_LIST *list, int i, int j);
// Não aceita outra inserção em uma coordenada já ocupada.
bool crossed_list_insert(CROSSED_LIST *list, int i, int j, void *object);

// Também escreve em i e j as coordenadas do objeto encontrado, quando não nulos.
void* crossed_list_find(CROSSED_LIST *list, void *target, 
		int (*cmp)(void*, void*), int *i, int *j);

// Desvincula o objeto e o devolve sem chamar destroy.
void* crossed_list_remove(CROSSED_LIST *list, int i, int j);

// isRow escolhe se index identifica uma linha ou uma coluna.
bool crossed_list_remove_line(CROSSED_LIST *list, int index, bool isRow);

// isRow escolhe qual eixo redimensionar; ao encolher, remove as células excedentes.
bool crossed_list_resize(CROSSED_LIST *list, int capacity, bool isRow);
int crossed_list_size(CROSSED_LIST *list);
// isRow escolhe se a capacidade consultada é de linhas ou de colunas.
int crossed_list_capacity(CROSSED_LIST *list, bool isRow);
bool crossed_list_empty(CROSSED_LIST *list);

// Para e devolve false se o visitor devolver false.
bool crossed_list_visit(CROSSED_LIST *list, 
		bool (*visitor)(int i, int j, void *object, void *context), void *context);

// isRow escolhe o eixo e index a linha ou coluna visitada.
bool crossed_list_visit_line(CROSSED_LIST *list, int index, 
		bool isRow, bool (*visitor)(int i, int j, void *object, void *context), void *context);

#endif
