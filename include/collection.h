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

/* As strings das opções são emprestadas e devem existir enquanto a coleção as utilizar. */
typedef struct {
    const char *order_field;
    bool descending;
    const char *group_field;
    const char *subgroup_field;
    const char *cross_field;
    const char *cross_second_field;
} COLLECTION_OPTIONS;

/* Cria a coleção do tipo indicado; retorna NULL se o tipo ou a alocação forem inválidos. */
COLLECTION *collection_create(COLLECTION_TYPE type, const COLLECTION_OPTIONS *options);
/* Libera a coleção e os metadados das estruturas internas. Não libera ITEMs, que pertencem ao
   ITEM_STORE. */
void collection_destroy(COLLECTION **collection);
/* Insere uma referência ao ITEM; retorna false para argumentos inválidos ou falha de inserção. O
   ITEM continua pertencendo ao ITEM_STORE. */
bool collection_insert(COLLECTION *collection, ITEM *item);
/* Remove a referência ao ITEM; retorna false se os argumentos forem inválidos ou o ITEM não
   estiver na coleção. Não libera o ITEM. */
bool collection_remove(COLLECTION *collection, ITEM *item);
/* Devolve a quantidade de ITEMs; retorna 0 se a coleção for NULL. */
size_t collection_size(const COLLECTION *collection);
/* Percorre os ITEMs; retorna true se visitar todos e false se os argumentos forem inválidos ou a
   função de visita parar. */
bool collection_visit(const COLLECTION *collection, COLLECTION_VISITOR visitor, void *context);
/* Percorre o índice cruzado escolhido; retorna true se visitar tudo e false para coleção
   incompatível, argumento inválido ou parada da função de visita. */
bool collection_visit_cross_index(const COLLECTION *collection, bool second_index,
                                  COLLECTION_VISITOR visitor, void *context);
/* Converte um nome de estrutura para COLLECTION_TYPE; retorna false se o nome ou os argumentos
   forem inválidos. */
bool collection_type_parse(const char *name, COLLECTION_TYPE *type);
/* Devolve o nome canônico do tipo; retorna "desconhecido" se não houver tipo correspondente. */
const char *collection_type_name(COLLECTION_TYPE type);

#endif
