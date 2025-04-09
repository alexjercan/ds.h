#define DS_LIST_ALLOCATOR_IMPLEMENTATION
#define DS_SB_IMPLEMENTATION
#define DS_DA_INIT_CAPACITY 16
#include "../ds.h"

int main() {
    int result = 0;

    char memory[1024] = {0};
    DS_ALLOCATOR allocator = {0};
    DS_INIT_ALLOCATOR(&allocator, memory, 1024);

    ds_string_builder sb = {0};
    ds_string_builder_init_allocator(&sb, &allocator);

    ds_string_builder_append(&sb, "Hello, %d!\n", 69);
    ds_string_builder_appendn(&sb, "World", 5);
    ds_string_builder_appendc(&sb, '\n');

    char *str = NULL;
    if (ds_string_builder_build(&sb, &str) != DS_OK) {
        DS_LOG_ERROR("Failed to build string");
        return_defer(1);
    }

    DS_LOG_INFO("String: %s", str);

defer:
    if (str != NULL) {
        DS_FREE(&allocator, str);
    }
    return result;
}
