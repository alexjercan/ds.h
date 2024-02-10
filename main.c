#include <stdio.h>
#include <stdlib.h>

#define return_defer(code)                                                     \
    do {                                                                       \
        result = code;                                                         \
        goto defer;                                                            \
    } while (0)

#define LOG_ERROR(format, ...)                                                 \
    fprintf(stderr, "ERROR: %s:%d: " format "\n", __FILE__, __LINE__,          \
            ##__VA_ARGS__)

#define LOG_INFO(format, ...)                                                  \
    fprintf(stdout, "INFO: " format "\n", ##__VA_ARGS__)

#define DA_INIT_CAPACITY 8192
#define da_append(da, item)                                                    \
    do {                                                                       \
        if ((da)->count >= (da)->capacity) {                                   \
            size_t new_capacity = (da)->capacity * 2;                          \
            if (new_capacity == 0) {                                           \
                new_capacity = DA_INIT_CAPACITY;                               \
            }                                                                  \
                                                                               \
            (da)->items =                                                      \
                realloc((da)->items, new_capacity * sizeof((da)->items[0]));   \
            if ((da)->items == NULL) {                                         \
                LOG_ERROR("Failed to allocate memory");                        \
                return_defer(1);                                               \
            }                                                                  \
                                                                               \
            (da)->capacity = new_capacity;                                     \
        }                                                                      \
                                                                               \
        (da)->items[(da)->count++] = (item);                                   \
    } while (0)

#define da_remove(da, index)                                                   \
    do {                                                                       \
        if ((index) < 0 || (index) >= (da)->count) {                           \
            LOG_ERROR("Index out of bounds: %d", index);                       \
            return_defer(1);                                                   \
        }                                                                      \
                                                                               \
        for (size_t i = (index); i < (da)->count - 1; i++) {                   \
            (da)->items[i] = (da)->items[i + 1];                               \
        }                                                                      \
        (da)->count--;                                                         \
    } while (0)

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

int simple_node_compare_min(const void *a, const void *b) {
    return ((struct simple_node *)b)->priority - ((struct simple_node *)a)->priority;
}

struct priority_queue {
        void **items;
        size_t count;
        size_t capacity;

        int (*compare)(const void *, const void *);
};

void priority_queue_init(struct priority_queue *pq, int (*compare)(const void *, const void *)) {
    pq->items = NULL;
    pq->count = 0;
    pq->capacity = 0;
    pq->compare = compare;
}

int priority_queue_insert(struct priority_queue *pq, void *item) {
    int result = 0;

    da_append(pq, item);

    int index = pq->count - 1;
    int parent = (index - 1) / 2;

    while (index != 0 && pq->compare(pq->items[index], pq->items[parent]) > 0) {
        void *temp = pq->items[index];
        pq->items[index] = pq->items[parent];
        pq->items[parent] = temp;

        index = parent;
        parent = (index - 1) / 2;
    }

defer:
    return result;
}

int priority_queue_pull(struct priority_queue *pq, void **item) {
    int result = 0;

    if (pq->count == 0) {
        LOG_ERROR("Priority queue is empty");
        return_defer(1);
    }

    *item = pq->items[0];
    pq->items[0] = pq->items[pq->count - 1];

    int index = 0;
    int swap = index;
    do {
        int left = 2 * index + 1;
        if (left < pq->count && pq->compare(pq->items[left], pq->items[swap]) > 0) {
            swap = left;
        }

        int right = 2 * index + 2;
        if (right < pq->count && pq->compare(pq->items[right], pq->items[swap]) > 0) {
            swap = right;
        }

        if (swap != index) {
            void *temp = pq->items[index];
            pq->items[index] = pq->items[swap];
            pq->items[swap] = temp;

            index = swap;
        }
    } while (swap != index);

    pq->count--;
defer:
    return result;
}

int priority_queue_peek(struct priority_queue *pq, void **item) {
    int result = 0;

    if (pq->count == 0) {
        LOG_ERROR("Priority queue is empty");
        return_defer(1);
    }

    *item = pq->items[0];

defer:
    return result;
}

int priority_queue_empty(struct priority_queue *pq) {
    return pq->count == 0;
}

void priority_queue_free(struct priority_queue *pq) {
    free(pq->items);
    pq->items = NULL;
    pq->count = 0;
    pq->capacity = 0;
    pq->compare = NULL;
}

int main() {
    struct priority_queue pq;
    priority_queue_init(&pq, simple_node_compare_max);
    priority_queue_init(&pq, simple_node_compare_min);

    struct simple_node *node[] = {
        simple_node_new(3, "apple"),
        simple_node_new(2, "banana"),
        simple_node_new(5, "cherry"),
        simple_node_new(1, "date"),
        simple_node_new(7, "elderberry"),
    };

    for (size_t i = 0; i < sizeof(node) / sizeof(node[0]); i++) {
        priority_queue_insert(&pq, node[i]);
    }

    struct simple_node *item;
    while (!priority_queue_empty(&pq)) {
        priority_queue_pull(&pq, (void **)&item);
        printf("Pulled: %d: %s\n", item->priority, item->value);
        free(item);
    }

    priority_queue_free(&pq);
    return 0;
}
