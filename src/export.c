
#include "export.h"

#include <stdio.h>

typedef struct {
    FILE *file;
    const READER *reader;
    size_t record_limit;
    size_t records_written;
    bool failed;
} WriteContext;

static bool write_csv_field(FILE *file, const char *value, char delimiter) {
    const char *character;
    bool needs_quotes = false;

    if (value == NULL) {
        value = "";
    }
    for (character = value; *character != '\0'; ++character) {
        if (*character == delimiter || *character == '"' ||
            *character == '\n' || *character == '\r') {
            needs_quotes = true;
            break;
        }
    }

    if (needs_quotes && fputc('"', file) == EOF) {
        return false;
    }
    for (character = value; *character != '\0'; ++character) {
        if (*character == '"' && needs_quotes && fputc('"', file) == EOF) {
            return false;
        }
        if (fputc((unsigned char)*character, file) == EOF) {
            return false;
        }
    }
    return !needs_quotes || fputc('"', file) != EOF;
}

static bool write_csv_record(FILE *file, const READER *reader, const ITEM *item) {
    size_t column_index;
    char delimiter = reader_delimiter(reader);

    for (column_index = 0; column_index < reader_column_count(reader); ++column_index) {
        const char *column_name = reader_column_name(reader, column_index);
        const char *value = item == NULL ? column_name : item_get(item, column_name);

        if (column_index != 0 && fputc(delimiter, file) == EOF) {
            return false;
        }
        if (!write_csv_field(file, value, delimiter)) {
            return false;
        }
    }
    return fputc('\n', file) != EOF;
}

static bool write_item(ITEM *item, void *context) {
    WriteContext *write_context = context;

    if (write_context->record_limit != 0 &&
        write_context->records_written >= write_context->record_limit) {
        return false;
    }
    if (!write_csv_record(write_context->file, write_context->reader, item)) {
        write_context->failed = true;
        return false;
    }
    ++write_context->records_written;
    return write_context->record_limit == 0 ||
           write_context->records_written < write_context->record_limit;
}

bool export_write(const char *path, const READER *reader, const COLLECTION *results,
                  size_t top_k, size_t *written) {
    FILE *output_file;
    WriteContext write_context;

    if (path == NULL || reader == NULL || results == NULL) {
        return false;
    }
    output_file = fopen(path, "wb");
    if (output_file == NULL) {
        return false;
    }

    write_context.file = output_file;
    write_context.reader = reader;
    write_context.record_limit = top_k;
    write_context.records_written = 0;
    write_context.failed = false;

    if (!write_csv_record(output_file, reader, NULL)) {
        write_context.failed = true;
    } else if (collection_size(results) != 0) {
        (void)collection_visit(results, write_item, &write_context);
    }
    if (fclose(output_file) != 0) {
        write_context.failed = true;
    }
    if (written != NULL) {
        *written = write_context.records_written;
    }
    return !write_context.failed;
}
