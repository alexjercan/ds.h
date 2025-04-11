#define DS_IO_IMPLEMENTATION
#define DS_DA_INIT_CAPACITY 16
#include "../ds.h"

int main() {
    int result = 0;

    char *buffer = NULL;

    long size = ds_io_read(NULL, &buffer, "r");
    if (size < 0) {
        DS_LOG_ERROR("Failed to read file");
        return_defer(1);
    }

    size = ds_io_write(NULL, buffer, size, "w");
    if (size < 0) {
        DS_LOG_ERROR("Failed to write file");
        return_defer(1);
    }

    DS_LOG_INFO("Wrote %ld bytes", size);

defer:
    if (buffer != NULL) {
        DS_FREE(NULL, buffer);
    }
    return result;
}

