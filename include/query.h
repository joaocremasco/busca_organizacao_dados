#ifndef TRABALHO_QUERY_H
#define TRABALHO_QUERY_H

#include "collection.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct query QUERY;

// o buffer de erro pode ser nulo
QUERY *query_load(const char *path, char *error, size_t error_size);
void query_destroy(QUERY **query);
// a pontuacao pode ser nula
bool query_matches(const QUERY *query, ITEM *item, double *similarity);
// as strings continuam sendo da query, nao destruir antes da colecao
COLLECTION_OPTIONS query_collection_options(const QUERY *query);
bool query_has_group(const QUERY *query);
bool query_has_cross(const QUERY *query);
bool query_has_order(const QUERY *query);

#endif
