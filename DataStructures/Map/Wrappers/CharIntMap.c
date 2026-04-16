#include "CharIntMap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t hashString(void* key) {
    char* str = (char*)key;
    size_t hash = 0;

    if (!str) return 0;

    while (*str) {
        hash = hash * 31 + (unsigned char)(*str++);
    }

    return hash;
}

static int equalStrings(void* lhs, void* rhs) {
    if (!lhs || !rhs) return lhs == rhs;
    return strcmp((char*)lhs, (char*)rhs) == 0;
}

CharIntMap* createCharIntMap() {
    return createMap(hashString, equalStrings);
}

int charIntMapPut(CharIntMap* map, const char* key, int value) {
    MapEntry* entry;
    char* storedKey;
    int* storedValue;

    if (!map || !key) return 0;

    entry = mapGetEntry(map, (void*)key);
    if (entry) {
        if (!entry->value) {
            entry->value = malloc(sizeof(int));
            if (!entry->value) return 0;
        }
        *(int*)entry->value = value;
        return 1;
    }

    storedKey = strdup(key);
    storedValue = malloc(sizeof(int));
    if (!storedKey || !storedValue) {
        free(storedKey);
        free(storedValue);
        return 0;
    }

    *storedValue = value;
    if (!mapPut(map, storedKey, storedValue, 0, 0)) {
        free(storedKey);
        free(storedValue);
        return 0;
    }

    return 1;
}

int charIntMapGet(CharIntMap* map, const char* key, int* outValue) {
    int* value;

    if (!map || !key || !outValue) return 0;

    value = (int*)mapGet(map, (void*)key);
    if (!value) return 0;

    *outValue = *value;
    return 1;
}

int charIntMapContainsKey(CharIntMap* map, const char* key) {
    if (!map || !key) return 0;
    return mapContainsKey(map, (void*)key);
}

int charIntMapRemove(CharIntMap* map, const char* key) {
    if (!map || !key) return 0;
    return mapRemove(map, (void*)key, free, free);
}

void freeCharIntMap(CharIntMap* map) {
    freeMap(map, free, free);
}

void charIntMapPrint(CharIntMap* map) {
    size_t i;

    if (!map) return;

    printf("{\n");
    for (i = 0; i < map->bucket_count; i++) {
        MapEntry* entry = map->buckets[i];
        while (entry) {
            printf("  \"%s\": %d\n", (char*)entry->key, *(int*)entry->value);
            entry = entry->next;
        }
    }
    printf("}\n");
}