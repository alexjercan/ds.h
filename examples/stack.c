#define DS_LL_IMPLEMENTATION
#include "../ds.h"

int main() {
    int result = 0;

    ds_linked_list stack;
    ds_linked_list_init(&stack, sizeof(int));

    for (int i = 0; i < 10; i++) {
        ds_linked_list_push_back(&stack, &i);
    }

    printf("Stack\n");
    for (int i = 0; i < 10; i++) {
        int value;
        if (ds_linked_list_pop_back(&stack, &value) != 0) {
            return_defer(1);
        }
        printf("Value: %d\n", value);
    }

defer:
    ds_linked_list_free(&stack);
    return result;
}

