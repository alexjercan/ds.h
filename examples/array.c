#define DS_DA_IMPLEMENTATION
#include "../ds.h"

int main() {
    int result = 0;

    ds_dynamic_array array;
    ds_dynamic_array_init(&array, sizeof(int));

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
