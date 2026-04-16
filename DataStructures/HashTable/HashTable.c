#include <stdio.h>
#include <stdlib.h>
#include "HashTable.h"


HashTable* createHashTable(size_t (*hashFunc)(void* key), int (*equalsFunc)(void* lhs, void* rhs)) {
    HashTable* table = malloc(sizeof(HashTable));
    if (!table) return NULL;

    table->buckets = calloc(TABLE_SIZE, sizeof(Entry*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }

    table->hashFunc = hashFunc;
    table->equalsFunc = equalsFunc;
    table->bucket_count = TABLE_SIZE;
    return table;
}

int ht_insert(HashTable* table, void* key) {
    size_t index;
    Entry* entry;

    if (!table || !table->hashFunc || !table->equalsFunc) return 0;

    index = table->hashFunc(key) % table->bucket_count;
    entry = table->buckets[index];

    while (entry) {
        if (table->equalsFunc(entry->key, key)) {
            return 1;
        }
        entry = entry->next;
    }

    Entry* new_entry = malloc(sizeof(Entry));
    if (!new_entry) return 0;
    new_entry->key = key;
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    return 1;
}

void* ht_getKey(HashTable* table, void* key) {
    size_t index;
    Entry* entry;

    if (!table || !table->hashFunc || !table->equalsFunc) return NULL;

    index = table->hashFunc(key) % table->bucket_count;
    entry = table->buckets[index];

    while (entry) {
        if (table->equalsFunc(entry->key, key)) {
            return entry->key;
        }
        entry = entry->next;
    }

    return NULL;
}

int ht_contains(HashTable* table, void* key) {
    return ht_getKey(table, key) != NULL;
}

int ht_delete(HashTable* table, void* key, void (*freeKey)(void*)) {
    size_t index;
    Entry* entry;
    Entry* prev = NULL;

    if (!table || !table->hashFunc || !table->equalsFunc) return 0;

    index = table->hashFunc(key) % table->bucket_count;
    entry = table->buckets[index];

    while (entry) {
        if (table->equalsFunc(entry->key, key)) {
            if (prev) prev->next = entry->next;
            else table->buckets[index] = entry->next;
            if (freeKey) freeKey(entry->key);
            free(entry);
            return 1;
        }
        prev = entry;
        entry = entry->next;
    }

    return 0;
}

void freeHashTable(HashTable* table, void (*freeKey)(void*)) {
    size_t i;

    if (!table) return;

    for (i = 0; i < table->bucket_count; i++) {
        Entry* entry = table->buckets[i];
        while (entry) {
            Entry* next = entry->next;
            if (freeKey) freeKey(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(table->buckets);
    free(table);
}


void ht_print(HashTable* table, void (*printKey)(void*)) {
    size_t i;

    if (!table || !printKey) return;

    printf("{\n");
    for (i = 0; i < table->bucket_count; i++) {
        Entry* entry = table->buckets[i];
        while (entry) {
            printf("  ");
            printKey(entry->key);
            printf("\n");
            entry = entry->next;
        }
    }
    printf("}\n");
}