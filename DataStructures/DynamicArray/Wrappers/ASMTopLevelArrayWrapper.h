#pragma once

#include "../DynamicArray.h"
#include <stdlib.h>

typedef struct ASMTopLevel ASMTopLevel;
void freeASMTopLevel(ASMTopLevel* topLevel);

/**
 * @file ASMTopLevelArrayWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store ASMTopLevel pointers
 */

typedef DynamicArray ASMTopLevelArray;

static inline ASMTopLevelArray* ASMTopLevelArray_create(void) {
    ASMTopLevelArray* arr = (ASMTopLevelArray*)malloc(sizeof(ASMTopLevelArray));
    if (!arr) return NULL;

    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }

    return arr;
}

static inline int ASMTopLevelArray_append(ASMTopLevelArray* arr, ASMTopLevel* topLevel) {
    return DArray_append(arr, (void*)topLevel);
}

static inline ASMTopLevel* ASMTopLevelArray_get(ASMTopLevelArray* arr, int index) {
    return (ASMTopLevel*)DArray_get(arr, index);
}

static inline int ASMTopLevelArray_set(ASMTopLevelArray* arr, int index, ASMTopLevel* topLevel) {
    return DArray_set(arr, index, (void*)topLevel);
}

static inline int ASMTopLevelArray_size(ASMTopLevelArray* arr) {
    return DArray_size(arr);
}

static inline int ASMTopLevelArray_capacity(ASMTopLevelArray* arr) {
    return DArray_capacity(arr);
}

static inline void ASMTopLevelArray_resetCursor(ASMTopLevelArray* arr) {
    DArray_resetCursor(arr);
}

static inline int ASMTopLevelArray_getCursor(ASMTopLevelArray* arr) {
    return DArray_getCursor(arr);
}

static inline void ASMTopLevelArray_setCursor(ASMTopLevelArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

static inline ASMTopLevel* ASMTopLevelArray_nextAndGet(ASMTopLevelArray* arr) {
    return (ASMTopLevel*)DArray_nextAndGet(arr);
}

static inline void ASMTopLevelArray_free(ASMTopLevelArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}

static inline void ASMTopLevelArray_freeWithTopLevels(ASMTopLevelArray* arr) {
    if (arr) {
        for (int i = 0; i < DArray_size(arr); i++) {
            ASMTopLevel* topLevel = ASMTopLevelArray_get(arr, i);
            if (topLevel) freeASMTopLevel(topLevel);
        }
        ASMTopLevelArray_free(arr);
    }
}