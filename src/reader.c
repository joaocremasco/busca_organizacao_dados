#include "../include/reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct reader {
  char *path;
  char *reader_name;
  FILE *file;            /* o CSV aberto */
  char delimiter;        /* ',' ou ';' ou tab, detectado no cabeçalho */
  char **columns;        /* nomes das colunas (cada um alocado) */
  size_t column_count;
  char *line;             /* buffer da linha, reaproveitado a cada reader_next */
  size_t line_capacity;
  char error[256];       /* mensagem do último erro */
};

static char *copy_string(const char *text) {
    size_t length = strlen(text) + 1;
    char *copy = malloc(length);

    if (copy != NULL)
        memcpy(copy, text, length);

    return copy;
}

/* Lê um registro completo, incluindo quebras de linha dentro de aspas. */
static int read_line(READER *reader) {
    size_t length = 0;
    int in_quotes = 0;
    int c;

    if (reader->line_capacity == 0) {
        reader->line_capacity = 128;
        reader->line = malloc(reader->line_capacity);
        if (reader->line == NULL) {
            reader->line_capacity = 0;
            snprintf(reader->error, sizeof(reader->error), "Falha ao alocar o buffer da linha");
            return -1;
        }
    }

    while ((c = fgetc(reader->file)) != EOF) {
        if (c == '\n' && !in_quotes)
            break;

        if (c == '"') {
            if (in_quotes) {
                int next = fgetc(reader->file);
                if (next == '"') {
                    if (length + 2 >= reader->line_capacity)
                        goto grow_for_pair;
                    reader->line[length++] = '"';
                    reader->line[length++] = '"';
                    continue;
                }
                in_quotes = 0;
                if (next != EOF)
                    ungetc(next, reader->file);
            } else {
                in_quotes = 1;
            }
        }

        if (length + 1 >= reader->line_capacity) {
            size_t capacity = reader->line_capacity * 2;
            char *line = realloc(reader->line, capacity);
            if (line == NULL) {
                snprintf(reader->error, sizeof(reader->error), "Falha ao ampliar o buffer da linha");
                return -1;
            }
            reader->line = line;
            reader->line_capacity = capacity;
        }
        reader->line[length++] = (char)c;
        continue;

grow_for_pair:
        {
            size_t capacity = reader->line_capacity * 2;
            char *line = realloc(reader->line, capacity);
            if (line == NULL) {
                snprintf(reader->error, sizeof(reader->error), "Falha ao ampliar o buffer da linha");
                return -1;
            }
            reader->line = line;
            reader->line_capacity = capacity;
            reader->line[length++] = '"';
            reader->line[length++] = '"';
        }
    }

    if (ferror(reader->file)) {
        snprintf(reader->error, sizeof(reader->error), "Erro ao ler o arquivo");
        return -1;
    }
    if (length == 0 && c == EOF)
        return 0;
    if (in_quotes) {
        snprintf(reader->error, sizeof(reader->error), "Campo CSV com aspas não fechadas");
        return -1;
    }
    if (length > 0 && reader->line[length - 1] == '\r')
        length--;
    reader->line[length] = '\0';
    return 1;
}

/* Conta delimitadores fora de campos entre aspas. */
static char detect_delimiter(const char *line) {
    const char candidates[] = {',', ';', '\t'};
    size_t counts[3] = {0, 0, 0};
    int in_quotes = 0;

    for (size_t i = 0; line[i] != '\0'; i++) {
        if (line[i] == '"') {
            if (in_quotes && line[i + 1] == '"') {
                i++;
                continue;
            }
            in_quotes = !in_quotes;
        } else if (!in_quotes) {
            for (size_t j = 0; j < 3; j++)
                if (line[i] == candidates[j]) counts[j]++;
        }
    }

    size_t best = 0;
    for (size_t i = 1; i < 3; i++)
        if (counts[i] > counts[best]) best = i;
    return candidates[best];
}

/* Divide e desescapa um registro in-place; os valores apontam para line. */
static int split_record(char *line, char delimiter, char ***values_out, size_t *count_out) {
    size_t capacity = 8, count = 0;
    char **values = malloc(capacity * sizeof(*values));
    char *read = line, *write = line;
    if (values == NULL) return 0;

    for (;;) {
        if (count == capacity) {
            capacity *= 2;
            char **grown = realloc(values, capacity * sizeof(*values));
            if (grown == NULL) { free(values); return 0; }
            values = grown;
        }
        values[count++] = write;
        int quoted = (*read == '"');
        int has_more = 0;
        if (quoted) read++;

        while (*read != '\0') {
            if (quoted && *read == '"' && read[1] == '"') {
                *write++ = '"';
                read += 2;
            } else if (quoted && *read == '"') {
                quoted = 0;
                read++;
                if (*read != delimiter && *read != '\0') { free(values); return 0; }
            } else if (!quoted && *read == delimiter) {
                read++;
                has_more = 1;
                break;
            } else {
                *write++ = *read++;
            }
        }
        if (quoted) { free(values); return 0; }
        *write++ = '\0';
        if (!has_more) break;
    }
    *values_out = values;
    *count_out = count;
    return 1;
}

static int read_header(READER *reader) {
    if (read_line(reader) != 1) {
        if (reader->error[0] == '\0')
            snprintf(reader->error, sizeof(reader->error), "O arquivo está vazio");
        return 0;
    }
    reader->delimiter = detect_delimiter(reader->line);
    char **values = NULL;
    if (!split_record(reader->line, reader->delimiter, &values, &reader->column_count)) {
        snprintf(reader->error, sizeof(reader->error), "Cabeçalho CSV inválido ou sem memória");
        return 0;
    }
    reader->columns = calloc(reader->column_count, sizeof(*reader->columns));
    if (reader->columns == NULL) {
        free(values);
        snprintf(reader->error, sizeof(reader->error), "Falha ao alocar nomes das colunas");
        return 0;
    }
    for (size_t i = 0; i < reader->column_count; i++) {
        reader->columns[i] = copy_string(values[i]);
        if (reader->columns[i] == NULL) {
            free(values);
            snprintf(reader->error, sizeof(reader->error), "Falha ao copiar nome de coluna");
            return 0;
        }
    }
    free(values);
    return 1;
}

READER *reader_open(const char *path, const char *reader_name) {
    READER *reader;

    if (path == NULL || reader_name == NULL)
        return NULL;

    reader = calloc(1, sizeof(*reader));
    if (reader == NULL)
        return NULL;

    reader->path = copy_string(path);
    reader->reader_name = copy_string(reader_name);
    if (reader->path == NULL || reader->reader_name == NULL) {
        snprintf(reader->error, sizeof(reader->error), "Falha ao alocar dados do leitor");
        reader_close(&reader);
        return NULL;
    }

    reader->file = fopen(path, "r");
    if (reader->file == NULL) {
        reader_close(&reader);
        return NULL;
    }

    if (!read_header(reader)) {
        reader_close(&reader);
        return NULL;
    }

    return reader;
}

void reader_close(READER **reader){
  if(reader == NULL || *reader == NULL)
    return;

  if ((*reader)->file != NULL)
    fclose((*reader)->file);

  for (size_t i = 0; i < (*reader)->column_count; i++)
    free((*reader)->columns[i]);

  free((*reader)->path);
  free((*reader)->reader_name);
  free((*reader)->columns);
  free((*reader)->line);
  free(*reader);
  *reader = NULL;
}

READER_STATUS reader_next(READER *reader, ITEM_STORE *store, ITEM **item) {
    if (reader == NULL || store == NULL || item == NULL)
        return READER_ERROR;

    *item = NULL;

    int status = read_line(reader);

    if (status == 0)
        return READER_EOF;

    if (status < 0)
        return READER_ERROR;

    char **values = NULL;
    size_t value_count = 0;
    if (!split_record(reader->line, reader->delimiter, &values, &value_count)) {
        snprintf(reader->error, sizeof(reader->error), "Registro CSV inválido ou sem memória");
        return READER_ERROR;
    }
    if (value_count != reader->column_count) {
        free(values);
        snprintf(reader->error, sizeof(reader->error), "Quantidade de campos incorreta");
        return READER_ERROR;
    }

    ITEM *new_item = item_create(store, reader->column_count);

    if (new_item == NULL) {
        free(values);
        return READER_ERROR;
    }

    for (size_t idx = 0; idx < reader->column_count; idx++) {
        if (!item_set(new_item, reader->columns[idx], values[idx])) {
            free(values);
            snprintf(reader->error, sizeof(reader->error), "Erro ao guardar o campo");
            return READER_ERROR;
        }
    }
    free(values);

    *item = new_item;
    return READER_OK;
}

size_t reader_column_count(const READER *reader) {
    return reader == NULL ? 0 : reader->column_count;
}

const char *reader_column_name(const READER *reader, size_t idx) {
    return reader != NULL && idx < reader->column_count
               ? reader->columns[idx] : NULL;
}

char reader_delimiter(const READER *reader) {
    return reader == NULL ? ',' : reader->delimiter;
}

const char *reader_error(const READER *reader) {
    return reader == NULL ? "reader inválido" : reader->error;
}
