#include "SemanticVariableMap.h"
#include <string.h>
#include <stdio.h>

static size_t hashString(void* key) {
    char* str = (char*)key;
    size_t hash = 0;
    while (*str) {
        hash = hash * 31 + (unsigned char)(*str++);
    }
    return hash;
}

static int equalStrings(void* lhs, void* rhs) {
    return strcmp((char*)lhs, (char*)rhs) == 0;
}


SemanticVariableMap* createSemanticVariableMap() {
    return createMap(hashString, equalStrings);
}

void freeSemanticVariableMap(SemanticVariableMap* map) {
    if (!map) return;
    freeMap(map, free, free);
}

int semanticMapPut(SemanticVariableMap* map, char* key, char* value) {
    return mapPut(map, strdup(key), strdup(value));
}

char* semanticMapGet(SemanticVariableMap* map, char* key) {
    char* val = (char*)mapGet(map, key);
    return val ? strdup(val) : NULL;
}

int semanticMapContainsKey(SemanticVariableMap* map, char* key) {
    return mapContainsKey(map, key);
}

int semanticMapRemove(SemanticVariableMap* map, char* key) {
    return mapRemove(map, key, free, free);
}