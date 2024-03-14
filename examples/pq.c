#include <stdio.h>
#include <stdlib.h>
#define DS_PQ_IMPLEMENTATION
#include "../ds.h"

struct simple_node {
        int priority;
        const char *value;
};

int simple_node_compare_max(const void *a, const void *b) {
    return ((struct simple_node *)a)->priority -
           ((struct simple_node *)b)->priority;
}

int main() {
    ds_priority_queue pq;
    ds_priority_queue_init(&pq, simple_node_compare_max,
                           sizeof(struct simple_node));

    struct simple_node node[] = {
        {3, "apple"},
        {2, "banana"},
        {5, "cherry"},
        {1, "date"},
        {7, "elderberry"},
    };

    for (size_t i = 0; i < sizeof(node) / sizeof(node[0]); i++) {
        ds_priority_queue_insert(&pq, &node[i]);
    }

    while (!ds_priority_queue_empty(&pq)) {
        struct simple_node item = {0};
        ds_priority_queue_pull(&pq, (void *)&item);
        printf("Pulled: %d: %s\n", item.priority, item.value);
    }

    ds_priority_queue_free(&pq);
    return 0;
}
