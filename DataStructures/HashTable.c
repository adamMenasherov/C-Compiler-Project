#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"


unsigned int hash(const char* key) {
    if (!key) return 0;
    unsigned int h = 0;
    while (*key)
        h = h * 31 + *key++;
    return h % TABLE_SIZE;
}

HashTable* createHashTable() {
    HashTable* table = calloc(1, sizeof(HashTable));
    return table;
}

void ht_insert(HashTable* table, const char* key, int value) {
    unsigned int index = hash(key);
    Entry* entry = table->buckets[index];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    // Insert new entry at front of chain
    Entry* new_entry = malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
}

// Returns 1 if found, 0 if not. Puts value in *out_value.
int ht_get(HashTable* table, const char* key, int* out_value) {
    unsigned int index = hash(key);
    Entry* entry = table->buckets[index];

    while (entry) {
        if (entry->key && strcmp(entry->key, key) == 0) {
            *out_value = entry->value;
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

void ht_delete(HashTable* table, const char* key) {
    unsigned int index = hash(key);
    Entry* entry = table->buckets[index];
    Entry* prev = NULL;

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) prev->next = entry->next;
            else table->buckets[index] = entry->next;
            free(entry->key);
            free(entry);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

void freeHashTable(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry* entry = table->buckets[i];
        while (entry) {
            Entry* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(table);
}


void ht_print(HashTable* table) {
    printf("{\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry* entry = table->buckets[i];
        while (entry) {
            printf("  \"%s\": %d\n", entry->key, entry->value);
            entry = entry->next;
        }
    }
    printf("}\n");
}