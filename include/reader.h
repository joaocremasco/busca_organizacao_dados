#ifndef TRABALHO_READER_H
#define TRABALHO_READER_H

#include "item.h"
#include <stddef.h>

typedef struct reader READER;
typedef enum { READER_ERROR = -1, READER_EOF = 0, READER_OK = 1 } READER_STATUS;

// o nome games ativa o ajuste do cabecalho
READER *reader_open(const char *path, const char *reader_name);
void reader_close(READER **reader);
// o item criado pertence ao armazenamento
READER_STATUS reader_next(READER *reader, ITEM_STORE *store, ITEM **item);
size_t reader_column_count(const READER *reader);
const char *reader_column_name(const READER *reader, size_t index);
// devolve virgula quando o leitor e nulo
char reader_delimiter(const READER *reader);
// a mensagem pertence ao leitor
const char *reader_error(const READER *reader);

#endif
