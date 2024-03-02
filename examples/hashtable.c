#define DS_HT_IMPLEMENTATION
#include "../ds.h"

unsigned int hash(const void *key) {
    unsigned int hash = 0;
    char *k = *(char **)key;
    for (int i = 0; i < strlen(k); i++) {
        hash = 31 * hash + k[i];
    }
    return hash;
}

int compare(const void *a, const void *b) {
    char *k1 = *(char **)a;
    char *k2 = *(char **)b;
    return strcmp(k1, k2);
}

int main() {
    int result = 0;

    ds_hash_table ht;
    ds_hash_table_init(&ht, sizeof(char *), sizeof(int), 100, hash, compare);

    char *key = "keyaaaaaaaa";
    int value = 42;
    ds_hash_table_insert(&ht, &key, &value);

    for (int i = 0; i < ht.capacity; i++) {
        for (int j = 0; j < ht.keys[i].count; j++) {
            char *k;
            ds_dynamic_array_get(&ht.keys[i], j, &k);

            int v;
            ds_dynamic_array_get(&ht.values[i], j, &v);

            printf("index: %d, key: %s, value: %d\n", i, k, v);
        }
    }

    char *find_key = "keyaaaaaaaa";
    value = 0;
    if (ds_hash_table_get(&ht, &find_key, &value) != 0) {
        printf("key not found\n");
    } else {
        printf("value: %d\n", value);
    }

defer:
    ds_hash_table_free(&ht);
    return result;
}

