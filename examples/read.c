#define DS_SB_IMPLEMENTATION
#define DS_IO_IMPLEMENTATION
#include "../ds.h"

int main(int argc, char *argv[]) {
    int result = 0;

    char *buffer = NULL;
    if (argc == 2) {
        if (ds_io_read(argv[1], &buffer, "r") < 0) {
            DS_LOG_ERROR("Failed to read file: %s", argv[1]);
            return_defer(1);
        }
    } else {
        if (ds_io_read(NULL, &buffer, "r") < 0) {
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
