#include <stdio.h>
#include <stdlib.h>
#define DS_PQ_IMPLEMENTATION
#include "../ds.h"

struct simple_node {
    int priority;
    const char *value;
};

struct simple_node *simple_node_new(int priority, const char *value) {
    struct simple_node *node = malloc(sizeof(struct simple_node));
    if (node == NULL) {
        LOG_ERROR("Failed to allocate memory");
        return NULL;
    }

    node->priority = priority;
    node->value = value;
    return node;
}

int simple_node_compare_max(const void *a, const void *b) {
    return ((struct simple_node *)a)->priority - ((struct simple_node *)b)->priority;
}

int main() {
    struct ds_priority_queue pq;
    ds_priority_queue_init(&pq, simple_node_compare_max);

    struct simple_node *node[] = {
        simple_node_new(3, "apple"),
        simple_node_new(2, "banana"),
        simple_node_new(5, "cherry"),
        simple_node_new(1, "date"),
        simple_node_new(7, "elderberry"),
    };

    for (size_t i = 0; i < sizeof(node) / sizeof(node[0]); i++) {
        ds_priority_queue_insert(&pq, node[i]);
    }

    struct simple_node *item;
    while (!ds_priority_queue_empty(&pq)) {
        ds_priority_queue_pull(&pq, (void **)&item);
        printf("Pulled: %d: %s\n", item->priority, item->value);
        free(item);
    }

    ds_priority_queue_free(&pq);
    return 0;
}
