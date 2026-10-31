#ifndef TRABALHO_READER_H
#define TRABALHO_READER_H

#include "item.h"
#include <stddef.h>

typedef struct reader READER;
typedef enum { READER_ERROR = -1, READER_EOF = 0, READER_OK = 1 } READER_STATUS;

// reader_name igual a "games" ativa a correção do cabeçalho dessa base.
READER *reader_open(const char *path, const char *reader_name);
void reader_close(READER **reader);
// O ITEM criado pertence ao ITEM_STORE, que deve continuar válido até liberá-lo.
READER_STATUS reader_next(READER *reader, ITEM_STORE *store, ITEM **item);
size_t reader_column_count(const READER *reader);
const char *reader_column_name(const READER *reader, size_t index);
// Devolve vírgula se reader for NULL.
char reader_delimiter(const READER *reader);
// A mensagem devolvida pertence ao READER.
const char *reader_error(const READER *reader);

#endif
