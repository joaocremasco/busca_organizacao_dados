// vetor sempre ordenado, busca e insercao usam busca binaria
#include <stdlib.h>
#include "../include/ord_list.h"

#define STARTING_CAPACITY 8

struct ord_list_ {
	int size;
	int capacity;
	void **array;
	void (*destroy)(void*);
	int (*cmp)(void*, void*);
	ORD_LIST_COMPARE cmp_with_context;
	void *cmp_context;
};

ORD_LIST* ord_list_init(void (*destroy)(void*), int (*cmp)(void*, void*)) {
	if(cmp == NULL)
		return NULL;

	ORD_LIST* sq = malloc(sizeof(*sq));
	
	if(sq == NULL)
		return NULL;

	sq->size = 0;
	sq->capacity = STARTING_CAPACITY;
	sq->array = malloc(sizeof(*sq->array) * STARTING_CAPACITY);
	sq->destroy = destroy;
	sq->cmp = cmp;
	sq->cmp_with_context = NULL;
	sq->cmp_context = NULL;

	if(sq->array == NULL) {
		free(sq);
		return NULL;
	}

	return sq;
}

ORD_LIST* ord_list_init_with_context(void (*destroy)(void*),
                                     ORD_LIST_COMPARE cmp, void *context) {
	if(cmp == NULL)
		return NULL;

	ORD_LIST* sq = malloc(sizeof(*sq));
	if(sq == NULL)
		return NULL;

	sq->size = 0;
	sq->capacity = STARTING_CAPACITY;
	sq->array = malloc(sizeof(*sq->array) * STARTING_CAPACITY);
	sq->destroy = destroy;
	sq->cmp = NULL;
	if(sq->array == NULL) {
		free(sq);
		return NULL;
	}

	sq->cmp_with_context = cmp;
	sq->cmp_context = context;
	return sq;
}

// usa o cmp com contexto se tiver, senao o simples
static int compare_objects(ORD_LIST *sq, void *left, void *right) {
	if(sq->cmp_with_context != NULL)
		return sq->cmp_with_context(left, right, sq->cmp_context);
	return sq->cmp(left, right);
}

void ord_list_destroy(ORD_LIST **sq) {
	if(sq == NULL || *sq == NULL)
		return;
	
	int sq_size = (*sq)->size;
	
	if((*sq)->destroy != NULL) {
		for(int i = 0; i < sq_size; i++) {
			(*sq)->destroy((*sq)->array[i]);
		}
	}

	free((*sq)->array);

	free(*sq);
	*sq = NULL;
}

bool ord_list_resize(ORD_LIST *sq, int capacity) {
	if(sq == NULL || capacity <= 0)
		return false;
	
	if(capacity < sq->size) {
		if(sq->destroy != NULL) {
			for(int i = capacity; i < sq->size; i++) {
				sq->destroy(sq->array[i]);
			}
		}
		sq->size = capacity;
	}

	void **tmp_array = realloc(sq->array, capacity * sizeof(*tmp_array));
		
	if(tmp_array == NULL)
		return false;

	sq->array = tmp_array;

	sq->capacity = capacity;
	
	return true;
}

int ord_list_lower_bound(ORD_LIST *sq, void *object) {
	if(sq == NULL || object == NULL)
		return -1;
	
	int low = 0, high = sq->size;
	
	while(low < high) {

		int mid = low + (high - low)/2;
		
		// res < 0 quer dizer que o elemento do meio vem antes do alvo
		
		int res = compare_objects(sq, sq->array[mid], object);

		if(res < 0) {
			low = mid + 1;
		}
		else {
			high = mid;	
		}
	}

	return low;
}

int ord_list_upper_bound(ORD_LIST *sq, void *object) {
	if(sq == NULL || object == NULL)
		return -1;
	
	int low = 0, high = sq->size;
	
	while(low < high) {

		int mid = low + (high - low)/2;
		
		// res > 0 quer dizer que o do meio ja passou do alvo
		
		int res = compare_objects(sq, sq->array[mid], object);

		if(res > 0) {
			high = mid;
		}
		else {
			low = mid + 1;	
		}
	}

	return low;
}

int ord_list_find(ORD_LIST *sq, void *object) {
	if(sq == NULL || object == NULL)
		return -1;
	
	int idx = ord_list_lower_bound(sq, object);
	
	if(idx == sq->size)
		return -1;

	if(compare_objects(sq, ord_list_get(sq, idx), object) == 0)
		return idx;

	return -1;
}

bool ord_list_insert(ORD_LIST* sq, void *object) {
	if(sq == NULL || object == NULL)
		return false;

	if(sq->size == sq->capacity) {
		if(!ord_list_resize(sq, sq->capacity * 2))
			return false;
	}
	
	// upper_bound deixa os iguais na ordem em que entraram
	int idx = ord_list_upper_bound(sq, object);

	int seq_size = sq->size;

	for(int i = seq_size; i > idx; i--) {
		sq->array[i] = sq->array[i - 1];
	}

	sq->size++;

	sq->array[idx] = object;

	return true;
}

void* ord_list_erase(ORD_LIST* sq, void *object) {
	if(sq == NULL || object == NULL)
		return NULL;
	
	int idx = ord_list_find(sq, object);

	if(idx == -1)
		return NULL;

	int seq_size = sq->size;

	void *obj = sq->array[idx];

	for(int i = idx + 1; i < seq_size; i++) {
		sq->array[i - 1] = sq->array[i];
	}
	
	sq->size--;

	return obj;
}

void* ord_list_get(ORD_LIST *sq, int idx) {
	if(sq == NULL || idx < 0 || idx >= sq->size)
		return NULL;

	return sq->array[idx];
}

int ord_list_size(ORD_LIST *sq) {
	return sq == NULL ? 0 : sq->size;
}

bool ord_list_empty(ORD_LIST *sq) {
	return sq == NULL ? true : sq->size == 0;
}

bool ord_list_visit(ORD_LIST *sq, bool (*visitor)(void *object, void *context), void *context) {
	if(sq == NULL || visitor == NULL)
		return false;

	for(int i = 0; i < sq->size; i++) {
		if(!visitor(sq->array[i], context))
			return false;
	}

	return true;
}
