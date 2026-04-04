#pragma once
#define TABLE_SIZE 64

typedef struct Entry {
    char* key;
    int value;
    struct Entry* next;  // for chaining collisions
} Entry;

typedef struct {
    Entry* buckets[TABLE_SIZE];
} HashTable;


unsigned int hash(const char* key);
HashTable* createHashTable();
void ht_insert(HashTable* table, const char* key, int value);
int ht_get(HashTable* table, const char* key, int* out_value);
void ht_delete(HashTable* table, const char* key);
void freeHashTable(HashTable* table);
void ht_print(HashTable* table);
