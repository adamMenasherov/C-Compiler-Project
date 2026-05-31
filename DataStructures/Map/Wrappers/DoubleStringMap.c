#include "DoubleStringMap.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/* Reinterpret a double as its raw 64-bit pattern for hashing/equality. */
static uint64_t doubleToBits(double d) {
    uint64_t bits;
    memcpy(&bits, &d, sizeof(bits));
    return bits;
}

static size_t hashDouble(void* key) {
    uint64_t bits;
    memcpy(&bits, key, sizeof(bits));
    /* FNV-1a mix */
    bits ^= bits >> 33;
    bits *= 0xff51afd7ed558ccdULL;
    bits ^= bits >> 33;
    return (size_t)bits;
}

static int equalsDouble(void* lhs, void* rhs) {
    return memcmp(lhs, rhs, sizeof(uint64_t)) == 0;
}

DoubleStringMap* createDoubleStringMap() {
    return createMap(hashDouble, equalsDouble);
}

const char* doubleStringMapGet(DoubleStringMap* map, double key) {
    uint64_t bits = doubleToBits(key);
    return (const char*)mapGet(map, &bits);
}

int doubleStringMapPut(DoubleStringMap* map, double key, const char* value) {
    if (!map || !value) return 0;

    uint64_t bits = doubleToBits(key);
    MapEntry* existing = mapGetEntry(map, &bits);
    if (existing) {
        free(existing->value);
        existing->value = strdup(value);
        return existing->value != NULL;
    }

    uint64_t* keyCopy = malloc(sizeof(uint64_t));
    char*     valCopy = strdup(value);
    if (!keyCopy || !valCopy) {
        free(keyCopy);
        free(valCopy);
        return 0;
    }
    *keyCopy = bits;
    if (!mapPut(map, keyCopy, valCopy, 0, 0)) {
        free(keyCopy);
        free(valCopy);
        return 0;
    }
    return 1;
}

void doubleStringMapForEach(DoubleStringMap* map, DoubleStringMapForEachFn callback, void* userData) {
    if (!map || !callback) return;
    for (size_t i = 0; i < map->bucket_count; i++) {
        MapEntry* entry = map->buckets[i];
        while (entry) {
            uint64_t bits;
            memcpy(&bits, entry->key, sizeof(bits));
            double key;
            memcpy(&key, &bits, sizeof(key));
            callback(key, (const char*)entry->value, userData);
            entry = entry->next;
        }
    }
}

void freeDoubleStringMap(DoubleStringMap* map) {
    freeMap(map, free, free);
}
