#include <stdint.h>
#include <stdio.h>
#define DS_AL_IMPLEMENTATION
#include "../ds.h"

#define ALLOC_SIZE 1024

int main() {
    uint8_t data[ALLOC_SIZE];
    ds_allocator allocator;
    ds_allocator_init(&allocator, data, ALLOC_SIZE);

    void *ptr1 = ds_allocator_alloc(&allocator, 128);
    void *ptr2 = ds_allocator_alloc(&allocator, 128);
    void *ptr3 = ds_allocator_alloc(&allocator, 128);

    printf("Expecting 3 blocks (alloc 128, alloc 128, alloc 128)\n");
    ds_allocator_dump(&allocator);

    ds_allocator_free(&allocator, ptr2);

    printf("\nExpecting 3 blocks, middle one is free (free second block)\n");
    ds_allocator_dump(&allocator);

    void *ptr4 = ds_allocator_alloc(&allocator, 32);
    void *ptr5 = ds_allocator_alloc(&allocator, 8);

    printf("\nExpecting 5 blocks, middle one is split (alloc 32, alloc 8)\n");
    ds_allocator_dump(&allocator);

    ds_allocator_free(&allocator, ptr4);
    ds_allocator_free(&allocator, ptr5);

    printf(
        "\nExpecting 3 blocks, middle 2 ones are coalesced (free 2 blocks)\n");
    ds_allocator_dump(&allocator);

    return 0;
}
