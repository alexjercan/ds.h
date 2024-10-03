#define DS_SB_IMPLEMENTATION
#include "../ds.h"

int read_file(const char *filename, char **buffer) {
    int result = 0;

    FILE *file = NULL;
    if (filename != NULL) {
        file = fopen(filename, "r");
        if (file == NULL) {
            DS_LOG_ERROR("Failed to open file: %s", filename);
            return_defer(1);
        }
    } else {
        file = stdin;
    }

    ds_string_builder sb;
    ds_string_builder_init(&sb);
    char line[LINE_MAX];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (ds_string_builder_append(&sb, line) != 0) {
            DS_LOG_ERROR("Failed to append line to string builder");
            return_defer(1);
        }
    }

    if (ds_string_builder_build(&sb, buffer) != 0) {
        DS_LOG_ERROR("Failed to build string from string builder");
        return_defer(1);
    }

defer:
    if (filename != NULL && file != NULL)
        fclose(file);
    ds_string_builder_free(&sb);
    return result;
}

int main(int argc, char *argv[]) {
    int result = 0;

    char *buffer = NULL;
    if (argc == 2) {
        if (read_file(argv[1], &buffer) != 0) {
            DS_LOG_ERROR("Failed to read file: %s", argv[1]);
            return_defer(1);
        }
    } else {
        if (read_file(NULL, &buffer) != 0) {
            DS_LOG_ERROR("Failed to read from stdin");
            return_defer(1);
        }
    }

    printf("The buffer is:\n---\n%s\n---\n", buffer);
defer:
    if (buffer != NULL)
        free(buffer);
    return result;
}
