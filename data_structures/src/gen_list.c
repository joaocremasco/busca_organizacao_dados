// Lista generalizada: cada nó guarda um átomo ou uma sublista.
#include <stdlib.h>
#include "../include/gen_list.h"

typedef struct node_ NODE;

struct node_ {
	NODE *next;
	NODE *prev;
	bool atom; // false indica uma sublista
	void *object;
};

struct gen_list_ {
	int size;
	NODE *front;
	NODE *back;
	void (*destroy)(void*);
};

// Atualiza isAtom apenas quando o chamador pediu essa informação.
static void set_atom(bool *ptr, bool v) {
	if(ptr != NULL)
		*ptr = v;
}

// Cria uma lista vazia; destroy é aplicado aos átomos ao destruir a estrutura.
// Devolve NULL se não houver memória. No projeto, os ITEMs pertencem ao ITEM_STORE.
GEN_LIST* gen_list_init(void (*destroy)(void*)) {

	GEN_LIST *list = malloc(sizeof(*list));
	
	if(list == NULL)
		return NULL;

	list->size = 0;
	list->front = NULL;
	list->back = NULL;
	list->destroy = destroy;

	return list;
}

// Libera os nós e as sublistas; para átomos, usa o callback opcional.
// Os ITEMs do projeto continuam sob responsabilidade do ITEM_STORE.
void gen_list_destroy(GEN_LIST **list) {
	if(list == NULL || *list == NULL)
		return;

	while((*list)->size > 0) {
		bool atom;
		void *obj = gen_list_pop_back(*list, &atom);

		if(atom) {
			if((*list)->destroy != NULL) {
				(*list)->destroy(obj);
			}
		}
		else {
			GEN_LIST *sub = obj;
			gen_list_destroy(&sub);
		}
	}

	free(*list);
	*list = NULL;
}

// Acrescenta object ao fim: atom=true marca um item, false marca uma sublista.
// Devolve false se os argumentos forem inválidos ou faltar memória.
bool gen_list_push_back(GEN_LIST *list, void *object, bool atom) {
	if(list == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(*node));

	if(node == NULL)
		return false;

	node->object = object;
	node->atom = atom;
	node->next = NULL;
	
	if(list->size == 0) {
		node->prev = NULL;
		list->front = node;
	}
	else {
		node->prev = list->back;
		list->back->next = node;
	}

	list->back = node;
	
	list->size++;

	return true;
}

// Remove o último nó sem destruir seu conteúdo; isAtom informa se era átomo.
// Devolve o conteúdo ou NULL se a lista estiver vazia ou for inválida.
void* gen_list_pop_back(GEN_LIST *list, bool *isAtom) {
	
	set_atom(isAtom, false);
	
	if(list == NULL || list->size == 0) {
		return NULL;
	}
	
	void *object = list->back->object;
	set_atom(isAtom, list->back->atom);
	
	NODE *tmp = list->back;
	
	if(tmp->prev != NULL)
		tmp->prev->next = NULL;
	
	list->back = tmp->prev;

	if(list->back == NULL)
		list->front = NULL;
	
	free(tmp);

	list->size--;
	
	return object;
}

// Consulta o último conteúdo sem removê-lo; isAtom recebe seu tipo quando pedido.
// Devolve NULL se a lista estiver vazia ou for inválida.
void* gen_list_back(GEN_LIST *list, bool *isAtom) {
	
	set_atom(isAtom, false);

	if(list == NULL || list->size == 0) {
		return NULL;
	}
	
	set_atom(isAtom, list->back->atom);
	return list->back->object;
}

// Insere no início e registra se object é átomo ou sublista.
// Devolve false se os argumentos forem inválidos ou faltar memória.
bool gen_list_push_front(GEN_LIST *list, void *object, bool atom) {
	if(list == NULL || object == NULL)
		return false;
	
	NODE *node = malloc(sizeof(*node));
	
	if(node == NULL)
		return false;

	node->object = object;
	node->atom = atom;
	node->prev = NULL;
	
	if(list->size == 0) {
		node->next = NULL;
		list->back = node;
	}
	else {
		node->next = list->front;
		list->front->prev = node;
	}

	list->front = node;
	
	list->size++;
	
	return true;
}

// Remove o primeiro nó sem destruir seu conteúdo; isAtom informa o tipo removido.
// Devolve NULL se a lista estiver vazia ou for inválida.
void* gen_list_pop_front(GEN_LIST* list, bool *isAtom) {
	
	set_atom(isAtom, false);

	if(list == NULL || list->size == 0) {
		return NULL;
	}

	void *object = list->front->object;
	set_atom(isAtom, list->front->atom);

	NODE *tmp = list->front;

	if(tmp->next != NULL)
		tmp->next->prev = NULL;

	list->front = list->front->next;

	if(list->front == NULL)
		list->back = NULL;

	free(tmp);

	list->size--;

	return object;
}

// Consulta o primeiro conteúdo sem removê-lo; isAtom recebe seu tipo quando pedido.
// Devolve NULL se a lista estiver vazia ou for inválida.
void* gen_list_front(GEN_LIST* list, bool *isAtom) {
	
	set_atom(isAtom, false);
	
	if(list == NULL || list->size == 0) {
		return NULL;
	}
	
	set_atom(isAtom, list->front->atom);
	return list->front->object;
}

// Procura apenas neste nível e só compara nós do tipo indicado por atom.
// Devolve NULL se não encontrar o conteúdo ou se algum argumento for inválido.
static NODE* find_helper(GEN_LIST *list, void *object, int (*cmp)(void*, void*), bool atom) {
	if(list == NULL || object == NULL || cmp == NULL) {
		return NULL;
	}

	int list_size = list->size;
	NODE *idx = list->front;
	
	for(int i = 0; i < list_size; i++) {
		if(idx->atom == atom && cmp(idx->object, object) == 0) {
			return idx;
		}

		idx = idx->next;
	}
	
	return NULL;
}

// Busca no nível atual pelo conteúdo e pelo tipo; isAtom recebe o tipo encontrado.
// Devolve NULL se não houver correspondência. A lista continua dona dos nós.
void* gen_list_find(GEN_LIST *list, void *object, bool atom, int (*cmp)(void*, void*), bool *isAtom) {
	
	set_atom(isAtom, false);

	if(list == NULL || object == NULL || cmp == NULL) {
		return NULL;
	}

	NODE *res = find_helper(list, object, cmp, atom);
	
	if(res == NULL)
		return NULL;
	
	set_atom(isAtom, res->atom);

	return res->object;
}

// Desvincula o conteúdo encontrado sem destruí-lo; isAtom informa seu tipo.
// Devolve NULL se não encontrar. O chamador passa a cuidar do conteúdo removido.
void* gen_list_erase(GEN_LIST *list, void *object, bool atom, int (*cmp)(void*, void*), bool *isAtom) {
	
	set_atom(isAtom, false);

	if(list == NULL || object == NULL || cmp == NULL) {
		return NULL;
	}
	
	NODE *idx = find_helper(list, object, cmp, atom);

	if(idx == NULL)
		return NULL;

	void *obj = idx->object;
	
	if(idx->prev != NULL) {
		idx->prev->next = idx->next;
	}
	else {
		list->front = idx->next;
	}
	if(idx->next != NULL) {
		idx->next->prev = idx->prev;
	}
	else {
		list->back = idx->prev;
	}
		
	list->size--;
	
	set_atom(isAtom, idx->atom);

	free(idx);

	return obj;
}

// Insere object antes de target no mesmo nível; os flags indicam átomo ou sublista.
// Devolve false se target não existir ou se faltar memória.
bool gen_list_insert(GEN_LIST *list, void *object, bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*)) {
	if(list == NULL || object == NULL || target == NULL || cmp == NULL)
		return false;
		
	NODE *idx = find_helper(list, target, cmp, targ_atom);
	
	if(idx == NULL) {
		return false;
	}

	NODE *node = malloc(sizeof(*node));

	if(node == NULL) {
		return false;
	}

	node->object = object;
	node->atom = obj_atom;
	
	node->next = idx;
	node->prev = idx->prev;

	if(idx->prev != NULL)
		idx->prev->next = node;
	else
		list->front = node;

	idx->prev = node;
	
	list->size++;

	return true;
}

// Troca o conteúdo e o tipo do nó alvo; isAtom informa o tipo que foi substituído.
// Devolve o conteúdo anterior ou NULL se o alvo não for encontrado.
void* gen_list_set(GEN_LIST *list, void *object, bool obj_atom, void *target, bool targ_atom, int (*cmp)(void*, void*), bool *isAtom) {
	
	set_atom(isAtom, false);

	if(list == NULL || object == NULL || target == NULL || cmp == NULL) {
		return NULL;
	}
		
	NODE *idx = find_helper(list, target, cmp, targ_atom);
	
	if(idx == NULL) {
		return NULL;
	}
	
	void *obj = idx->object;
	set_atom(isAtom, idx->atom);

	idx->object = object;
	idx->atom = obj_atom;

	return obj;
}

int gen_list_size(GEN_LIST *list) {
	return list == NULL ? 0 : list->size;
}

bool gen_list_empty(GEN_LIST *list) {
	return list == NULL ? true : list->size == 0;
}

// Desce recursivamente nas sublistas e chama visitor apenas para os átomos.
// Devolve false se a lista for inválida ou se visitor pedir para parar.
bool gen_list_visit(GEN_LIST *list, bool (*visitor)(void *object, void *context), void *context) {
	if(list == NULL || visitor == NULL)
		return false;

	for(NODE *node = list->front; node != NULL; node = node->next) {
		if(node->atom) {
			if(!visitor(node->object, context))
				return false;
		}
		else if(node->object != NULL && !gen_list_visit(node->object, visitor, context)) {
			return false;
		}
	}

	return true;
}
