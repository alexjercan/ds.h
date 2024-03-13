#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void uint64_read_le(uint8_t *data, uint64_t *value) {
    *value = ((uint64_t)data[0] << 0) | ((uint64_t)data[1] << 8) |
             ((uint64_t)data[2] << 16) | ((uint64_t)data[3] << 24) |
             ((uint64_t)data[4] << 32) | ((uint64_t)data[5] << 40) |
             ((uint64_t)data[6] << 48) | ((uint64_t)data[7] << 56);
}

static void uint64_write_le(uint8_t *data, uint64_t value) {
    data[0] = (value >> 0) & 0xff;
    data[1] = (value >> 8) & 0xff;
    data[2] = (value >> 16) & 0xff;
    data[3] = (value >> 24) & 0xff;
    data[4] = (value >> 32) & 0xff;
    data[5] = (value >> 40) & 0xff;
    data[6] = (value >> 48) & 0xff;
    data[7] = (value >> 56) & 0xff;
}

static void uint32_read_le(uint8_t *data, uint32_t *value) {
    *value = ((uint32_t)data[0] << 0) | ((uint32_t)data[1] << 8) |
             ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

static void uint32_write_le(uint8_t *data, uint32_t value) {
    data[0] = (value >> 0) & 0xff;
    data[1] = (value >> 8) & 0xff;
    data[2] = (value >> 16) & 0xff;
    data[3] = (value >> 24) & 0xff;
}

#define BLOCK_METADATA_SIZE 28
#define BLOCK_INDEX_UNDEFINED -1

/*
 * | prev | next | size | free | ... size bytes of data ... |
 */
typedef struct block {
        int64_t prev;  // 8 bytes
        int64_t next;  // 8 bytes
        uint64_t size; // 8 bytes
        uint32_t free; // 4 bytes
        uint8_t *data; // 8 bytes
} block_t;

static void block_read(uint8_t *data, block_t *block) {
    uint64_read_le(data + 0, (uint64_t *)&block->prev);
    uint64_read_le(data + 8, (uint64_t *)&block->next);
    uint64_read_le(data + 16, &block->size);
    uint32_read_le(data + 24, &block->free);
    block->data = data + BLOCK_METADATA_SIZE;
}

static void block_write(uint8_t *data, block_t *block) {
    uint64_write_le(data + 0, block->prev);
    uint64_write_le(data + 8, block->next);
    uint64_write_le(data + 16, block->size);
    uint32_write_le(data + 24, block->free);
}

typedef struct allocator {
        uint8_t *start;
        uint8_t *prev;
        uint8_t *top;
        uint64_t size;
} allocator_t;

void allocator_init(allocator_t *allocator, uint8_t *start, uint64_t size) {
    allocator->start = start;
    allocator->prev = NULL;
    allocator->top = start;
    allocator->size = size;
}

void allocator_dump(allocator_t *allocator) {
    block_t block = {0};
    uint8_t *ptr = allocator->start;

    printf("%*s %*s %*s %*s %*s\n", 14, "", 14, "prev", 14, "next", 14, "size",
           14, "free");

    while (ptr < allocator->top) {
        block_read(ptr, &block);

        uint8_t *prev = (block.prev == BLOCK_INDEX_UNDEFINED)
                            ? NULL
                            : allocator->start + block.prev;
        uint8_t *next = (block.next == BLOCK_INDEX_UNDEFINED)
                            ? NULL
                            : allocator->start + block.next;

        printf("%*p %*p %*p %*lu %*u\n", 14, ptr, 14, prev, 14, next, 14,
               block.size, 14, block.free);

        ptr += (block.size + BLOCK_METADATA_SIZE);
    }
}

static int allocator_find_block(allocator_t *allocator, uint64_t size,
                                block_t *block) {
    (void)allocator;
    (void)size;
    (void)block;
    return 0;
}

void *allocator_alloc(allocator_t *allocator, uint64_t size) {
    block_t block = {0};
    if (allocator_find_block(allocator, size, &block) != 0) {
        block.free = 0;
        return block.data;
    }

    if (allocator->top + size + BLOCK_METADATA_SIZE >
        allocator->start + allocator->size) {
        return NULL;
    }

    block.next = BLOCK_INDEX_UNDEFINED;
    block.size = size;
    block.free = 0;
    block.data = allocator->top + BLOCK_METADATA_SIZE;

    if (allocator->prev == NULL) {
        block.prev = BLOCK_INDEX_UNDEFINED;
    } else {
        block.prev = (uint64_t)(allocator->prev - allocator->start);

        block_t prev = {0};
        block_read(allocator->prev, &prev);
        prev.next = (uint64_t)(allocator->top - allocator->start);

        block_write(allocator->prev, &prev);
    }

    block_write(allocator->top, &block);

    allocator->prev = allocator->top;
    allocator->top += size + BLOCK_METADATA_SIZE;

    return block.data;
}

void allocator_free(allocator_t *allocator, void *ptr) {
    if ((uint8_t *)ptr > allocator->top || (uint8_t *)ptr < allocator->start) {
        return;
    }

    block_t block = {0};
    block_read(ptr - BLOCK_METADATA_SIZE, &block);
    block.free = 1;
    block_write(ptr - BLOCK_METADATA_SIZE, &block);
}

#define ALLOC_SIZE 1024

int main() {
    uint8_t data[ALLOC_SIZE];
    allocator_t allocator;
    allocator_init(&allocator, data, ALLOC_SIZE);

    void *ptr = allocator_alloc(&allocator, 128);
    ptr = allocator_alloc(&allocator, 128);
    ptr = allocator_alloc(&allocator, 128);

    allocator_dump(&allocator);

    return 0;
}
