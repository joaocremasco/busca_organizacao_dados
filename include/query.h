#ifndef TRABALHO_QUERY_H
#define TRABALHO_QUERY_H

#include "trabalho/collection.h"

typedef struct query QUERY;

QUERY *query_load(const char *path, char *error, size_t error_size);
void query_destroy(QUERY **query);
bool query_matches(const QUERY *query, ITEM *item, double *similarity);
COLLECTION_OPTIONS query_collection_options(const QUERY *query);
bool query_has_group(const QUERY *query);
bool query_has_cross(const QUERY *query);

#endif
