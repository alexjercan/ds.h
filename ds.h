#ifndef DS_H
#define DS_H

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
    fprintf(stdout, "INFO: %s:%d: " format "\n", __FILE__, __LINE__,           \
            ##__VA_ARGS__)

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

// PRIORITY QUEUE
//
// The priority queue is implemented as a heap, where you can define the
// comparison function to use when inserting items. The comparison function
// should return a positive value if the first item has higher priority than
// the second item, a negative value if the second item has higher priority than
// the first item, and 0 if the items have the same priority.

struct ds_priority_queue;

void ds_priority_queue_init(struct ds_priority_queue *pq,
                            int (*compare)(const void *, const void *));
int ds_priority_queue_insert(struct ds_priority_queue *pq, void *item);
int ds_priority_queue_pull(struct ds_priority_queue *pq, void **item);
int ds_priority_queue_peek(struct ds_priority_queue *pq, void **item);
int ds_priority_queue_empty(struct ds_priority_queue *pq);
void ds_priority_queue_free(struct ds_priority_queue *pq);

#endif // DS_H

#ifdef DS_IMPLEMENTATION
#define DS_PQ_IMPLEMENTATION
#endif // DS_IMPLEMENTATION

#ifdef DS_PQ_IMPLEMENTATION

struct ds_priority_queue {
        void **items;
        size_t count;
        size_t capacity;

        int (*compare)(const void *, const void *);
};

void ds_priority_queue_init(struct ds_priority_queue *pq,
                            int (*compare)(const void *, const void *)) {
    pq->items = NULL;
    pq->count = 0;
    pq->capacity = 0;
    pq->compare = compare;
}

int ds_priority_queue_insert(struct ds_priority_queue *pq, void *item) {
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

int ds_priority_queue_pull(struct ds_priority_queue *pq, void **item) {
    int result = 0;

    if (pq->count == 0) {
        LOG_ERROR("Priority queue is empty");
        *item = NULL;
        return_defer(1);
    }

    *item = pq->items[0];
    pq->items[0] = pq->items[pq->count - 1];

    size_t index = 0;
    size_t swap = index;
    do {
        size_t left = 2 * index + 1;
        if (left < pq->count &&
            pq->compare(pq->items[left], pq->items[swap]) > 0) {
            swap = left;
        }

        size_t right = 2 * index + 2;
        if (right < pq->count &&
            pq->compare(pq->items[right], pq->items[swap]) > 0) {
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

int ds_priority_queue_peek(struct ds_priority_queue *pq, void **item) {
    int result = 0;

    if (pq->count == 0) {
        LOG_ERROR("Priority queue is empty");
        *item = NULL;
        return_defer(1);
    }

    *item = pq->items[0];

defer:
    return result;
}

int ds_priority_queue_empty(struct ds_priority_queue *pq) {
    return pq->count == 0;
}

void ds_priority_queue_free(struct ds_priority_queue *pq) {
    free(pq->items);
    pq->items = NULL;
    pq->count = 0;
    pq->capacity = 0;
    pq->compare = NULL;
}

#endif // DS_PQ_IMPLEMENTATION
