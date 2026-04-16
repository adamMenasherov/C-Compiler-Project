#include "SemanticIdentifierMap.h"
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


SemanticIdentifierMap* createSemanticIdentifierMap() {
    return createMap(hashString, equalStrings);
}

SemanticIdentifierMap* copySemanticIdentifierMap(SemanticIdentifierMap* original) {
    return copyMap(original, copyString, copyString);
}

void freeSemanticIdentifierMap(SemanticIdentifierMap* map) {
    if (!map) return;
    freeMap(map, free, free);
}

int semanticMapPut(SemanticIdentifierMap* map, char* key, char* value, int isInScope, int hasExternalLinkage) {
    return mapPut(map, strdup(key), strdup(value), isInScope, hasExternalLinkage);
}

char* semanticMapGet(SemanticIdentifierMap* map, char* key) {
    char* val = (char*)mapGet(map, key);
    return val ? strdup(val) : NULL;
}

int semanticMapContainsKey(SemanticIdentifierMap* map, char* key) {
    return mapContainsKey(map, key);
}

MapEntry* getSemanticMapEntry(SemanticIdentifierMap* map, char* key) {
    if (!map) return NULL;
    return mapGetEntry(map, key);
}

int isFromCurrentBlock(SemanticIdentifierMap* map, char* key) {
    if (!map) return 0;
    MapEntry* entry = getSemanticMapEntry(map, key);
    return entry ? entry->isInScope : 0;
}

int semanticMapRemove(SemanticIdentifierMap* map, char* key) {
    return mapRemove(map, key, free, free);
}