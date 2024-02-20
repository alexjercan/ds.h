#define DS_LL_IMPLEMENTATION
#include "../ds.h"

int main() {
    int result = 0;

    ds_linked_list queue;
    ds_linked_list_init(&queue, sizeof(int));

    for (int i = 0; i < 10; i++) {
        ds_linked_list_push_back(&queue, &i);
    }

    printf("Queue\n");
    for (int i = 0; i < 10; i++) {
        int value;
        if (ds_linked_list_pop_front(&queue, &value) != 0) {
            return_defer(1);
        }
        printf("Value: %d\n", value);
    }

defer:
    ds_linked_list_free(&queue);
    return result;
}


