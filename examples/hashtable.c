#define DS_HT_IMPLEMENTATION
#include "../ds.h"

unsigned int hash(const void *key) {
    unsigned int hash = 0;
    char *k = *(char **)key;
    for (size_t i = 0; i < strlen(k); i++) {
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

    char *key1 = "key";
    int value1 = 42;
    ds_hash_table_insert(&ht, &key1, &value1);

    char *key2 = "key";
    int value2 = 69;
    ds_hash_table_insert(&ht, &key2, &value2);

    char *key3 = "key2";
    int value3 = 420;
    ds_hash_table_insert(&ht, &key3, &value3);

    char *find_key = "key";
    int value = 0;
    if (ds_hash_table_get(&ht, &find_key, &value) != 0) {
        printf("key not found\n");
    } else {
        printf("value: %d\n", value);
    }

    find_key = "key2";
    if (ds_hash_table_get(&ht, &find_key, &value) != 0) {
        printf("key not found\n");
    } else {
        printf("value: %d\n", value);
    }

    return_defer(0);

defer:
    ds_hash_table_free(&ht);
    return result;
}

