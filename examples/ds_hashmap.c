#define DS_HM_IMPLEMENTATION
#include "../ds.h"

// Example

#define MAX_CAPACITY 100

unsigned long my_hash(const void *key) {
    unsigned long hash = 0;
    char *name = (char *)key;
    for (unsigned int i = 0; i < strlen(name); i++) {
        hash = 31 * hash + name[i];
    }
    return hash % MAX_CAPACITY;
}

int my_compare(const void *k1, const void *k2) {
    return strcmp((char *)k1, (char *)k2);
}

void my_map_print(ds_hashmap map) {
    for (unsigned int i = 0; i < map.capacity; i++) {
        if (map.buckets[i].count == 0) {
            continue;
        }

        for (unsigned int j = 0; j < map.buckets[i].count; j++) {
            ds_hashmap_kv kv = {0};
            ds_dynamic_array_get(&map.buckets[i], j, &kv);

            printf("%s\n", (char *)kv.key);
        }
    }
}

int main() {
    int result = 0;
    ds_hashmap map = {0};

    if (ds_hashmap_init(&map, MAX_CAPACITY, my_hash, my_compare) != DS_OK) {
        DS_LOG_ERROR("Error initializing hashmap");
        return_defer(-1);
    }

    {
        ds_hashmap_kv kv = { .key = "name", .value = "John" };
        if (ds_hashmap_insert(&map, &kv) != DS_OK) {
            DS_LOG_ERROR("Could not insert into hashmap");
            return_defer(-1);
        }
    }

    {
        int age = 3;
        ds_hashmap_kv kv = { .key = "age", .value = &age };
        if (ds_hashmap_insert(&map, &kv) != DS_OK) {
            DS_LOG_ERROR("Could not insert into hashmap");
            return_defer(-1);
        }
    }

    {
        ds_hashmap_kv kv = { .key = "name", .value = NULL };
        if (ds_hashmap_get(&map, &kv) != DS_OK) {
            DS_LOG_ERROR("Could not get from hashmap");
            return_defer(-1);
        }

        DS_LOG_INFO("The name is %s", (char *)kv.value);
    }

    {
        ds_hashmap_kv kv = { .key = "name", .value = NULL };
        if (ds_hashmap_get(&map, &kv) != DS_OK) {
            DS_LOG_ERROR("Could not get from hashmap");
            return_defer(-1);
        }

        DS_LOG_INFO("The name is %s", (char *)kv.value);
    }

    {
        DS_LOG_INFO("Keys before delete:");
        my_map_print(map);
        if (ds_hashmap_delete(&map, "age") != DS_OK) {
            DS_LOG_ERROR("Could not delete from hashmap");
            return_defer(-1);
        }
        DS_LOG_INFO("Keys after delete:");
        my_map_print(map);
    }

defer:
    ds_hashmap_free(&map);
    return result;
}
