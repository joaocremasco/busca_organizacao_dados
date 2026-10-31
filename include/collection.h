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

// Guarda ponteiros emprestados; as strings precisam durar enquanto existir a coleção.
typedef struct {
    const char *order_field;
    bool descending;
    const char *group_field;
    const char *subgroup_field;
    const char *cross_field;
    const char *cross_second_field;
} COLLECTION_OPTIONS;

// options pode ser NULL quando a estrutura não depende de campos da consulta.
COLLECTION *collection_create(COLLECTION_TYPE type, const COLLECTION_OPTIONS *options);
// Libera a estrutura e seus índices, mas não os ITEMs, que pertencem ao ITEM_STORE.
void collection_destroy(COLLECTION **collection);
// Guarda uma referência ao ITEM; devolve false se a estrutura ou seus campos obrigatórios falharem.
bool collection_insert(COLLECTION *collection, ITEM *item);
// Na pilha remove apenas o topo e na fila apenas o início.
bool collection_remove(COLLECTION *collection, ITEM *item);
size_t collection_size(const COLLECTION *collection);
// Para e devolve false se visitor interromper o percurso ou os argumentos forem inválidos.
bool collection_visit(const COLLECTION *collection, COLLECTION_VISITOR visitor, void *context);
// Percorre o primeiro índice por linha e o segundo por coluna.
bool collection_visit_cross_index(const COLLECTION *collection, bool second_index,
                                  COLLECTION_VISITOR visitor, void *context);
// Aceita os nomes convencionais em inglês e os nomes do enunciado.
bool collection_type_parse(const char *name, COLLECTION_TYPE *type);
const char *collection_type_name(COLLECTION_TYPE type);

#endif
