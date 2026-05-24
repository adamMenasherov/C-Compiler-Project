#pragma once
#include <stddef.h>
#define TABLE_SIZE 64

typedef struct Entry {
    void* key;
    struct Entry* next;  // for chaining collisions
} Entry;

typedef struct {
    Entry** buckets;
    size_t (*hashFunc)(void* key);
    int (*equalsFunc)(void* lhs, void* rhs);
    size_t bucket_count;
} HashTable;


HashTable* createHashTable(size_t (*hashFunc)(void* key), int (*equalsFunc)(void* lhs, void* rhs));
int ht_insert(HashTable* table, void* key);
void* ht_getKey(HashTable* table, void* key);
int ht_contains(HashTable* table, void* key);
int ht_delete(HashTable* table, void* key, void (*freeKey)(void*));
void freeHashTable(HashTable* table, void (*freeKey)(void*));
void ht_print(HashTable* table, void (*printKey)(void*));
