#pragma once

#include "../map.h"

typedef Map CharIntMap;

CharIntMap* createCharIntMap();
int charIntMapPut(CharIntMap* map, const char* key, int value);
int charIntMapGet(CharIntMap* map, const char* key, int* outValue);
int charIntMapContainsKey(CharIntMap* map, const char* key);
int charIntMapRemove(CharIntMap* map, const char* key);
void freeCharIntMap(CharIntMap* map);
void charIntMapPrint(CharIntMap* map);