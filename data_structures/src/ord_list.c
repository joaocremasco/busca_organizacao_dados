// Vetor mantido em ordem; a busca da posição usa busca binária.
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

// Cria o vetor ordenado com cmp, que deve devolver negativo, zero ou positivo.
// Devolve NULL se cmp for nulo ou alguma alocação falhar.
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

// Variante que também passa context ao comparador; destroy é opcional.
// Devolve NULL se o comparador for nulo ou alguma alocação falhar.
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

// Usa o comparador com contexto quando configurado; caso contrário, usa cmp.
static int compare_objects(ORD_LIST *sq, void *left, void *right) {
	if(sq->cmp_with_context != NULL)
		return sq->cmp_with_context(left, right, sq->cmp_context);
	return sq->cmp(left, right);
}

// Libera o vetor e a lista; se destroy foi informado, chama-o para cada objeto.
// No projeto, os ITEMs pertencem ao ITEM_STORE e não são liberados pela coleção.
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

// Ajusta a capacidade; ao encolher, descarta os objetos além do novo limite.
// Devolve false para capacidade inválida ou se realloc falhar.
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

// Procura em [low, high) a primeira posição cujo objeto não vem antes de object.
// Devolve -1 para argumentos inválidos.
int ord_list_lower_bound(ORD_LIST *sq, void *object) {
	if(sq == NULL || object == NULL)
		return -1;
	
	int low = 0, high = sq->size;
	
	while(low < high) {

		int mid = low + (high - low)/2;
		
		// O limite superior fica no meio quando ele já pode ser a primeira posição válida.
		
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

// Procura em [low, high) a primeira posição depois de todos os objetos iguais.
// Devolve -1 para argumentos inválidos.
int ord_list_upper_bound(ORD_LIST *sq, void *object) {
	if(sq == NULL || object == NULL)
		return -1;
	
	int low = 0, high = sq->size;
	
	while(low < high) {

		int mid = low + (high - low)/2;
		
		// Igualdade avança low para manter a inserção após os valores repetidos.
		
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

// Usa lower_bound e confirma a igualdade com o comparador da lista.
// Devolve o índice encontrado ou -1 se não houver correspondência.
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

// Insere na posição devolvida por upper_bound; os itens seguintes são deslocados
// para abrir espaço, preservando a ordem de chegada entre chaves iguais.
bool ord_list_insert(ORD_LIST* sq, void *object) {
	if(sq == NULL || object == NULL)
		return false;

	if(sq->size == sq->capacity) {
		if(!ord_list_resize(sq, sq->capacity * 2))
			return false;
	}
	
	// upper_bound mantém a ordem de chegada entre objetos com a mesma chave.
	int idx = ord_list_upper_bound(sq, object);

	int seq_size = sq->size;

	for(int i = seq_size; i > idx; i--) {
		sq->array[i] = sq->array[i - 1];
	}

	sq->size++;

	sq->array[idx] = object;

	return true;
}

// Remove a primeira chave igual a object e devolve o ponteiro sem destruí-lo.
// Devolve NULL se não encontrar o objeto.
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

// Remove pelo índice e devolve o ponteiro sem chamar destroy; NULL indica índice inválido.
void* ord_list_erase_at(ORD_LIST *sq, int idx) {
	if(sq == NULL || idx < 0 || idx >= sq->size)
		return NULL;
	void *obj = sq->array[idx];
	for(int i = idx + 1; i < sq->size; i++)
		sq->array[i - 1] = sq->array[i];
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

// Percorre os objetos na ordem mantida pelo vetor; false também indica parada do visitor.
bool ord_list_visit(ORD_LIST *sq, bool (*visitor)(void *object, void *context), void *context) {
	if(sq == NULL || visitor == NULL)
		return false;

	for(int i = 0; i < sq->size; i++) {
		if(!visitor(sq->array[i], context))
			return false;
	}

	return true;
}
