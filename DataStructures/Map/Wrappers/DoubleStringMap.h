#pragma once

#include "../map.h"

/* Maps a double value (by bit-pattern) to a heap-allocated string. */
typedef Map DoubleStringMap;

typedef void (*DoubleStringMapForEachFn)(double key, const char* value, void* userData);

DoubleStringMap* createDoubleStringMap();
/* Returns the stored string (NOT a copy). NULL if not found. */
const char* doubleStringMapGet(DoubleStringMap* map, double key);
/* Stores a copy of value. Overwrites any existing entry for the same key. */
int doubleStringMapPut(DoubleStringMap* map, double key, const char* value);
void doubleStringMapForEach(DoubleStringMap* map, DoubleStringMapForEachFn callback, void* userData);
void freeDoubleStringMap(DoubleStringMap* map);
