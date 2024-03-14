#define DS_AL_IMPLEMENTATION
#define DS_LL_IMPLEMENTATION
#include "../ds.h"

#define MEMORY_SIZE 8192 * sizeof(int) * 2

int main() {
    int result = 0;

    uint8_t data[MEMORY_SIZE];

    ds_allocator allocator;
    ds_allocator_init(&allocator, data, MEMORY_SIZE);

    ds_linked_list queue;
    ds_linked_list_init_allocator(&queue, sizeof(int), &allocator);

    for (int i = 0; i < 10; i++) {
        ds_linked_list_push_back(&queue, &i);
    }

    ds_allocator_dump(&allocator);

    printf("Queue\n");
    for (int i = 0; i < 10; i++) {
        int value;
        if (ds_linked_list_pop_front(&queue, &value) != 0) {
            return_defer(1);
        }
        printf("Value: %d\n", value);
    }

    ds_allocator_dump(&allocator);

defer:
    ds_linked_list_free(&queue);
    return result;
}
