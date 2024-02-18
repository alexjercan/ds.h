#define DS_SS_IMPLEMENTATION
#include "../ds.h"

#define LINE_MAX 2048

int main() {
    int result = 0;

    char line[LINE_MAX];
    if (fgets(line, sizeof(line), stdin) == NULL) {
        DS_LOG_ERROR("Failed to read line from stdin");
        return_defer(1);
    }

    ds_string_slice slice;
    ds_string_slice_init(&slice, line, strlen(line));

    ds_string_slice token;
    while (ds_string_slice_tokenize(&slice, ',', &token) == 0) {
        char *token_str = NULL;
        if (ds_string_slice_to_owned(&token, &token_str) != 0) {
            DS_LOG_ERROR("Failed to convert token to owned string");
            return_defer(1);
        }

        printf("Token: '%s'\n", token_str);
        free(token_str);
    }

defer:
    ds_string_slice_free(&slice);
    return result;
}
