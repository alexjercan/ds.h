#define DS_ARENA_ALLOCATOR_IMPLEMENTATION
#include "../ds.h"

#define MEMORY_CAPACITY 1024
char memory[MEMORY_CAPACITY] = {0};
DS_ALLOCATOR allocator = {0};

static void dump_memory() {
    for (unsigned int i = 0; i < 150; i=i+8) {
        unsigned long x = *(unsigned long *)(memory + i);
        printf("%lx ", x);
        printf("\n");
    }
}

typedef struct node_t {
    struct node_t *next;
    unsigned long size;
} node_t;

static void *create(node_t *start, unsigned long size) {
    node_t *iter = start;

    while (iter->next != NULL) {
        iter = iter->next;
    }

    void *ptr = DS_MALLOC(&allocator, sizeof(node_t) + size);

    // // before
    // node_t node = {0};
    // DS_MEMCPY(&node, ptr, sizeof(node_t));
    // node.next = NULL;
    // node.size = size;
    // DS_MEMCPY(ptr, &node, sizeof(node_t));

    // after
    node_t *node_ref = (node_t *)ptr;
    node_ref->next = NULL;
    node_ref->size = size;

    iter->next = node_ref;

    return (char *)ptr + sizeof(node_t);
}

static void delete(node_t *start, void *data) {
    node_t *node_ref = (node_t *)((char *)data - sizeof(node_t));
    node_t *iter = start;

    while (iter->next != node_ref) {
        iter = iter->next;
    }

    iter->next = iter->next->next;

    DS_FREE(&allocator, node_ref);
}

int main() {
    DS_INIT_ALLOCATOR(&allocator, memory, MEMORY_CAPACITY);
    DS_DUMP_ALLOCATOR(allocator);

    node_t start = {0};
    void *data = create(&start, 8);
    *(long *)data = 69;

    void *data2 = create(&start, 8);
    *(long *)data2 = 420;

    delete(&start, data);

    DS_DUMP_ALLOCATOR(allocator);
    dump_memory();

    delete(&start, data2);

    return 0;
}
