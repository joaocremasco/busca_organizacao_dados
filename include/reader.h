#ifndef TRABALHO_READER_H
#define TRABALHO_READER_H

#include "item.h"
#include <stddef.h>

typedef struct reader READER;
typedef enum { READER_ERROR = -1, READER_EOF = 0, READER_OK = 1 } READER_STATUS;

/* Abre o CSV e lê o cabeçalho; retorna NULL se o arquivo, cabeçalho ou alocação forem inválidos. */
READER *reader_open(const char *path, const char *reader_name);
/* Fecha o arquivo e libera os dados do leitor; aceita ponteiro NULL. */
void reader_close(READER **reader);
/* Lê um registro e cria o ITEM no ITEM_STORE; retorna READER_OK, READER_EOF ou READER_ERROR. O
   ITEM pertence ao armazenamento. */
READER_STATUS reader_next(READER *reader, ITEM_STORE *store, ITEM **item);
/* Devolve a quantidade de colunas; retorna 0 se reader for NULL. */
size_t reader_column_count(const READER *reader);
/* Devolve o nome da coluna, pertencente ao leitor; retorna NULL para leitor ou índice inválido. */
const char *reader_column_name(const READER *reader, size_t index);
/* Devolve o delimitador detectado; retorna caractere nulo se reader for NULL. */
char reader_delimiter(const READER *reader);
/* Devolve a mensagem de erro, pertencente ao leitor; retorna NULL se reader for NULL. */
const char *reader_error(const READER *reader);

#endif
