// matriz esparsa, cada no fica ligado na sua linha e na sua coluna
#include <stdlib.h>
#include "../include/crossed_list.h"

typedef struct node_ NODE;

struct node_ {
	int i;
	int j;
	NODE *up;
	NODE *down;
	NODE *right;
	NODE *left;
	void *object;
};

struct crossed_list_ {
	int size;
	NODE **rows;
	NODE **columns;
	int row_cap;
	int col_cap;
	void (*destroy)(void*);
};

CROSSED_LIST* crossed_list_init(void (*destroy)(void*)) {

	CROSSED_LIST *list = malloc(sizeof(*list));

	if(list == NULL)
		return NULL;

	list->size = 0;
	list->rows = NULL;
	list->columns = NULL;
	list->row_cap = 0;
	list->col_cap = 0;
	list->destroy = destroy;

	return list;
}

// ultimo no da linha i com coluna <= j, NULL se nao tiver
static NODE* crossed_list_iterate_right(CROSSED_LIST *list, int i, int j) {
	NODE *prev = NULL;
	NODE *idx = list->rows[i];

	while(idx != NULL && idx->j <= j) {
		prev = idx;
		idx = idx->right;
	}

	return prev;
}

// ultimo no da coluna j com linha <= i, NULL se nao tiver
static NODE* crossed_list_iterate_down(CROSSED_LIST *list, int i, int j) {
	
	NODE *prev = NULL;
	NODE *idx = list->columns[j];

	while(idx != NULL && idx->i <= i) {
		prev = idx;
		idx = idx->down;
	}

	return prev;
}

// so anda pela linha, a lista da linha e ordenada por coluna
static NODE* crossed_list_find_node(CROSSED_LIST *list, int i, int j) {
	if(list == NULL || i < 0 || i >= list->row_cap || j < 0 || j >= list->col_cap)
		return NULL;

	NODE *idx = list->rows[i];

	while(idx != NULL && idx->j < j) {
		idx = idx->right;
	}

	if(idx != NULL && idx->j == j)
		return idx;

	return NULL;
}

// se o no for a cabeca da linha ou da coluna atualiza o vetor tambem
static void crossed_list_unlink(CROSSED_LIST *list, NODE *node) {
	if(node->up != NULL) {
		node->up->down = node->down;
	}
	else if(node->j < list->col_cap && list->columns[node->j] == node) {
		list->columns[node->j] = node->down;
	}

	if(node->down != NULL) {
		node->down->up = node->up;
	}

	if(node->left != NULL) {
		node->left->right = node->right;
	}
	else if(node->i < list->row_cap && list->rows[node->i] == node) {
		list->rows[node->i] = node->right;
	}

	if(node->right != NULL) {
		node->right->left = node->left;
	}

	list->size--;
}

// apaga o no e tudo que vem depois dele na linha (ou coluna)
static bool crossed_list_erase_tail(CROSSED_LIST *list, NODE *node, bool isRow) {
	if(list == NULL)
		return false;

	NODE *idx = node;
	NODE *tmp;

	while(idx != NULL) {

		tmp = isRow ? idx->right : idx->down;

		if(list->destroy != NULL && idx->object != NULL)
			list->destroy(idx->object);

		crossed_list_unlink(list, idx);
		free(idx);

		idx = tmp;
	}

	return true;
}

void crossed_list_destroy(CROSSED_LIST **list) {
	if(list == NULL || *list == NULL)
		return;

	int row_cap = (*list)->row_cap;

	for(int i = 0; i < row_cap; i++) {
		crossed_list_erase_tail(*list, (*list)->rows[i], true);
	}

	free((*list)->rows);
	free((*list)->columns);
	free(*list);
	*list = NULL;
}

bool crossed_list_resize(CROSSED_LIST *list, int capacity, bool isRow) {
	if(list == NULL || capacity < 0)
		return false;
 
	int old_cap = isRow ? list->row_cap : list->col_cap;
	NODE **old_arr = isRow ? list->rows : list->columns;
	NODE **new;
	NODE **tail = NULL;
 
	if(capacity == old_cap)
		return true;
 
	if(capacity > old_cap) {
 
		new = realloc(old_arr, capacity * sizeof(*new));
 
		if(new == NULL)
			return false;
 
		for(int i = old_cap; i < capacity; i++) {
			new[i] = NULL;
		}
	}
	else {
		int tail_cap = old_cap - capacity;

		tail = malloc((tail_cap) * sizeof(*tail));
 
		if(tail == NULL)
			return false;
 
		for(int i = 0; i < tail_cap; i++) {
			tail[i] = old_arr[i + capacity];
		}
 
		if(capacity == 0) {
			free(old_arr);
			new = NULL;
		}
		else {
			new = realloc(old_arr, capacity * sizeof(*new));
 
			if(new == NULL) {
				free(tail);
				return false;
			}
		}
	}
 
	if(isRow) {
		list->rows = new;
		list->row_cap = capacity;
	}
	else {
		list->columns = new;
		list->col_cap = capacity;
	}
 
	if(tail != NULL) {
		for(int i = 0; i < old_cap - capacity; i++) {
			crossed_list_erase_tail(list, tail[i], isRow);
		}
 
		free(tail);
	}
 
	return true;
}


bool crossed_list_insert(CROSSED_LIST *list, int i, int j, void *object) {
	if(list == NULL || object == NULL || i < 0 || i >= list->row_cap || j < 0 || j >= list->col_cap)
		return false;
	if(crossed_list_find_node(list, i, j) != NULL)
		return false;

	NODE *left = crossed_list_iterate_right(list, i, j);
	NODE *right = left != NULL ? left->right : list->rows[i];

	NODE *up = crossed_list_iterate_down(list, i, j);
	NODE *down = up != NULL ? up->down : list->columns[j];

	NODE *node = malloc(sizeof(*node));

	if(node == NULL)
		return false;

	node->i = i;
	node->j = j;
	node->object = object;

	node->left = left;
	node->right = right;

	if(left != NULL) {
		left->right = node;
	}
	else {
		list->rows[i] = node;
	}

	if(right != NULL) {
		right->left = node;
	}

	node->up = up;
	node->down = down;

	if(up != NULL) {
		up->down = node;
	}
	else {
		list->columns[j] = node;
	}

	if(down != NULL)
		down->up = node;

	list->size++;

	return true;
}

void* crossed_list_get(CROSSED_LIST *list, int i, int j) {
	NODE *node = crossed_list_find_node(list, i, j);

	if(node == NULL)
		return NULL;

	return node->object;
}

void* crossed_list_find(CROSSED_LIST *list, void *target, int (*cmp)(void*, void*), int *i, int *j) {
	if(list == NULL || target == NULL || cmp == NULL)
		return NULL;

	int row_cap = list->row_cap;

	for(int row = 0; row < row_cap; row++) {
		NODE *idx = list->rows[row];

		while(idx != NULL) {
			if(idx->object != NULL && cmp(idx->object, target) == 0) {
				if(i != NULL)
					*i = idx->i;

				if(j != NULL)
					*j = idx->j;

				return idx->object;
			}

			idx = idx->right;
		}
	}

	return NULL;
}

void* crossed_list_remove(CROSSED_LIST *list, int i, int j) {
	
	NODE *node = crossed_list_find_node(list, i, j);

	if(node == NULL)
		return NULL;

	void *object = node->object;

	crossed_list_unlink(list, node);
	free(node);

	return object;
}

bool crossed_list_remove_line(CROSSED_LIST *list, int index, bool isRow) {
	if(list == NULL || index < 0)
		return false;

	if(isRow) {
		if(index >= list->row_cap)
			return false;

		return crossed_list_erase_tail(list, list->rows[index], true);
	}
	else {
		if(index >= list->col_cap)
			return false;

		return crossed_list_erase_tail(list, list->columns[index], false);
	}
}

int crossed_list_size(CROSSED_LIST *list) {
	return list == NULL ? 0 : list->size;
}

int crossed_list_capacity(CROSSED_LIST *list, bool isRow) {
	if(list == NULL) 
		return 0;

	return isRow ? list->row_cap : list->col_cap;
}

bool crossed_list_empty(CROSSED_LIST *list) {
	return list == NULL ? true : list->size == 0;
}

bool crossed_list_visit(CROSSED_LIST *list, bool (*visitor)(int i, int j, void *object, void *context), void *context) {
	if(list == NULL || visitor == NULL)
		return false;

	for(int i = 0; i < list->row_cap; i++) {
		for(NODE *node = list->rows[i]; node != NULL; node = node->right) {
			if(!visitor(node->i, node->j, node->object, context))
				return false;
		}
	}

	return true;
}

bool crossed_list_visit_line(CROSSED_LIST *list, int index, bool isRow, bool (*visitor)(int i, int j, void *object, void *context), void *context) {
	if(list == NULL || visitor == NULL || index < 0)
		return false;

	if(isRow) {
		if(index >= list->row_cap)
			return false;

		for(NODE *node = list->rows[index]; node != NULL; node = node->right) {
			if(!visitor(node->i, node->j, node->object, context))
				return false;
		}
	}
	else {
		if(index >= list->col_cap)
			return false;

		for(NODE *node = list->columns[index]; node != NULL; node = node->down) {
			if(!visitor(node->i, node->j, node->object, context))
				return false;
		}
	}

	return true;
}
