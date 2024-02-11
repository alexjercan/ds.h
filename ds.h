#ifndef DS_H
#define DS_H

// DOCUMENTATION
//
// This is a single-header library that provides a set of data structures and
// utilities for C programs. The library is designed to be simple and easy to
// use, and it is intended to be used in small to medium-sized projects.
//
// Options:
// - DS_IMPLEMENTATION: Define this macro in one source file to include the
// implementation of all the data structures and utilities
// - DS_PQ_IMPLEMENTATION: Define this macro in one source file to include the
//  implementation of the priority queue data structure

// MEMORY MANAGEMENT
//
// The memory management macros are used to allocate, reallocate and free
// memory, and to exit the program. The DS_MALLOC macro is used to allocate
// memory, the DS_REALLOC macro is used to reallocate memory, the DS_FREE macro
// is used to free memory, and the DS_EXIT macro is used to exit the program.
//
// Options:
// - DS_NO_STDLIB: Disables the use of the standard library

#ifndef DS_NO_STDLIB
#include <stdlib.h>

#define DS_MALLOC(sz) malloc(sz)
#define DS_REALLOC(ptr, old_sz, new_sz) realloc(ptr, new_sz)
#define DS_FREE(ptr) free(ptr)
#define DS_EXIT(code) exit(code)
#else
#if defined(DS_MALLOC) && defined(DS_REALLOC) && defined(DS_FREE) &&           \
    defined(DS_EXIT)
// ok
#else
#error                                                                         \
    "Must define DS_MALLOC, DS_REALLOC, DS_FREE and DS_EXIT when DS_NO_STDLIB is defined"
#endif

#define NULL 0

#endif

// LOGGING
//
// The logging macros are used to print messages to the standard output and
// standard error streams. The DS_LOG_ERROR macro is used to print error
// messages, and the DS_LOG_INFO macro is used to print informational messages.
// The DS_PANIC macro is used to print an error message and exit the program.
//
// Options:
// - DS_NO_STDIO: Disables the use of the standard input/output streams
// - DS_QUIET: Disables the use of the standard error stream
// - DS_NO_LOG_ERROR: Disables the use of the DS_LOG_ERROR macro
// - DS_NO_LOG_INFO: Disables the use of the DS_LOG_INFO macro
// - DS_NO_TERMINAL_COLORS: Disables the use of terminal colors in the output

#ifndef DS_NO_STDIO
#include <stdio.h>
#endif

#ifdef DS_QUIET
#define DS_NO_LOG_ERROR
#define DS_NO_LOG_INFO
#endif

#ifdef DS_NO_TERMINAL_COLORS
#define DS_TERMINAL_RED ""
#define DS_TERMINAL_BLUE ""
#define DS_TERMINAL_RESET ""
#else
#define DS_TERMINAL_RED "\033[1;31m"
#define DS_TERMINAL_BLUE "\033[1;34m"
#define DS_TERMINAL_RESET "\033[0m"
#endif

#if defined(DS_NO_STDIO) || defined(DS_NO_LOG_ERROR)
#define DS_LOG_ERROR(format, ...)
#else
#define DS_LOG_ERROR(format, ...)                                              \
    fprintf(stderr,                                                            \
            DS_TERMINAL_RED "ERROR" DS_TERMINAL_RESET ": %s:%d: " format "\n", \
            __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#if defined(DS_NO_STDIO) || defined(DS_NO_LOG_INFO)
#define DS_LOG_INFO(format, ...)
#else
#define DS_LOG_INFO(format, ...)                                               \
    fprintf(stdout,                                                            \
            DS_TERMINAL_BLUE "INFO" DS_TERMINAL_RESET ": %s:%d: " format "\n", \
            __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define DS_PANIC(format, ...)                                                  \
    do {                                                                       \
        DS_LOG_ERROR(format, ##__VA_ARGS__);                                   \
        DS_EXIT(1);                                                            \
    } while (0)

// RETURN DEFER
//
// The return_defer macro is a simple way to return a value and jump to a label
// to execute cleanup code. It is similar to the defer statement in Go.

#define return_defer(code)                                                     \
    do {                                                                       \
        result = code;                                                         \
        goto defer;                                                            \
    } while (0)

// DYNAMIC ARRAY
//
// The dynamic array is a simple array that grows as needed. To use the dynamic
// array append macro, you need to define a struct with the following fields:
//  - items: a pointer to the array of items
//  - count: the number of items in the array
//  - capacity: the number of items that can be stored in the array

#define DA_INIT_CAPACITY 8192
#define ds_da_append(da, item)                                                 \
    do {                                                                       \
        if ((da)->count >= (da)->capacity) {                                   \
            unsigned int new_capacity = (da)->capacity * 2;                    \
            if (new_capacity == 0) {                                           \
                new_capacity = DA_INIT_CAPACITY;                               \
            }                                                                  \
                                                                               \
            (da)->items =                                                      \
                DS_REALLOC((da)->items, (da)->capacity * sizeof(*(da)->items), \
                           new_capacity * sizeof(*(da)->items));               \
            if ((da)->items == NULL) {                                         \
                DS_PANIC("Failed to reallocate dynamic array");                \
            }                                                                  \
                                                                               \
            (da)->capacity = new_capacity;                                     \
        }                                                                      \
                                                                               \
        (da)->items[(da)->count++] = (item);                                   \
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
        unsigned int count;
        unsigned int capacity;

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
    ds_da_append(pq, item);

    int index = pq->count - 1;
    int parent = (index - 1) / 2;

    while (index != 0 && pq->compare(pq->items[index], pq->items[parent]) > 0) {
        void *temp = pq->items[index];
        pq->items[index] = pq->items[parent];
        pq->items[parent] = temp;

        index = parent;
        parent = (index - 1) / 2;
    }

    return 0;
}

int ds_priority_queue_pull(struct ds_priority_queue *pq, void **item) {
    int result = 0;

    if (pq->count == 0) {
        DS_LOG_ERROR("Priority queue is empty");
        *item = NULL;
        return_defer(1);
    }

    *item = pq->items[0];
    pq->items[0] = pq->items[pq->count - 1];

    unsigned int index = 0;
    unsigned int swap = index;
    do {
        index = swap;

        unsigned int left = 2 * index + 1;
        if (left < pq->count &&
            pq->compare(pq->items[left], pq->items[swap]) > 0) {
            swap = left;
        }

        unsigned int right = 2 * index + 2;
        if (right < pq->count &&
            pq->compare(pq->items[right], pq->items[swap]) > 0) {
            swap = right;
        }

        void *temp = pq->items[index];
        pq->items[index] = pq->items[swap];
        pq->items[swap] = temp;
    } while (swap != index);

    pq->count--;
defer:
    return result;
}

int ds_priority_queue_peek(struct ds_priority_queue *pq, void **item) {
    int result = 0;

    if (pq->count == 0) {
        DS_LOG_ERROR("Priority queue is empty");
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
    DS_FREE(pq->items);
    pq->items = NULL;
    pq->count = 0;
    pq->capacity = 0;
    pq->compare = NULL;
}

#endif // DS_PQ_IMPLEMENTATION
