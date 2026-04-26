#pragma once

#include "../DynamicArray.h"
#include <stdlib.h>

typedef struct TACKYTopLevel TACKYTopLevel;
void freeTackyTopLevel(TACKYTopLevel* topLevel);

/**
 * @file TACKYTopLevelArrayWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store TACKYTopLevel pointers
 */

typedef DynamicArray TACKYTopLevelArray;

static inline TACKYTopLevelArray* TACKYTopLevelArray_create(void) {
    TACKYTopLevelArray* arr = (TACKYTopLevelArray*)malloc(sizeof(TACKYTopLevelArray));
    if (!arr) return NULL;

    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }

    return arr;
}

static inline int TACKYTopLevelArray_append(TACKYTopLevelArray* arr, TACKYTopLevel* topLevel) {
    return DArray_append(arr, (void*)topLevel);
}

static inline TACKYTopLevel* TACKYTopLevelArray_get(TACKYTopLevelArray* arr, int index) {
    return (TACKYTopLevel*)DArray_get(arr, index);
}

static inline int TACKYTopLevelArray_set(TACKYTopLevelArray* arr, int index, TACKYTopLevel* topLevel) {
    return DArray_set(arr, index, (void*)topLevel);
}

static inline int TACKYTopLevelArray_size(TACKYTopLevelArray* arr) {
    return DArray_size(arr);
}

static inline int TACKYTopLevelArray_capacity(TACKYTopLevelArray* arr) {
    return DArray_capacity(arr);
}

static inline void TACKYTopLevelArray_resetCursor(TACKYTopLevelArray* arr) {
    DArray_resetCursor(arr);
}

static inline int TACKYTopLevelArray_getCursor(TACKYTopLevelArray* arr) {
    return DArray_getCursor(arr);
}

static inline void TACKYTopLevelArray_setCursor(TACKYTopLevelArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

static inline TACKYTopLevel* TACKYTopLevelArray_nextAndGet(TACKYTopLevelArray* arr) {
    return (TACKYTopLevel*)DArray_nextAndGet(arr);
}

static inline void TACKYTopLevelArray_free(TACKYTopLevelArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}

static inline void TACKYTopLevelArray_freeWithTopLevels(TACKYTopLevelArray* arr) {
    if (arr) {
        for (int i = 0; i < DArray_size(arr); i++) {
            TACKYTopLevel* topLevel = TACKYTopLevelArray_get(arr, i);
            if (topLevel) freeTackyTopLevel(topLevel);
        }
        TACKYTopLevelArray_free(arr);
    }
}
