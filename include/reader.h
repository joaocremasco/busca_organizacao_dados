#ifndef TRABALHO_READER_H
#define TRABALHO_READER_H

#include "item.h"
#include <stddef.h>

typedef struct reader READER;
typedef enum { READER_ERROR = -1, READER_EOF = 0, READER_OK = 1 } READER_STATUS;

// reader_name "games" ativa a correcao do cabecalho
READER *reader_open(const char *path, const char *reader_name);
void reader_close(READER **reader);
// o item novo e do store
READER_STATUS reader_next(READER *reader, ITEM_STORE *store, ITEM **item);
size_t reader_column_count(const READER *reader);
const char *reader_column_name(const READER *reader, size_t index);
// virgula se reader for NULL
char reader_delimiter(const READER *reader);
// a mensagem pertence ao reader
const char *reader_error(const READER *reader);

#endif
