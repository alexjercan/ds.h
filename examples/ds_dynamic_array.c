#define DS_LIST_ALLOCATOR_IMPLEMENTATION
#define DS_DA_IMPLEMENTATION
#define DS_DA_INIT_CAPACITY 16
#include "../ds.h"

int main() {
    int result = 0;

    char memory[1024] = {0};
    DS_ALLOCATOR allocator = {0};
    DS_INIT_ALLOCATOR(&allocator, memory, 1024);

    ds_dynamic_array array = {0};
    ds_dynamic_array_init_allocator(&array, sizeof(int), &allocator);

    for (int i = 0; i < 10; i++) {
        ds_dynamic_array_append(&array, &i);
    }

    int sum = 0;
    for (int i = 0; i < 10; i++) {
        int value;
        if (ds_dynamic_array_get(&array, i, &value) != 0) {
            return_defer(1);
        }
        DS_LOG_INFO("Value: %d", value);
        sum += value;
    }

    DS_LOG_INFO("Result: %d\n", sum);

defer:
    ds_dynamic_array_free(&array);
    return result;
}
