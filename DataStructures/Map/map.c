#include "map.h"

Map* createMap(size_t (*hashFunc)(void* key), int (*equalsFunc)(void* lhs, void* rhs)) {
    Map* map = malloc(sizeof(Map));
    if (!map) return NULL;

    map->bucket_count = MAP_SIZE;
    map->buckets = calloc(MAP_SIZE, sizeof(MapEntry*));
    map->hashFunc = hashFunc;
    map->equalsFunc = equalsFunc;
    if (!map->buckets) {
        free(map);
        return NULL;
    }

    return map;
}

Map* copyMap(Map* original, void* (*copyKey)(void*), void* (*copyValue)(void*)) {
    if (!original) return NULL;

    Map* newMap = createMap(original->hashFunc, original->equalsFunc);
    if (!newMap) return NULL;

    for (size_t i = 0; i < original->bucket_count; i++) {
        MapEntry* entry = original->buckets[i];
        while (entry) {
            void* newKey = copyKey ? copyKey(entry->key) : entry->key;
            void* newValue = copyValue ? copyValue(entry->value) : entry->value;
            mapPut(newMap, newKey, newValue, 0, entry->hasExternalLinkage);
            entry = entry->next;
        }
    }

    return newMap;
}


void freeMap(Map* map, void (*freeKey)(void*), void (*freeValue)(void*)) {
    if (!map) return;

    for (size_t i = 0; i < map->bucket_count; i++) {
        MapEntry* entry = map->buckets[i];
        while (entry) {
            MapEntry* next = entry->next;
            if (freeKey) freeKey(entry->key);
            if (freeValue) freeValue(entry->value);
            free(entry);
            entry = next;
        }
    }
    free(map->buckets);
    free(map);
}

int mapPut(Map* map, void* key, void* value, int isInScope, int hasExternalLinkage) {
    if (!map) return 0;

    size_t hash = map->hashFunc(key) % map->bucket_count;
    MapEntry* entry = map->buckets[hash];
    while (entry) {
        if (map->equalsFunc(entry->key, key)) {
            entry->value = value; 
            entry->isInScope = isInScope;
            entry->hasExternalLinkage = hasExternalLinkage;
            return 1;
        }
        entry = entry->next;
    }

    MapEntry* new_entry = malloc(sizeof(MapEntry));
    if (!new_entry) return 0;
    new_entry->key = key;
    new_entry->value = value;
    new_entry->isInScope = isInScope;
    new_entry->hasExternalLinkage = hasExternalLinkage;
    new_entry->next = map->buckets[hash];
    map->buckets[hash] = new_entry;

    return 1;
}

MapEntry* mapGetEntry(Map* map, void* key) {
    if (!map) return NULL;

    size_t hash = map->hashFunc(key) % map->bucket_count;
    MapEntry* entry = map->buckets[hash];
    while (entry) {
        if (map->equalsFunc(entry->key, key)) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

void* mapGet(Map* map, void* key) {
    MapEntry* entry = mapGetEntry(map, key);
    return entry ? entry->value : NULL;
}


int mapContainsKey(Map* map, void* key) {
    if (!map) return 0;

    size_t hash = map->hashFunc(key) % map->bucket_count;
    MapEntry* entry = map->buckets[hash];
    while (entry) {
        if (map->equalsFunc(entry->key, key)) {
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

int mapRemove(Map* map, void* key, void (*freeKey)(void*), void (*freeValue)(void*)) {
    if (!map) return 0;

    size_t hash = map->hashFunc(key) % map->bucket_count;
    MapEntry* entry = map->buckets[hash];
    MapEntry* prev = NULL;
    while (entry) {
        if (map->equalsFunc(entry->key, key)) {
            if (prev) {
                prev->next = entry->next;
            } else {
                map->buckets[hash] = entry->next;
            }
            if (freeKey) freeKey(entry->key);
            if (freeValue) freeValue(entry->value);
            free(entry);
            return 1;
        }
        prev = entry;
        entry = entry->next;
    }
    return 0;
}