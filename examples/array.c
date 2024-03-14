#include <stdint.h>
#include <stdio.h>
#define DS_AL_IMPLEMENTATION
#define DS_DA_IMPLEMENTATION
#include "../ds.h"

#define MEMORY_SIZE 8192 * sizeof(int) * 2

int main() {
    int result = 0;

    uint8_t data[MEMORY_SIZE];

    ds_allocator allocator;
    ds_allocator_init(&allocator, data, MEMORY_SIZE);

    ds_dynamic_array array;
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

    printf("Result: %d\n", sum);

defer:
    ds_dynamic_array_free(&array);
    return result;
}
