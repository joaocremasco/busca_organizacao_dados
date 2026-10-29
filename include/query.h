#ifndef TRABALHO_QUERY_H
#define TRABALHO_QUERY_H

#include "collection.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct query QUERY;

/* Lê e valida a consulta; retorna NULL em erro e, se possível, grava a descrição em error. */
QUERY *query_load(const char *path, char *error, size_t error_size);
/* Libera a consulta e seus campos alocados; aceita ponteiro NULL. */
void query_destroy(QUERY **query);
/* Avalia filtros e similaridade e grava a pontuação se similarity não for NULL; retorna false se
   o ITEM não for aceito ou houver argumento inválido. */
bool query_matches(const QUERY *query, ITEM *item, double *similarity);
/* Devolve as opções por valor; as strings apontadas continuam pertencendo à consulta. */
COLLECTION_OPTIONS query_collection_options(const QUERY *query);
/* Retorna true se a consulta definir GROUP; false se não definir ou query for NULL. */
bool query_has_group(const QUERY *query);
/* Retorna true se a consulta definir CROSS; false se não definir ou query for NULL. */
bool query_has_cross(const QUERY *query);
/* Retorna true se a consulta definir ORDER; false se não definir ou query for NULL. */
bool query_has_order(const QUERY *query);

#endif
