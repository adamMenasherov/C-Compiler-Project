#include "SemanticVariableMap.h"
#include <string.h>
#include <stdio.h>

static void* copyString(void* value) {
    return strdup((const char*)value);
}

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

SemanticVariableMap* copySemanticVariableMap(SemanticVariableMap* original) {
    return copyMap(original, copyString, copyString);
}

void freeSemanticVariableMap(SemanticVariableMap* map) {
    if (!map) return;
    freeMap(map, free, free);
}

int semanticMapPut(SemanticVariableMap* map, char* key, char* value) {
    return mapPut(map, strdup(key), strdup(value), 1);
}

char* semanticMapGet(SemanticVariableMap* map, char* key) {
    char* val = (char*)mapGet(map, key);
    return val ? strdup(val) : NULL;
}

int semanticMapContainsKey(SemanticVariableMap* map, char* key) {
    return mapContainsKey(map, key);
}

MapEntry* getSemanticMapEntry(SemanticVariableMap* map, char* key) {
    if (!map) return NULL;
    return mapGetEntry(map, key);
}

int isFromCurrentBlock(SemanticVariableMap* map, char* key) {
    if (!map) return 0;
    MapEntry* entry = getSemanticMapEntry(map, key);
    return entry ? entry->isInBlock : 0;
}

int semanticMapRemove(SemanticVariableMap* map, char* key) {
    return mapRemove(map, key, free, free);
}