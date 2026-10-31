#ifndef TRABALHO_QUERY_H
#define TRABALHO_QUERY_H

#include "collection.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct query QUERY;

// error pode ser NULL; error_size limita a mensagem gravada.
QUERY *query_load(const char *path, char *error, size_t error_size);
void query_destroy(QUERY **query);
// similarity pode ser NULL se a pontuação calculada não for necessária.
bool query_matches(const QUERY *query, ITEM *item, double *similarity);
// Os campos devolvidos são emprestados da query e permanecem válidos até query_destroy.
COLLECTION_OPTIONS query_collection_options(const QUERY *query);
bool query_has_group(const QUERY *query);
bool query_has_cross(const QUERY *query);
bool query_has_order(const QUERY *query);

#endif
