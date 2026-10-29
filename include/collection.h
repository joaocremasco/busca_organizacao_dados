#ifndef TRABALHO_COLLECTION_H
#define TRABALHO_COLLECTION_H

#include "item.h"
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    COLLECTION_STACK, COLLECTION_QUEUE, COLLECTION_ARRAY_LIST, COLLECTION_LINKED_LIST,
    COLLECTION_SENTINEL_LIST, COLLECTION_SORTED_LIST, COLLECTION_GROUPED_LIST,
    COLLECTION_CROSS_LIST
} COLLECTION_TYPE;

typedef struct collection COLLECTION;
typedef bool (*COLLECTION_VISITOR)(ITEM *item, void *context);

// so guarda os ponteiros, as strings tem que viver mais que a colecao
typedef struct {
    const char *order_field;
    bool descending;
    const char *group_field;
    const char *subgroup_field;
    const char *cross_field;
    const char *cross_second_field;
} COLLECTION_OPTIONS;

// options pode ser NULL
COLLECTION *collection_create(COLLECTION_TYPE type, const COLLECTION_OPTIONS *options);
// nao libera os itens, quem libera e o ITEM_STORE
void collection_destroy(COLLECTION **collection);
// guarda so a referencia, falha tambem se faltar GROUP ou CROSS na colecao que precisa
bool collection_insert(COLLECTION *collection, ITEM *item);
// pilha e fila so removem o item do topo e do inicio
bool collection_remove(COLLECTION *collection, ITEM *item);
size_t collection_size(const COLLECTION *collection);
// se o visitor devolver false ja para por ali
bool collection_visit(const COLLECTION *collection, COLLECTION_VISITOR visitor, void *context);
// so pra lista cruzada, percorre linha por linha ou coluna por coluna
bool collection_visit_cross_index(const COLLECTION *collection, bool second_index,
                                  COLLECTION_VISITOR visitor, void *context);
// aceita os nomes em ingles e os do enunciado
bool collection_type_parse(const char *name, COLLECTION_TYPE *type);
const char *collection_type_name(COLLECTION_TYPE type);

#endif
