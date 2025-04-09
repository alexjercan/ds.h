// # DOCUMENTATION
//
// This is a simple C library that provides some helper functions and macros
// for memory management, logging, and error handling.
//
// Options:
// - DS_NO_STDLIB: Disables the use of the standard library
//
// ## MEMORY MANAGEMENT
//
// The memory management functions are used to allocate and free memory
// using the selected allocator implementation.
//
// Options:
// - DS_ARENA_ALLOCATOR_IMPLEMENTATION: Use the arena allocator
// - DS_LIST_ALLOCATOR_IMPLEMENTATION: Use the list allocator
// - DS_PQ_IMPLEMENTATION: Use the priority queue implementation
// - DS_LL_IMPLEMENTATION: Use the linked list implementation
//
// ## DATA STRUCTURES
//
// This is a single-header library that provides a set of data structures and
// utilities for C programs. The library is designed to be simple and easy to
// use, and it is intended to be used in small to medium-sized projects.
//
// Options:
// - DS_DA_IMPLEMENTATION: Define this macro in one source file to include the
// implementation of the dynamic array data structure
// - DS_SB_IMPLEMENTATION: Define this macro in one source file to include the
// implementation of the string builder and string slice utilities
//
// ## LOGGING
//
// The logging macros are used to print messages to the standard output and
// standard error streams. The DS_LOG_ERROR macro is used to print error
// messages, and the DS_LOG_INFO macro is used to print informational messages.
// The DS_PANIC macro is used to print an error message and exit the program.
//
// Options:
// - DS_NO_STDIO: Disables the use of the standard input/output streams
// - DS_LOG_LEVEL: Sets the logging level to one of the following values:
//  - DS_LOG_LEVEL_DEBUG: Print debug, info, warn and error messages
//  - DS_LOG_LEVEL_INFO: Print info, warn and error messages
//  - DS_LOG_LEVEL_WARN: Print warn and error messages
//  - DS_LOG_LEVEL_ERROR: Print error messages
//  - DS_LOG_LEVEL_NONE: Print no messages
// The default logging level is DS_LOG_LEVEL_DEBUG
// - DS_NO_TERMINAL_COLORS: Disables the use of terminal colors in the log
// messages
//
// ## Tools and Utilities
//
// The tools and utilities are used to provide some helper functions and
// macros for things I do often in my C projects.
//
// Options:
// - DS_IO_IMPLEMENTATION: Define this macro for some io utils

#ifndef DS_H
#define DS_H

#ifndef DS_NO_STDLIB
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#endif

#ifdef DS_NO_STDLIB
#define DS_NO_STDIO
#endif

#ifndef DS_NO_STDIO
#include <stdio.h>
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef DSHDEF
#ifdef DSH_STATIC
#define DSHDEF static
#else
#define DSHDEF extern
#endif
#endif

// Return codes
typedef int DS_RESULT;
#define DS_OK 0
#define DS_ERR 1

// Boolean type
typedef int boolean;
#define true 1
#define false 0

// RETURN DEFER
//
// The return_defer macro is a simple way to return a value and jump to a label
// to execute cleanup code. It is similar to the defer statement in Go.

#ifndef return_defer
#define return_defer(code)                                                     \
    do {                                                                       \
        result = code;                                                         \
        goto defer;                                                            \
    } while (0)
#endif // return_defer

// DS_EXIT
//
// The DS_EXIT macro is used to exit the program with a specified exit code.
#if defined(DS_EXIT) // ok
#elif !defined(DS_NO_STDLIB)
#define DS_EXIT(code) exit(code)
#else
#define DS_EXIT(code)
#warning "DS_EXIT is not defined, using empty definition"
#endif

// ARENA ALLOCATOR
//
// The arena allocator is a simple memory allocator that uses a single
// block of memory to allocate smaller blocks of memory. It is designed
// for performance and simplicity, and it is suitable for use in
// embedded systems.
typedef struct ds_arena_allocator {
    char *memory;
    unsigned long offset;
    unsigned long size;
} ds_arena_allocator;

DSHDEF void ds_arena_allocator_init(ds_arena_allocator *allocator, void *memory, unsigned long size);
DSHDEF void *ds_arena_allocator_alloc(ds_arena_allocator *allocator, unsigned long size);
DSHDEF void ds_arena_allocator_clear(ds_arena_allocator *allocator);
DSHDEF void ds_arena_allocator_dump(ds_arena_allocator allocator);

// LIST ALLOCATOR
//
// The list allocator is a simple memory allocator that uses a linked list
// to manage memory blocks. It is designed for flexibility and
// dynamic memory allocation, and it is suitable for use in
// applications where memory usage is unpredictable.
typedef struct ds_list_allocator {
    struct ds_list_allocator_node *memory;
} ds_list_allocator;

DSHDEF void ds_list_allocator_init(ds_list_allocator *allocator, void *memory, unsigned long size);
DSHDEF void *ds_list_allocator_alloc(ds_list_allocator *allocator, unsigned long size);
DSHDEF void ds_list_allocator_free(ds_list_allocator *allocator, void *ptr);
DSHDEF void ds_list_allocator_clear(ds_list_allocator *allocator);
DSHDEF void ds_list_allocator_dump(ds_list_allocator allocator);

// DS_ALLOCATOR
//
// The DS_ALLOCATOR macro is used to select the appropriate allocator
// implementation based on the compilation flags. It allows the user
// to choose between the arena allocator and the list allocator
// based on their needs.
#if defined(DS_ALLOCATOR) // ok
#elif defined(DS_ARENA_ALLOCATOR_IMPLEMENTATION)
#define DS_ALLOCATOR ds_arena_allocator
#elif defined(DS_LIST_ALLOCATOR_IMPLEMENTATION)
#define DS_ALLOCATOR ds_list_allocator
#elif !defined(DS_NO_STDLIB)
#define DS_ALLOCATOR void *
#else
#define DS_ALLOCATOR void *
#error "DS_NO_STDLIB requires either DS_ARENA_ALLOCATOR_IMPLEMENTATION or DS_LIST_ALLOCATOR_IMPLEMENTATION"
#endif

// DS_INIT_ALLOCATOR
//
// The DS_INIT_ALLOCATOR macro is used to initialize the allocator
// based on the selected implementation. It allows the user
// to initialize the allocator with a block of memory and a size.
#if defined(DS_INIT_ALLOCATOR) // ok
#elif defined(DS_ARENA_ALLOCATOR_IMPLEMENTATION)
#define DS_INIT_ALLOCATOR(allocator, memory, size) ds_arena_allocator_init(allocator, memory, size)
#elif defined(DS_LIST_ALLOCATOR_IMPLEMENTATION)
#define DS_INIT_ALLOCATOR(allocator, memory, size) ds_list_allocator_init(allocator, memory, size)
#elif !defined(DS_NO_STDLIB)
#define DS_INIT_ALLOCATOR(allocator, memory, size)
#else
#define DS_INIT_ALLOCATOR(allocator, memory, size)
#endif

// DS_MALLOC
//
// The DS_MALLOC macro is used to allocate memory using the
// selected allocator implementation. It allows the user
// to allocate memory of a specified size
// and returns a pointer to the allocated memory.
#if defined(DS_MALLOC) // ok
#elif defined(DS_ARENA_ALLOCATOR_IMPLEMENTATION)
#define DS_MALLOC(allocator, size) ds_arena_allocator_alloc(allocator, size)
#elif defined(DS_LIST_ALLOCATOR_IMPLEMENTATION)
#define DS_MALLOC(allocator, size) ds_list_allocator_alloc(allocator, size)
#elif !defined(DS_NO_STDLIB)
#define DS_MALLOC(allocator, size) malloc(size)
#else
#define DS_MALLOC(allocator, size) NULL
#endif

// DS_FREE
//
// The DS_FREE macro is used to free memory using the
// selected allocator implementation. It allows the user
// to free memory that was previously allocated
// and returns a pointer to the freed memory.
#if defined(DS_FREE) // ok
#elif defined(DS_ARENA_ALLOCATOR_IMPLEMENTATION)
#define DS_FREE(allocator, ptr)
#elif defined(DS_LIST_ALLOCATOR_IMPLEMENTATION)
#define DS_FREE(allocator, ptr) ds_list_allocator_free(allocator, ptr)
#elif !defined(DS_NO_STDLIB)
#define DS_FREE(allocator, ptr) free(ptr)
#else
#define DS_FREE(allocator, ptr)
#endif

// DS_CLEAR
//
// The DS_CLEAR macro is used to clear the allocator
// based on the selected implementation.
#if defined(DS_CLEAR) // ok
#elif defined(DS_ARENA_ALLOCATOR_IMPLEMENTATION)
#define DS_CLEAR(allocator) ds_arena_allocator_clear(allocator)
#elif defined(DS_LIST_ALLOCATOR_IMPLEMENTATION)
#define DS_CLEAR(allocator) ds_list_allocator_clear(allocator)
#elif !defined(DS_NO_STDLIB)
#define DS_CLEAR(allocator)
#else
#define DS_CLEAR(allocator)
#endif

// DS_DUMP
//
// The DS_DUMP macro is used to dump the allocator
// based on the selected implementation.
#if defined(DS_DUMP) // ok
#elif defined(DS_ARENA_ALLOCATOR_IMPLEMENTATION)
#define DS_DUMP_ALLOCATOR(allocator) ds_arena_allocator_dump(allocator)
#elif defined(DS_LIST_ALLOCATOR_IMPLEMENTATION)
#define DS_DUMP_ALLOCATOR(allocator) ds_list_allocator_dump(allocator)
#elif !defined(DS_NO_STDLIB)
#define DS_DUMP_ALLOCATOR(allocator)
#else
#define DS_DUMP_ALLOCATOR(allocator)
#endif

// DS_MEMCPY
//
// The DS_MEMCPY macro is used to copy memory
#if defined(DS_MEMCPY) // ok
#elif !defined(DS_NO_STDLIB)
#define DS_MEMCPY(dst, src, size) memcpy(dst, src, size)
#else
#define DS_MEMCPY(dst, src, sz)                                                \
    do {                                                                       \
        for (unsigned long i = 0; i < sz; i++) {                               \
            ((char *)dst)[i] = ((char *)src)[i];                               \
        }                                                                      \
    } while (0)
#endif // DS_MEMCPY

#if defined(DS_MEMCMP) // ok
#elif !defined(DS_NO_STDLIB)
#define DS_MEMCMP(ptr1, ptr2, sz) memcmp(ptr1, ptr2, sz)
#elif defined(DS_NO_STDLIB)
#define DS_MEMCMP(ptr1, ptr2, sz)                                              \
    ({                                                                         \
        int result = 0;                                                        \
        for (unsigned int i = 0; i < sz; i++) {                                \
            if (((char *)ptr1)[i] != ((char *)ptr2)[i]) {                      \
                result = ((char *)ptr1)[i] - ((char *)ptr2)[i];                \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        result;                                                                \
    })
#endif

#if defined(DS_STRLEN) // ok
#elif !defined(DS_NO_STDLIB)
#define DS_STRLEN(str) strlen(str)
#elif defined(DS_NO_STDLIB)
#define DS_STRLEN(str)                                                         \
    ({                                                                         \
        unsigned long len = 0;                                                 \
        while (((char *)str)[len] != '\0') {                                   \
            len++;                                                             \
        }                                                                      \
        len;                                                                   \
    })
#endif

static inline void *allocator_realloc(void *allocator, void *ptr, unsigned long old_sz, unsigned long new_sz) {
    (void)(allocator); // Suppress unused parameter warning
    void *new_ptr = DS_MALLOC(allocator, new_sz);
    if (new_ptr == NULL) {
        DS_FREE(allocator, ptr);
        return NULL;
    }
    if (ptr != NULL) {
        DS_MEMCPY(new_ptr, ptr, ((old_sz < new_sz) ? old_sz : new_sz));
        DS_FREE(allocator, ptr);
    }
    return new_ptr;
}

// DS_REALLOC
//
// The DS_REALLOC macro is used to reallocate memory
// using the selected allocator implementation.
#if defined(DS_ARENA_ALLOCATOR_IMPLEMENTATION)
#define DS_REALLOC(allocator, ptr, old_sz, new_sz) allocator_realloc(allocator, ptr, old_sz, new_sz)
#elif defined(DS_LIST_ALLOCATOR_IMPLEMENTATION)
#define DS_REALLOC(allocator, ptr, old_sz, new_sz) allocator_realloc(allocator, ptr, old_sz, new_sz)
#elif !defined(DS_NO_STDLIB)
#define DS_REALLOC(allocator, ptr, old_sz, new_sz) realloc(ptr, new_sz)
#else
#define DS_REALLOC(allocator, ptr, old_sz, new_sz) NULL
#endif

// LOGGING

#if defined(DS_NO_STDIO) && !defined(fprintf)
#define fprintf(stream, format, ...) 0
#endif

#if defined(DS_NO_STDIO) && !defined(stdin)
#define stdin NULL
#endif

#if defined(DS_NO_STDIO) && !defined(stderr)
#define stderr NULL
#endif

#if defined(DS_NO_STDIO) && !defined(stdout)
#define stdout NULL
#endif

#define DS_LOG_LEVEL_DEBUG 1
#define DS_LOG_LEVEL_INFO 10
#define DS_LOG_LEVEL_WARN 100
#define DS_LOG_LEVEL_ERROR 1000
#define DS_LOG_LEVEL_NONE 10000

#ifndef DS_LOG_LEVEL
#define DS_LOG_LEVEL DS_LOG_LEVEL_DEBUG
#endif

#ifdef DS_NO_TERMINAL_COLORS
#define DS_TERMINAL_RED ""
#define DS_TERMINAL_YELLOW ""
#define DS_TERMINAL_BLUE ""
#define DS_TERMINAL_RESET ""
#else
#define DS_TERMINAL_RED "\033[1;31m"
#define DS_TERMINAL_YELLOW "\033[1;33m"
#define DS_TERMINAL_BLUE "\033[1;34m"
#define DS_TERMINAL_RESET "\033[0m"
#endif

// DS_LOG_ERROR
//
// The DS_LOG_ERROR macro is used to log error messages
#if DS_LOG_LEVEL > DS_LOG_LEVEL_ERROR
#define DS_LOG_ERROR(format, ...)
#else
#define DS_LOG_ERROR(format, ...)                                              \
    fprintf(stderr,                                                            \
            DS_TERMINAL_RED "ERROR" DS_TERMINAL_RESET ": %s:%d: " format "\n", \
            __FILE__, __LINE__, ##__VA_ARGS__)
#endif

// DS_LOG_WARN
//
// The DS_LOG_WARN macro is used to log warning messages
#if DS_LOG_LEVEL > DS_LOG_LEVEL_WARN
#define DS_LOG_WARN(format, ...)
#else
#define DS_LOG_WARN(format, ...)                                               \
    fprintf(stdout,                                                            \
            DS_TERMINAL_YELLOW "WARN" DS_TERMINAL_RESET ": %s:%d: " format     \
                               "\n",                                           \
            __FILE__, __LINE__, ##__VA_ARGS__)
#endif

// DS_LOG_INFO
//
// The DS_LOG_INFO macro is used to log informational messages
#if DS_LOG_LEVEL > DS_LOG_LEVEL_INFO
#define DS_LOG_INFO(format, ...)
#else
#define DS_LOG_INFO(format, ...)                                               \
    fprintf(stdout,                                                            \
            DS_TERMINAL_BLUE "INFO" DS_TERMINAL_RESET ": %s:%d: " format "\n", \
            __FILE__, __LINE__, ##__VA_ARGS__)
#endif

// DS_LOG_DEBUG
//
// The DS_LOG_DEBUG macro is used to log debug messages
#if DS_LOG_LEVEL > DS_LOG_LEVEL_DEBUG
#define DS_LOG_DEBUG(format, ...)
#else
#define DS_LOG_DEBUG(format, ...)                                              \
    fprintf(stdout, "DEBUG: %s:%d: " format "\n", __FILE__, __LINE__,          \
            ##__VA_ARGS__)
#endif

// DS_PANIC
//
// The DS_PANIC macro is used to log a panic message and exit the program
#ifndef DS_PANIC
#define DS_PANIC(format, ...)                                                  \
    do {                                                                       \
        DS_LOG_ERROR(format, ##__VA_ARGS__);                                   \
        DS_EXIT(1);                                                            \
    } while (0)
#endif

#define DS_ERROR_OOM "Buy more RAM!"
#define DS_ERROR_UNREACHABLE "unreachable"

// DS_EXPECT
//
// The DS_EXPECT macro is used to check the result of a function call
// and log an error message if the result is not OK. It is similar
// to the assert statement in C, but it allows for more
// flexibility in handling errors.
#define DS_EXPECT(result, message)                                             \
  do {                                                                         \
    if ((result) != DS_OK) {                                                   \
      DS_PANIC(message);                                                       \
    }                                                                          \
  } while (0)

#define DS_UNREACHABLE(result) DS_EXPECT((result), DS_ERROR_UNREACHABLE)

// DYNAMIC ARRAY
//
// The dynamic array is a simple array that grows as needed. This is the real
// implementation of dynamic arrays. The macros from the header file are just
// quick inline versions of these functions. This implementation is generic and
// can be used with any type of item, unlike the macros which require you to
// define the array structure with the items, count and capacity fields.
typedef struct ds_dynamic_array {
        DS_ALLOCATOR *allocator;
        void *items;
        unsigned long item_size;
        unsigned long count;
        unsigned long capacity;
} ds_dynamic_array;

#ifndef DS_DA_INIT_CAPACITY
#define DS_DA_INIT_CAPACITY 8192
#endif

DSHDEF void ds_dynamic_array_init_allocator(ds_dynamic_array *da,
                                            unsigned long item_size,
                                            DS_ALLOCATOR *allocator);
DSHDEF void ds_dynamic_array_init(ds_dynamic_array *da,
                                  unsigned long item_size);
DSHDEF DS_RESULT ds_dynamic_array_append(ds_dynamic_array *da,
                                         const void *item);
DSHDEF DS_RESULT ds_dynamic_array_pop(ds_dynamic_array *da, const void **item);
DSHDEF DS_RESULT ds_dynamic_array_append_many(ds_dynamic_array *da,
                                              void **new_items,
                                              unsigned long new_items_count);
DSHDEF DS_RESULT ds_dynamic_array_get(ds_dynamic_array *da, unsigned long index,
                                      void *item);
DSHDEF DS_RESULT ds_dynamic_array_get_ref(ds_dynamic_array *da,
                                          unsigned long index, void **item);
DSHDEF DS_RESULT ds_dynamic_array_copy(ds_dynamic_array *da,
                                       ds_dynamic_array *copy);
DSHDEF void ds_dynamic_array_sort(ds_dynamic_array *da,
                                  int (*compare)(const void *, const void *));
DSHDEF DS_RESULT ds_dynamic_array_reverse(ds_dynamic_array *da);
DSHDEF DS_RESULT ds_dynamic_array_swap(ds_dynamic_array *da,
                                       unsigned long index1,
                                       unsigned long index2);
DSHDEF DS_RESULT ds_dynamic_array_delete(ds_dynamic_array *da,
                                         unsigned long index);
DSHDEF void ds_dynamic_array_free(ds_dynamic_array *da);

// STRING SLICE
//
// The string slice is a simple utility to work with substrings. You can use the
// string slice to tokenize a string, and to convert a string slice to an owned
// string.
typedef struct ds_string_slice {
        DS_ALLOCATOR *allocator;
        char *str;
        unsigned long len;
} ds_string_slice;

#define DS_STRING_SLICE(string) ((ds_string_slice){.str = string, .len = DS_STRLEN((string))})

DSHDEF void ds_string_slice_init_allocator(ds_string_slice *ss, char *str,
                                           unsigned long len,
                                           DS_ALLOCATOR *allocator);
DSHDEF void ds_string_slice_init(ds_string_slice *ss, char *str,
                                 unsigned long len);
DSHDEF boolean ds_string_slice_tokenize(ds_string_slice *ss, char delimiter,
                                        ds_string_slice *token);
DSHDEF boolean ds_string_slice_take_while_pred(ds_string_slice *ss,
                                               boolean (*predicate)(char),
                                               ds_string_slice *token);
DSHDEF void ds_string_slice_trim_left_ws(ds_string_slice *ss);
DSHDEF void ds_string_slice_trim_right_ws(ds_string_slice *ss);
DSHDEF void ds_string_slice_trim_left(ds_string_slice *ss, char chr);
DSHDEF void ds_string_slice_trim_right(ds_string_slice *ss, char chr);
DSHDEF void ds_string_slice_trim(ds_string_slice *ss, char chr);
DSHDEF DS_RESULT ds_string_slice_to_owned(ds_string_slice *ss, char **str);
DSHDEF boolean ds_string_slice_equals(ds_string_slice *ss, ds_string_slice *so);
DSHDEF boolean ds_string_slice_starts_with(ds_string_slice *ss,
                                           ds_string_slice *prefix);
DSHDEF boolean ds_string_slice_starts_with_pred(ds_string_slice *ss,
                                                boolean (*predicate)(char));
DSHDEF void ds_string_slice_step(ds_string_slice *ss, int count);
DSHDEF boolean ds_string_slice_empty(ds_string_slice *ss);
DSHDEF void ds_string_slice_free(ds_string_slice *ss);

// STRING BUILDER
//
// The string builder is a simple utility to build strings. You can append
// formatted strings to the string builder, and then build the final string.
// The string builder will automatically grow as needed.
typedef struct ds_string_builder {
        ds_dynamic_array items;
} ds_string_builder;

DSHDEF void ds_string_builder_init_allocator(ds_string_builder *sb,
                                             DS_ALLOCATOR *allocator);
DSHDEF void ds_string_builder_init(ds_string_builder *sb);
DSHDEF DS_RESULT ds_string_builder_append(ds_string_builder *sb,
                                          const char *format, ...);
DSHDEF DS_RESULT ds_string_builder_appendn(ds_string_builder *sb,
                                           const char *str, unsigned long len);
DSHDEF DS_RESULT ds_string_builder_appendc(ds_string_builder *sb, char chr);
DSHDEF DS_RESULT ds_string_builder_build(ds_string_builder *sb, char **str);
DSHDEF void ds_string_builder_to_slice(ds_string_builder *sb, ds_string_slice *ss);
DSHDEF void ds_string_builder_free(ds_string_builder *sb);

// IO
//
// The io utils are a simple set of utilities to read and write files.
#ifndef LINE_MAX
#define LINE_MAX 4096
#endif

DSHDEF long ds_io_read(const char *filename, char **buffer, const char *mode);
DSHDEF long ds_io_write(const char *filename, char *buffer,
                        unsigned long buffer_len, const char *mode);


// PRIORITY QUEUE
//
// The priority queue is implemented as a heap, where you can define the
// comparison function to use when inserting items. The comparison function
// should return a positive value if the first item has higher priority than
// the second item, a negative value if the second item has higher priority than
// the first item, and 0 if the items have the same priority.
typedef struct ds_priority_queue {
        ds_dynamic_array items;
        int (*compare)(const void *, const void *);
} ds_priority_queue;

DSHDEF void ds_priority_queue_init_allocator(
    ds_priority_queue *pq, int (*compare)(const void *, const void *),
    unsigned long item_size, DS_ALLOCATOR *allocator);
DSHDEF void ds_priority_queue_init(ds_priority_queue *pq,
                                   int (*compare)(const void *, const void *),
                                   unsigned long item_size);
DSHDEF DS_RESULT ds_priority_queue_insert(ds_priority_queue *pq, void *item);
DSHDEF DS_RESULT ds_priority_queue_pull(ds_priority_queue *pq, void *item);
DSHDEF DS_RESULT ds_priority_queue_peek(ds_priority_queue *pq, void *item);
DSHDEF boolean ds_priority_queue_empty(ds_priority_queue *pq);
DSHDEF void ds_priority_queue_free(ds_priority_queue *pq);

// (DOUBLY) LINKED LIST
//
// The linked list is a simple list that can be used to push and pop items from
// the front and back of the list.
typedef struct ds_linked_list_node {
        void *item;
        struct ds_linked_list_node *prev;
        struct ds_linked_list_node *next;
} ds_linked_list_node;

typedef struct ds_linked_list {
        DS_ALLOCATOR *allocator;
        unsigned long item_size;
        ds_linked_list_node *head;
        ds_linked_list_node *tail;
} ds_linked_list;

DSHDEF void ds_linked_list_init_allocator(ds_linked_list *ll,
                                          unsigned long item_size,
                                          DS_ALLOCATOR *allocator);
DSHDEF void ds_linked_list_init(ds_linked_list *ll, unsigned long item_size);
DSHDEF DS_RESULT ds_linked_list_push_back(ds_linked_list *ll, void *item);
DSHDEF DS_RESULT ds_linked_list_push_front(ds_linked_list *ll, void *item);
DSHDEF DS_RESULT ds_linked_list_pop_back(ds_linked_list *ll, void *item);
DSHDEF DS_RESULT ds_linked_list_pop_front(ds_linked_list *ll, void *item);
DSHDEF boolean ds_linked_list_empty(ds_linked_list *ll);
DSHDEF void ds_linked_list_free(ds_linked_list *ll);

#endif // DS_H

#ifdef DS_PQ_IMPLEMENTATION
#define DS_DA_IMPLEMENTATION
#endif // DS_PQ_IMPLEMENTATION

#ifdef DS_IO_IMPLEMENTATION
#define DS_SB_IMPLEMENTATION
#endif // DS_IO_IMPLEMENTATION

#ifdef DS_SB_IMPLEMENTATION
#define DS_DA_IMPLEMENTATION
#endif // DS_SB_IMPLEMENTATION

#ifdef DS_ARENA_ALLOCATOR_IMPLEMENTATION

DSHDEF void ds_arena_allocator_init(ds_arena_allocator *allocator, void *memory, unsigned long size) {
    allocator->memory = memory;
    allocator->offset = 0;
    allocator->size = size;
}

DSHDEF void *ds_arena_allocator_alloc(ds_arena_allocator *allocator, unsigned long size) {
    char *result = allocator->memory + allocator->offset;

    if (allocator->offset + size > allocator->size) {
        return NULL;
    }

    allocator->offset += size;

    return result;
}

DSHDEF void ds_arena_allocator_clear(ds_arena_allocator *allocator) {
    allocator->offset = 0;
}

DSHDEF void ds_arena_allocator_dump(ds_arena_allocator allocator) {
    fprintf(stdout, "Arena Allocator:\n");
    fprintf(stdout, "Memory: %p\n", allocator.memory);
    fprintf(stdout, "Offset: %lu\n", allocator.offset);
}

#endif // DS_ARENA_ALLOCATOR_IMPLEMENTATION

#ifdef DS_LIST_ALLOCATOR_IMPLEMENTATION

typedef struct ds_list_allocator_node {
    struct ds_list_allocator_node *prev;
    struct ds_list_allocator_node *next;
    boolean free;
    unsigned long size;
} ds_list_allocator_node;

DSHDEF void ds_list_allocator_init(ds_list_allocator *allocator, void *memory, unsigned long size) {
    ds_list_allocator_node *start = (ds_list_allocator_node *)memory;
    start->prev = NULL;
    start->next = NULL;
    start->free = true;
    start->size = size - sizeof(ds_list_allocator_node);

    allocator->memory = start;
}

DSHDEF void ds_list_allocator_dump(ds_list_allocator allocator) {
    ds_list_allocator_node *node = allocator.memory;
    fprintf(stdout, "| start | prev | next | free | size |\n");
    fprintf(stdout, "|-------|------|------|------|------|\n");

    while (node != NULL) {
        fprintf(stdout, "| %p | %p | %p | %u | %lu |\n", node, node->prev, node->next, node->free, node->size);

        node = node->next;
    }
}

static boolean ds_list_allocator_find(ds_list_allocator allocator, unsigned long size, ds_list_allocator_node **find) {
    ds_list_allocator_node *node = allocator.memory;

    while (node != NULL) {
        if (node->free == true && node->size >= size) {
            *find = node;
            return true;
        }

        node = node->next;
    }

    return false;
}

DSHDEF void *ds_list_allocator_alloc(ds_list_allocator *allocator, unsigned long size) {
    ds_list_allocator_node *node = NULL;
    if (!ds_list_allocator_find(*allocator, size, &node)) {
        return NULL;
    }

    unsigned long total_size = sizeof(ds_list_allocator_node) + size;
    if (node->size > total_size) {
        ds_list_allocator_node *split = (ds_list_allocator_node *)((char *)node + total_size);

        split->prev = node;
        split->next = node->next;
        split->free = true;
        split->size = node->size - total_size;

        if (node->next != NULL) {
            node->next->prev = split;
        }

        node->next = split;
        node->size = size;
    }

    node->free = false;

    return (char*)node + sizeof(ds_list_allocator_node);
}

DSHDEF void ds_list_allocator_free(ds_list_allocator *allocator, void *ptr) {
    (void)(allocator); // Suppress unused parameter warning
    ds_list_allocator_node *node = (ds_list_allocator_node *)((char*)ptr - sizeof(ds_list_allocator_node));

    if (node->prev != NULL) {
        ds_list_allocator_node *prev = node->prev;
        if (prev->free) {
            prev->next = node->next;
            prev->size = prev->size + node->size + sizeof(ds_list_allocator_node);

            if (node->next != NULL) {
                node->next->prev = prev;
            }

            node = prev;
        }
    }

    if (node->next != NULL) {
        ds_list_allocator_node *next = node->next;
        if (next->free) {
            if (next->next != NULL) {
                next->next->prev = next->prev;
            }

            node->next = next->next;
            node->size = node->size + next->size + sizeof(ds_list_allocator_node);
        }
    }

    node->free = true;
}

#endif // DS_LIST_ALLOCATOR_IMPLEMENTATION

#ifdef DS_DA_IMPLEMENTATION

// Initialize the dynamic array with a custom allocator
//
// The item_size parameter is the size of each item in the array.
DSHDEF void ds_dynamic_array_init_allocator(ds_dynamic_array *da,
                                            unsigned long item_size,
                                            DS_ALLOCATOR *allocator) {
    da->allocator = allocator;
    da->items = NULL;
    da->item_size = item_size;
    da->count = 0;
    da->capacity = 0;
}

// Initialize the dynamic array
//
// The item_size parameter is the size of each item in the array.
DSHDEF void ds_dynamic_array_init(ds_dynamic_array *da,
                                  unsigned long item_size) {
    ds_dynamic_array_init_allocator(da, item_size, NULL);
}

// Append an item to the dynamic array
//
// Returns 0 if the item was appended successfully, 1 if the array could not be
// reallocated.
DSHDEF DS_RESULT ds_dynamic_array_append(ds_dynamic_array *da,
                                         const void *item) {
    DS_RESULT result = DS_OK;

    if (da->count >= da->capacity) {
        unsigned long new_capacity = da->capacity * 2;
        if (new_capacity == 0) {
            new_capacity = DS_DA_INIT_CAPACITY;
        }

        da->items =
            DS_REALLOC(da->allocator, da->items, da->capacity * da->item_size,
                       new_capacity * da->item_size);

        if (da->items == NULL) {
            DS_LOG_ERROR("Failed to reallocate dynamic array");
            return_defer(DS_ERR);
        }

        da->capacity = new_capacity;
    }

    DS_MEMCPY((char *)da->items + da->count * da->item_size, item,
              da->item_size);
    da->count++;

defer:
    return result;
}

// Pop an item from the dynamic array
//
// Returns 0 if the item was popped successfully, 1 if the array is empty.
// If the item is NULL, then we just pop the item without returning it.
DSHDEF DS_RESULT ds_dynamic_array_pop(ds_dynamic_array *da, const void **item) {
    DS_RESULT result = DS_OK;

    if (da->count == 0) {
        DS_LOG_ERROR("Dynamic array is empty");
        *item = NULL;
        return_defer(DS_ERR);
    }

    if (item != NULL) {
        *item = (char *)da->items + (da->count - 1) * da->item_size;
    }
    da->count--;

defer:
    return result;
}

// Append multiple items to the dynamic array
//
// Returns 0 if the items were appended successfully, 1 if the array could not
// be reallocated.
DSHDEF DS_RESULT ds_dynamic_array_append_many(ds_dynamic_array *da,
                                              void **new_items,
                                              unsigned long new_items_count) {
    DS_RESULT result = DS_OK;

    if (da->count + new_items_count > da->capacity) {
        if (da->capacity == 0) {
            da->capacity = DS_DA_INIT_CAPACITY;
        }
        while (da->count + new_items_count > da->capacity) {
            da->capacity *= 2;
        }

        da->items =
            DS_REALLOC(da->allocator, da->items, da->capacity * da->item_size,
                       da->capacity * da->item_size);
        if (da->items == NULL) {
            DS_LOG_ERROR("Failed to reallocate dynamic array");
            return_defer(DS_ERR);
        }
    }

    DS_MEMCPY((char *)da->items + da->count * da->item_size, new_items,
              new_items_count * da->item_size);
    da->count += new_items_count;

defer:
    return result;
}

// Get an item from the dynamic array
//
// Returns 0 if the item was retrieved successfully, 1 if the index is out of
// bounds.
DSHDEF DS_RESULT ds_dynamic_array_get(ds_dynamic_array *da, unsigned long index,
                                      void *item) {
    DS_RESULT result = DS_OK;

    if (index >= da->count) {
        DS_LOG_ERROR("Index out of bounds");
        return_defer(DS_ERR);
    }

    DS_MEMCPY(item, (char *)da->items + index * da->item_size, da->item_size);

defer:
    return result;
}

// Get a reference to an item from the dynamic array
DSHDEF DS_RESULT ds_dynamic_array_get_ref(ds_dynamic_array *da,
                                          unsigned long index, void **item) {
    DS_RESULT result = DS_OK;

    if (index >= da->count) {
        DS_LOG_ERROR("Index out of bounds %d %d", index, da->count);
        return_defer(DS_ERR);
    }

    *item = (char *)da->items + index * da->item_size;

defer:
    return result;
}

// Copy the dynamic array to another dynamic array
//
// Returns 0 if the array was copied successfully, 1 if the array could not be
// allocated.
DSHDEF DS_RESULT ds_dynamic_array_copy(ds_dynamic_array *da,
                                       ds_dynamic_array *copy) {
    DS_RESULT result = DS_OK;

    copy->items = DS_MALLOC(da->allocator, da->capacity * da->item_size);
    if (copy->items == NULL) {
        DS_LOG_ERROR("Failed to allocate dynamic array items");
        return_defer(DS_ERR);
    }

    copy->item_size = da->item_size;
    copy->count = da->count;
    copy->capacity = da->capacity;

    DS_MEMCPY(copy->items, da->items, da->count * da->item_size);

defer:
    return result;
}

// Sort the dynamic array
//
// This uses the qsort algorithm
DSHDEF void ds_dynamic_array_sort(ds_dynamic_array *da,
                                  int (*compare)(const void *, const void *)) {
    qsort(da->items, da->count, da->item_size, compare);
}

// Reverse the dynamic array
//
// Returns 0 if the array was reversed successfully, 1 if the array could not be
// allocated.
DSHDEF DS_RESULT ds_dynamic_array_reverse(ds_dynamic_array *da) {
    DS_RESULT result = DS_OK;

    for (unsigned long i = 0; i < da->count / 2; i++) {
        unsigned long j = da->count - i - 1;

        if (ds_dynamic_array_swap(da, i, j) != 0) {
            DS_LOG_ERROR("Failed to swap items");
            return_defer(DS_ERR);
        }
    }

defer:
    return result;
}

// Swap two items in the dynamic array
//
// Returns 0 if the items were swapped successfully, 1 if the index is out of
// bounds or if the temporary item could not be allocated.
DSHDEF DS_RESULT ds_dynamic_array_swap(ds_dynamic_array *da,
                                       unsigned long index1,
                                       unsigned long index2) {
    DS_RESULT result = DS_OK;

    if (index1 >= da->count || index2 >= da->count) {
        DS_LOG_ERROR("Index out of bounds");
        return_defer(DS_ERR);
    }

    if (index1 == index2) {
        return_defer(DS_OK);
    }

    void *temp = DS_MALLOC(da->allocator, da->item_size);
    if (temp == NULL) {
        DS_LOG_ERROR("Failed to allocate temporary item");
        return_defer(DS_ERR);
    }

    void *index1_item = NULL;
    if (ds_dynamic_array_get_ref(da, index1, &index1_item) != DS_OK) {
        DS_LOG_ERROR("Could not get item");
        return_defer(DS_ERR);
    }

    void *index2_item = NULL;
    if (ds_dynamic_array_get_ref(da, index2, &index2_item) != DS_OK) {
        DS_LOG_ERROR("Could not get item");
        return_defer(DS_ERR);
    }


    DS_MEMCPY(temp, index1_item, da->item_size);
    DS_MEMCPY(index1_item, index2_item, da->item_size);
    DS_MEMCPY(index2_item, temp, da->item_size);

    DS_FREE(da->allocator, temp);

defer:
    return result;
}

// Delete an item from the dynamic array
//
// Returns 0 in case of succsess. Returns 1 if the index is out of bounds
DSHDEF DS_RESULT ds_dynamic_array_delete(ds_dynamic_array *da,
                                         unsigned long index) {
    DS_RESULT result = DS_OK;

    if (index >= da->count) {
        DS_LOG_ERROR("Index out of bounds");
        return_defer(DS_ERR);
    }

    unsigned int n = da->count - index - 1;

    if (n > 0) {
        void *dest = NULL;
        if (ds_dynamic_array_get_ref(da, index, &dest) != DS_OK) {
            DS_LOG_ERROR("Could not get item");
            return_defer(DS_ERR);
        }

        void *src = NULL;
        if (ds_dynamic_array_get_ref(da, index + 1, &src) != DS_OK) {
            DS_LOG_ERROR("Could not get item");
            return_defer(DS_ERR);
        }

        DS_MEMCPY(dest, src, n * da->item_size);
    }

    da->count -= 1;

defer:
    return result;
}

// Free the dynamic array
DSHDEF void ds_dynamic_array_free(ds_dynamic_array *da) {
    if (da->items != NULL) {
        DS_FREE(da->allocator, da->items);
    }

    da->allocator = NULL;
    da->items = NULL;
    da->count = 0;
    da->capacity = 0;
}

#endif // DS_DA_IMPLEMENTATION

#ifdef DS_SB_IMPLEMENTATION

DSHDEF void ds_string_builder_init_allocator(ds_string_builder *sb,
                                             DS_ALLOCATOR *allocator) {
    ds_dynamic_array_init_allocator(&sb->items, sizeof(char), allocator);
}

// Initialize the string builder
DSHDEF void ds_string_builder_init(ds_string_builder *sb) {
    ds_string_builder_init_allocator(sb, NULL);
}

// Append a formatted string to the string builder
//
// Returns 0 if the string was appended successfully.
DSHDEF DS_RESULT ds_string_builder_appendn(ds_string_builder *sb,
                                           const char *str, unsigned long len) {
    return ds_dynamic_array_append_many(&sb->items, (void **)str, len);
}

// Append a formatted string to the string builder
//
// Returns 0 if the string was appended successfully.
DSHDEF DS_RESULT ds_string_builder_append(ds_string_builder *sb,
                                          const char *format, ...) {
    DS_RESULT result = DS_OK;

    va_list args;
    va_start(args, format);
    int needed = vsnprintf(NULL, 0, format, args);
    va_end(args);

    char *buffer = DS_MALLOC(sb->items.allocator, needed + 1);
    if (buffer == NULL) {
        DS_LOG_ERROR("Failed to allocate string");
        return_defer(DS_ERR);
    }

    va_start(args, format);
    vsnprintf(buffer, needed + 1, format, args);
    va_end(args);

    if (ds_dynamic_array_append_many(&sb->items, (void **)buffer, needed) !=
        DS_OK) {
        return_defer(DS_ERR);
    }

defer:
    if (buffer != NULL) {
        DS_FREE(sb->items.allocator, buffer);
    }
    return result;
}

// Append a character to the string builder
//
// Returns 0 if the character was appended successfully.
DSHDEF DS_RESULT ds_string_builder_appendc(ds_string_builder *sb, char chr) {
    return ds_dynamic_array_append(&sb->items, &chr);
}

// Build the final string from the string builder
//
// Returns 0 if the string was built successfully, 1 if the string could not be
// allocated.
DSHDEF DS_RESULT ds_string_builder_build(ds_string_builder *sb, char **str) {
    DS_RESULT result = DS_OK;

    *str = DS_MALLOC(sb->items.allocator, sb->items.count + 1);
    if (*str == NULL) {
        DS_LOG_ERROR("Failed to allocate string");
        return_defer(DS_ERR);
    }

    DS_MEMCPY(*str, (char *)sb->items.items, sb->items.count);
    (*str)[sb->items.count] = '\0';

defer:
    return result;
}

// Consumes the string builder into a string slice
DSHDEF void ds_string_builder_to_slice(ds_string_builder *sb, ds_string_slice *ss) {
    ss->str = (char *)sb->items.items;
    ss->len = sb->items.count;
    ss->allocator = sb->items.allocator;
}

// Free the string builder
DSHDEF void ds_string_builder_free(ds_string_builder *sb) {
    ds_dynamic_array_free(&sb->items);
}

DSHDEF void ds_string_slice_init_allocator(ds_string_slice *ss, char *str,
                                           unsigned long len,
                                           DS_ALLOCATOR *allocator) {
    ss->allocator = allocator;
    ss->str = str;
    ss->len = len;
}

// Initialize the string slice
DSHDEF void ds_string_slice_init(ds_string_slice *ss, char *str,
                                 unsigned long len) {
    ds_string_slice_init_allocator(ss, str, len, NULL);
}

// Tokenize the string slice by a delimiter
//
// Returns true if a token was found, false if the string slice is empty.
DSHDEF boolean ds_string_slice_tokenize(ds_string_slice *ss, char delimiter,
                                        ds_string_slice *token) {
    boolean result = true;

    if (ss->len == 0 || ss->str == NULL) {
        return_defer(false);
    }

    token->str = ss->str;
    token->len = 0;

    for (unsigned int i = 0; i < ss->len; i++) {
        if (ss->str[i] == delimiter) {
            token->len = i;
            ss->str += i + 1;
            ss->len -= i + 1;
            return_defer(true);
        }
    }

    token->len = ss->len;
    ss->str += ss->len;
    ss->len = 0;

defer:
    return result;
}

// Build a token by taking from the string slice while the predicate holds true
//
// Returns true if a token was found, false if the string slice is empty.
DSHDEF boolean ds_string_slice_take_while_pred(ds_string_slice *ss,
                                               boolean (*predicate)(char),
                                               ds_string_slice *token) {
    boolean result = true;

    if (ss->len == 0 || ss->str == NULL) {
        return_defer(false);
    }

    token->str = ss->str;
    token->len = 0;

    for (unsigned int i = 0; i < ss->len; i++) {
        if (predicate(ss->str[i]) == 0) {
            token->len = i;
            ss->str += i;
            ss->len -= i;
            return_defer(true);
        }
    }

    token->len = ss->len;
    ss->str += ss->len;
    ss->len = 0;

defer:
    return result;
}

// Trim the left side of the string slice by whitespaces
DSHDEF void ds_string_slice_trim_left_ws(ds_string_slice *ss) {
    while (ss->len > 0 && isspace(ss->str[0])) {
        ss->str++;
        ss->len--;
    }
}


// Trim the left side of the string slice by whitespaces
DSHDEF void ds_string_slice_trim_right_ws(ds_string_slice *ss) {
    while (ss->len > 0 && isspace(ss->str[ss->len - 1])) {
        ss->len--;
    }
}

// Trim the left side of the string slice by a character
DSHDEF void ds_string_slice_trim_left(ds_string_slice *ss, char chr) {
    while (ss->len > 0 && ss->str[0] == chr) {
        ss->str++;
        ss->len--;
    }
}

// Trim the right side of the string slice by a character
DSHDEF void ds_string_slice_trim_right(ds_string_slice *ss, char chr) {
    while (ss->len > 0 && ss->str[ss->len - 1] == chr) {
        ss->len--;
    }
}

// Trim the string slice by a character
DSHDEF void ds_string_slice_trim(ds_string_slice *ss, char chr) {
    ds_string_slice_trim_left(ss, chr);
    ds_string_slice_trim_right(ss, chr);
}

// Convert the string slice to an owned string
//
// Returns 0 if the string was converted successfully, 1 if the string could not
// be allocated.
DSHDEF DS_RESULT ds_string_slice_to_owned(ds_string_slice *ss, char **str) {
    DS_RESULT result = DS_OK;

    *str = DS_MALLOC(ss->allocator, ss->len + 1);
    if (*str == NULL) {
        DS_LOG_ERROR("Failed to allocate string");
        return_defer(DS_ERR);
    }

    DS_MEMCPY(*str, ss->str, ss->len);
    (*str)[ss->len] = '\0';

defer:
    return result;
}

// Check if the string slice is equal to a specific string
//
// Returns 1 in case the string slice equals str. Returns 0 otherwise
DSHDEF boolean ds_string_slice_equals(ds_string_slice *ss, ds_string_slice *so) {
    return ss->len == so->len && DS_MEMCMP(ss->str, so->str, ss->len) == 0;
}

// Check if the string slice starts with a specific string
//
// Returns 1 in case the string slice starts with str. Returns 0 otherwise
DSHDEF boolean ds_string_slice_starts_with(ds_string_slice *ss,
                                           ds_string_slice *prefix) {
    return DS_MEMCMP(ss->str, prefix->str, prefix->len) == 0;
}

// Check if the string slice starts with a char that matches a predicate function
//
// Returns 1 if the string slice starts with a predicate. Returns 0 otherwise.
DSHDEF boolean ds_string_slice_starts_with_pred(ds_string_slice *ss,
                                                boolean (*predicate)(char)) {
    return predicate(*ss->str);
}

// Step the string slice by one character forward
DSHDEF void ds_string_slice_step(ds_string_slice *ss, int count) {
    ss->str += count;
    ss->len -= count;
}

// Check if the string slice is empty
DSHDEF boolean ds_string_slice_empty(ds_string_slice *ss) {
    return ss->len == 0;
}

// Free the string slice
DSHDEF void ds_string_slice_free(ds_string_slice *ss) {
    ss->allocator = NULL;
    ss->str = NULL;
    ss->len = 0;
}

#endif // DS_SB_IMPLEMENTATION

#ifdef DS_IO_IMPLEMENTATION

// Read a file
//
// Reads the contents of a binary file into a buffer.
//
// Arguments:
// - filename: name of the file to read
// - buffer: pointer to the buffer to store the contents of the file
// - mode: the mode to read in
//
// Returns:
// - the number of bytes read
DSHDEF long ds_io_read(const char *filename, char **buffer, const char *mode) {
    long result = 0;

    unsigned long line_size;
    FILE *file = NULL;
    ds_string_builder sb;
    ds_string_builder_init(&sb);

    if (filename != NULL) {
        file = fopen(filename, mode);
        if (file == NULL) {
            DS_LOG_ERROR("Failed to open file: %s", filename);
            return_defer(-1);
        }
    } else {
        file = stdin;
    }

    char line[LINE_MAX] = {0};
    do {
        line_size = fread(line, sizeof(char), LINE_MAX, file);

        if (ds_string_builder_appendn(&sb, line, line_size) != 0) {
            DS_LOG_ERROR("Failed to append line to string builder");
            return_defer(-1);
        }

        memset(line, 0, sizeof(line));
    } while (line_size > 0);

    if (ds_string_builder_build(&sb, buffer) != 0) {
        DS_LOG_ERROR("Failed to build string from string builder");
        return_defer(-1);
    }

    result = sb.items.count;

defer:
    if (filename != NULL && file != NULL)
        fclose(file);
    ds_string_builder_free(&sb);

    return result;
}

// Write a file
//
// Writes the contents of a buffer into a binary file.
//
// Arguments:
// - filename: name of the file to read
// - buffer: pointer to the buffer to use on write
// - buffer_len: the size of the buffer
// - mode: the mode to write in
//
// Returns:
// - the number of bytes written
DSHDEF long ds_io_write(const char *filename, char *buffer,
                        unsigned long buffer_len, const char *mode) {
    long result = 0;

    unsigned long buffer_size;
    FILE *file = NULL;

    if (filename != NULL) {
        file = fopen(filename, mode);
        if (file == NULL) {
            DS_LOG_ERROR("Failed to open file: %s", filename);
            return_defer(-1);
        }
    } else {
        file = stdout;
    }

    buffer_size = fwrite(buffer, sizeof(char), buffer_len, file);
    result = buffer_size;

defer:
    if (filename != NULL && file != NULL)
        fclose(file);

    return result;
}

#endif // DS_IO_IMPLEMENTATION

#ifdef DS_PQ_IMPLEMENTATION

// Initialize the priority queue with a custom allocator
DSHDEF void ds_priority_queue_init_allocator(
    ds_priority_queue *pq, int (*compare)(const void *, const void *),
    unsigned long item_size, DS_ALLOCATOR *allocator) {
    ds_dynamic_array_init_allocator(&pq->items, item_size, allocator);

    pq->compare = compare;
}

// Initialize the priority queue
DSHDEF void ds_priority_queue_init(ds_priority_queue *pq,
                                   int (*compare)(const void *, const void *),
                                   unsigned long item_size) {
    ds_priority_queue_init_allocator(pq, compare, item_size, NULL);
}

// Insert an item into the priority queue
//
// Returns 0 if the item was inserted successfully.
DSHDEF DS_RESULT ds_priority_queue_insert(ds_priority_queue *pq, void *item) {
    DS_RESULT result = DS_OK;
    ds_dynamic_array_append(&pq->items, item);

    int index = pq->items.count - 1;
    int parent = (index - 1) / 2;

    void *index_item = NULL;
    if (ds_dynamic_array_get_ref(&pq->items, index, &index_item) != DS_OK) {
        DS_LOG_ERROR("Could not get item");
        return_defer(DS_ERR);
    }

    void *parent_item = NULL;
    if (ds_dynamic_array_get_ref(&pq->items, parent, &parent_item) != DS_OK) {
        DS_LOG_ERROR("Could not get item");
        return_defer(DS_ERR);
    }

    while (index != 0 && pq->compare(index_item, parent_item) > 0) {
        ds_dynamic_array_swap(&pq->items, index, parent);

        index = parent;
        parent = (index - 1) / 2;

        if (ds_dynamic_array_get_ref(&pq->items, index, &index_item) != DS_OK) {
            DS_LOG_ERROR("Could not get item");
            return_defer(DS_ERR);
        }
        if (ds_dynamic_array_get_ref(&pq->items, parent, &parent_item) != DS_OK) {
            DS_LOG_ERROR("Could not get item");
            return_defer(DS_ERR);
        }
    }

defer:
    return result;
}

// Pull the item with the highest priority from the priority queue
//
// Returns 0 if an item was pulled successfully, 1 if the priority queue is
// empty.
DSHDEF DS_RESULT ds_priority_queue_pull(ds_priority_queue *pq, void *item) {
    DS_RESULT result = DS_OK;

    if (pq->items.count == 0) {
        DS_LOG_ERROR("Priority queue is empty");
        return_defer(DS_ERR);
    }

    if (ds_dynamic_array_get(&pq->items, 0, item) != DS_OK) {
        return_defer(DS_ERR);
    }
    ds_dynamic_array_swap(&pq->items, 0, pq->items.count - 1);

    unsigned int index = 0;
    unsigned int swap = index;
    void *swap_item = NULL;
    do {
        index = swap;

        unsigned int left = 2 * index + 1;
        if (left < pq->items.count - 1) {
            void *left_item = NULL;
            if (ds_dynamic_array_get_ref(&pq->items, swap, &swap_item) != DS_OK) {
                DS_LOG_ERROR("Could not get item");
                return_defer(DS_ERR);
            }
            if (ds_dynamic_array_get_ref(&pq->items, left, &left_item) != DS_OK) {
                DS_LOG_ERROR("Could not get item");
                return_defer(DS_ERR);
            }
            if (pq->compare(left_item, swap_item) > 0) {
                swap = left;
            }
        }

        unsigned int right = 2 * index + 2;
        if (right < pq->items.count - 1) {
            void *right_item = NULL;
            if (ds_dynamic_array_get_ref(&pq->items, swap, &swap_item) != DS_OK) {
                DS_LOG_ERROR("Could not get item");
                return_defer(DS_ERR);
            }
            if (ds_dynamic_array_get_ref(&pq->items, right, &right_item) != DS_OK) {
                DS_LOG_ERROR("Could not get item");
                return_defer(DS_ERR);
            }
            if (pq->compare(right_item, swap_item) > 0) {
                swap = right;
            }
        }

        ds_dynamic_array_swap(&pq->items, index, swap);
    } while (swap != index);

    pq->items.count--;

defer:
    return result;
}

// Peek at the item with the highest priority in the priority queue
//
// Returns 0 if an item was peeked successfully, 1 if the priority queue is
// empty.
DSHDEF DS_RESULT ds_priority_queue_peek(ds_priority_queue *pq, void *item) {
    int result = DS_OK;

    if (pq->items.count == 0) {
        DS_LOG_ERROR("Priority queue is empty");
        return_defer(DS_ERR);
    }

    if (ds_dynamic_array_get(&pq->items, 0, item) != DS_OK) {
        return_defer(DS_ERR);
    }

defer:
    return result;
}

// Check if the priority queue is empty
DSHDEF boolean ds_priority_queue_empty(ds_priority_queue *pq) {
    return pq->items.count == 0;
}

// Free the priority queue
DSHDEF void ds_priority_queue_free(ds_priority_queue *pq) {
    ds_dynamic_array_free(&pq->items);

    pq->compare = NULL;
}

#endif // DS_PQ_IMPLEMENTATION

#ifdef DS_LL_IMPLEMENTATION

// Initialize the linked list with a custom allocator
//
// The item_size parameter is the size of each item in the list.
DSHDEF void ds_linked_list_init_allocator(ds_linked_list *ll,
                                          unsigned long item_size,
                                          DS_ALLOCATOR *allocator) {
    ll->allocator = allocator;
    ll->item_size = item_size;
    ll->head = NULL;
    ll->tail = NULL;
}

// Initialize the linked list
//
// The item_size parameter is the size of each item in the list.
DSHDEF void ds_linked_list_init(ds_linked_list *ll, unsigned long item_size) {
    ds_linked_list_init_allocator(ll, item_size, NULL);
}

// Push an item to the back of the linked list
//
// Returns 0 if the item was pushed successfully, 1 if the list could not be
// allocated.
DSHDEF DS_RESULT ds_linked_list_push_back(ds_linked_list *ll, void *item) {
    DS_RESULT result = DS_OK;

    ds_linked_list_node *node =
        DS_MALLOC(ll->allocator, sizeof(ds_linked_list_node));
    if (node == NULL) {
        DS_LOG_ERROR("Failed to allocate linked list node");
        return_defer(DS_ERR);
    }

    node->item = DS_MALLOC(ll->allocator, ll->item_size);
    if (node->item == NULL) {
        DS_LOG_ERROR("Failed to allocate linked list item");
        return_defer(DS_ERR);
    }

    DS_MEMCPY(node->item, item, ll->item_size);
    node->prev = ll->tail;
    node->next = NULL;

    if (ll->tail != NULL) {
        ll->tail->next = node;
    }
    ll->tail = node;

    if (ll->head == NULL) {
        ll->head = node;
    }

defer:
    if (result != DS_OK && node != NULL) {
        if (node->item != NULL) {
            DS_FREE(ll->allocator, node->item);
        }
        DS_FREE(ll->allocator, node);
    }
    return result;
}

// Push an item to the front of the linked list
//
// Returns 0 if the item was pushed successfully, 1 if the list could not be
// allocated.
DSHDEF DS_RESULT ds_linked_list_push_front(ds_linked_list *ll, void *item) {
    DS_RESULT result = DS_OK;

    ds_linked_list_node *node =
        DS_MALLOC(ll->allocator, sizeof(ds_linked_list_node));
    if (node == NULL) {
        DS_LOG_ERROR("Failed to allocate linked list node");
        return_defer(DS_ERR);
    }

    node->item = DS_MALLOC(ll->allocator, ll->item_size);
    if (node->item == NULL) {
        DS_LOG_ERROR("Failed to allocate linked list item");
        return_defer(DS_ERR);
    }

    DS_MEMCPY(node->item, item, ll->item_size);
    node->prev = NULL;
    node->next = ll->head;

    if (ll->head != NULL) {
        ll->head->prev = node;
    }
    ll->head = node;

    if (ll->tail == NULL) {
        ll->tail = node;
    }

defer:
    if (result != DS_OK && node != NULL) {
        if (node->item != NULL) {
            DS_FREE(ll->allocator, node->item);
        }
        DS_FREE(ll->allocator, node);
    }
    return result;
}

// Pop an item from the back of the linked list
//
// Returns 0 if the item was popped successfully, 1 if the list is empty.
// The item is stored in the item parameter.
DSHDEF DS_RESULT ds_linked_list_pop_back(ds_linked_list *ll, void *item) {
    ds_linked_list_node *node = NULL;
    DS_RESULT result = DS_OK;

    if (ll->tail == NULL) {
        DS_LOG_ERROR("Linked list is empty");
        return_defer(DS_ERR);
    }

    node = ll->tail;
    DS_MEMCPY(item, node->item, ll->item_size);

    ll->tail = node->prev;
    if (ll->tail != NULL) {
        ll->tail->next = NULL;
    }

    if (node == ll->head) {
        ll->head = NULL;
    }

defer:
    if (node != NULL) {
        if (node->item != NULL) {
            DS_FREE(ll->allocator, node->item);
        }
        DS_FREE(ll->allocator, node);
    }
    return result;
}

// Pop an item from the front of the linked list
//
// Returns 0 if the item was popped successfully, 1 if the list is empty.
// The item is stored in the item parameter.
DSHDEF DS_RESULT ds_linked_list_pop_front(ds_linked_list *ll, void *item) {
    ds_linked_list_node *node = NULL;
    DS_RESULT result = DS_OK;

    if (ll->head == NULL) {
        DS_LOG_ERROR("Linked list is empty");
        return_defer(DS_ERR);
    }

    node = ll->head;
    DS_MEMCPY(item, node->item, ll->item_size);

    ll->head = node->next;
    if (ll->head != NULL) {
        ll->head->prev = NULL;
    }

    if (node == ll->tail) {
        ll->tail = NULL;
    }

defer:
    if (node != NULL) {
        if (node->item != NULL) {
            DS_FREE(ll->allocator, node->item);
        }
        DS_FREE(ll->allocator, node);
    }
    return result;
}

// Check if the linked list is empty
//
// Returns 1 if the list is empty, 0 if the list is not empty.
DSHDEF boolean ds_linked_list_empty(ds_linked_list *ll) { return ll->head == NULL; }

DSHDEF void ds_linked_list_free(ds_linked_list *ll) {
    ds_linked_list_node *node = ll->head;
    while (node != NULL) {
        ds_linked_list_node *next = node->next;
        if (node->item != NULL) {
            DS_FREE(ll->allocator, node->item);
        }
        DS_FREE(ll->allocator, node);
        node = next;
    }

    ll->allocator = NULL;
    ll->item_size = 0;
    ll->head = NULL;
    ll->tail = NULL;
}

#endif // DS_LL_IMPLEMENTATION
