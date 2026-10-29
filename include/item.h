#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stddef.h>

typedef struct item_ ITEM;
typedef struct item_store ITEM_STORE;

/* ITEM_STORE é dono dos ITEMs; as coleções armazenam apenas referências. */
/* Cria o armazenamento, que passa a ser dono dos ITEMs registrados; retorna NULL se a alocação
   falhar. */
ITEM_STORE *item_store_create(void);
/* Libera todos os ITEMs registrados e o próprio armazenamento; aceita ponteiro NULL. */
void item_store_destroy(ITEM_STORE **store);
/* Cria e registra um ITEM no armazenamento, que passa a ser seu dono; retorna NULL para
   argumentos inválidos ou falha de alocação. */
ITEM *item_create(ITEM_STORE *store, size_t field_capacity) ;
/* Acrescenta um par nome/valor ao ITEM; retorna false para argumentos inválidos ou falha de
   alocação. */
bool item_set(ITEM *item, const char *name, const char *raw_value);
/* Devolve o valor do campo, pertencente ao ITEM; retorna NULL se o campo não existir ou os
   argumentos forem inválidos. */
const char *item_get(const ITEM *item, const char *name);
/* Converte o campo para double e grava em value; retorna false se não existir, não for numérico
   ou os argumentos forem inválidos. */
bool item_get_number(const ITEM *item, const char *name, double *value);
/* Devolve a quantidade de campos; retorna 0 se item for NULL. */
size_t item_field_count(const ITEM *item);
/* Devolve o nome do campo, pertencente ao ITEM; retorna NULL se o índice ou ITEM forem
   inválidos. */
const char *item_field_name(const ITEM *item, size_t index);
/* Devolve o valor do campo, pertencente ao ITEM; retorna NULL se o índice ou ITEM forem
   inválidos. */
const char *item_field_value(const ITEM *item, size_t index);

#endif
