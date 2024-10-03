#define DS_DA_IMPLEMENTATION
#include "../ds.h"

typedef struct ds_hashmap_kv {
    void *key;
    void *value;
} ds_hashmap_kv;

typedef struct ds_hashmap {
    ds_dynamic_array *buckets; /* ds_hashmap_kv */
    unsigned int capacity;
    unsigned int (*hash)(const void *);
    int (*compare)(const void *, const void *);
} ds_hashmap;

DSHDEF int ds_hashmap_init(ds_hashmap *map, unsigned int capacity,
                              unsigned int (*hash)(const void *),
                              int (*compare)(const void *, const void *));
DSHDEF int ds_hashmap_insert(ds_hashmap *map, ds_hashmap_kv *kv);
DSHDEF int ds_hashmap_get(ds_hashmap *map, ds_hashmap_kv *kv);
DSHDEF int ds_hashmap_delete(ds_hashmap *map, const void *key);
DSHDEF int ds_hashmap_free(ds_hashmap *map);

DSHDEF int ds_hashmap_init(ds_hashmap *map, unsigned int capacity,
                              unsigned int (*hash)(const void *),
                              int (*compare)(const void *, const void *)) {
    int result = 0;

    map->capacity = capacity;

    map->buckets = DS_MALLOC(NULL, capacity * sizeof(ds_dynamic_array));
    if (map->buckets == NULL) {
        DS_LOG_ERROR("Failed to allocate hashmap buckets");
        return_defer(1);
    }

    for (int i = 0; i < map->capacity; i++) {
        ds_dynamic_array_init(map->buckets + i, sizeof(ds_hashmap_kv));
    }

    map->hash = hash;
    map->compare = compare;

defer:
    return result;
}

DSHDEF int ds_hashmap_insert(ds_hashmap *map, ds_hashmap_kv *kv) {
    int result = 0;

    unsigned int index = map->hash(kv->key) % map->capacity;

    if (ds_dynamic_array_append(map->buckets + index, kv) != 0) {
        DS_LOG_ERROR("Failed to insert item into bucket");
        return_defer(1);
    }

defer:
    return result;
}

DSHDEF int ds_hashmap_get(ds_hashmap *map, ds_hashmap_kv *kv) {
    int result = 0;
    int found = 0;

    unsigned int index = map->hash(kv->key) % map->capacity;
    ds_dynamic_array *bucket = map->buckets + index;

    for (int i = 0; bucket->count; i++) {
        ds_hashmap_kv tmp = {0};
        ds_dynamic_array_get(bucket, i, &tmp);

        if (map->compare(kv->key, tmp.key) == 0) {
            kv->value = tmp.value;
            found = 1;
            break;
        }
    }

    if (found == 0) {
        DS_LOG_ERROR("Failed to find item in hashmap");
        return_defer(1);
    }

defer:
    return result;
}

DSHDEF int ds_hashmap_delete(ds_hashmap *map, const void *key) {
    int result = 0;
    int found = 0;

    unsigned int index = map->hash(key) % map->capacity;
    ds_dynamic_array *bucket = map->buckets + index;

    for (int i = 0; bucket->count; i++) {
        ds_hashmap_kv tmp = {0};
        ds_dynamic_array_get(bucket, i, &tmp);

        if (map->compare(key, tmp.key) == 0) {
            // ds_dynamic_array_delete(bucket, i);
            // ds_dyanmic_array_update(bucket, i, item);
            found = 1;
            break;
        }
    }

    if (found == 0) {
        DS_LOG_ERROR("Failed to find item in hashmap");
        return_defer(1);
    }

defer:
    return result;
}

DSHDEF int ds_hashmap_free(ds_hashmap *map) {
    int result = 0;

    for (int i = 0; i < map->capacity; i++) {
        ds_dynamic_array_free(map->buckets + i);
    }

defer:
    return result;
}

// Example

#define MAX_CAPACITY 100

unsigned int my_hash(const void *key) {
    unsigned int hash = 0;
    char *name = (char *)key;
    for (int i = 0; i < strlen(name); i++) {
        hash = 31 * hash + name[i];
    }
    return hash % MAX_CAPACITY;
}

int my_compare(const void *k1, const void *k2) {
    return strcmp((char *)k1, (char *)k2);
}

int main() {
    int result = 0;
    ds_hashmap map = {0};

    if (ds_hashmap_init(&map, MAX_CAPACITY, my_hash, my_compare) != 0) {
        DS_LOG_ERROR("Error initializing hashmap");
        return_defer(-1);
    }

    {
        ds_hashmap_kv kv = { .key = "name", .value = "John" };
        if (ds_hashmap_insert(&map, &kv) != 0) {
            DS_LOG_ERROR("Could not insert into hashmap");
            return_defer(-1);
        }
    }

    {
        int age = 3;
        ds_hashmap_kv kv = { .key = "age", .value = &age };
        if (ds_hashmap_insert(&map, &kv) != 0) {
            DS_LOG_ERROR("Could not insert into hashmap");
            return_defer(-1);
        }
    }

    {
        ds_hashmap_kv kv = { .key = "name", .value = NULL };
        if (ds_hashmap_get(&map, &kv) != 0) {
            DS_LOG_ERROR("Could not get from hashmap");
            return_defer(-1);
        }

        DS_LOG_INFO("The name is %s", (char *)kv.value);
    }

    {
        if (ds_hashmap_delete(&map, "age") != 0) {
            DS_LOG_ERROR("Could not delete from hashmap");
            return_defer(-1);
        }
    }

defer:
    ds_hashmap_free(&map);
    return result;
}
