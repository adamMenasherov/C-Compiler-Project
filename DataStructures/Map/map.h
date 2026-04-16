#pragma once
#include <stdlib.h>
#define MAP_SIZE 64

typedef struct MapEntry {
    void* key;
    void* value;
    int isInScope;
    int hasExternalLinkage;
    struct MapEntry* next; // For handling collisions (chaining)
} MapEntry;

typedef struct {
    MapEntry** buckets;
    size_t (*hashFunc)(void* key);
    int (*equalsFunc)(void* lhs, void* rhs);
    size_t bucket_count;
} Map;

Map* createMap(size_t (*hashFunc)(void* key), int (*equalsFunc)(void* lhs, void* rhs));
Map* copyMap(Map* original, void* (*copyKey)(void*), void* (*copyValue)(void*));
void freeMap(Map* map, void (*freeKey)(void*), void (*freeValue)(void*));
int mapPut(Map* map, void* key, void* value, int isInScope, int hasExternalLinkage);
MapEntry* mapGetEntry(Map* map, void* key);
void* mapGet(Map* map, void* key);
int mapContainsKey(Map* map, void* key);
int mapRemove(Map* map, void* key, void (*freeKey)(void*), void (*freeValue)(void*)); 