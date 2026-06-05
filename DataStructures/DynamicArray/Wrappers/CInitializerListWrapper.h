#pragma once

#include "../DynamicArray.h"

typedef struct CInitializer CInitializer;

/**
 * @file CInitializerListWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store CInitializer pointers
 *
 * Note: This file assumes CInitializer is defined by the caller.
 */

typedef DynamicArray CInitializerList;

static inline CInitializerList* CInitializerList_create(void) {
    CInitializerList* arr = (CInitializerList*)malloc(sizeof(CInitializerList));
    if (!arr) return NULL;

    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }

    return arr;
}

static inline int CInitializerList_append(CInitializerList* arr, CInitializer* initializer) {
    return DArray_append(arr, (void*)initializer);
}

static inline CInitializer* CInitializerList_get(CInitializerList* arr, int index) {
    return (CInitializer*)DArray_get(arr, index);
}

static inline int CInitializerList_set(CInitializerList* arr, int index, CInitializer* initializer) {
    return DArray_set(arr, index, (void*)initializer);
}

static inline int CInitializerList_size(CInitializerList* arr) {
    return DArray_size(arr);
}

static inline int CInitializerList_capacity(CInitializerList* arr) {
    return DArray_capacity(arr);
}

static inline void CInitializerList_resetCursor(CInitializerList* arr) {
    DArray_resetCursor(arr);
}

static inline int CInitializerList_getCursor(CInitializerList* arr) {
    return DArray_getCursor(arr);
}

static inline void CInitializerList_setCursor(CInitializerList* arr, int pos) {
    DArray_setCursor(arr, pos);
}

static inline CInitializer* CInitializerList_nextAndGet(CInitializerList* arr) {
    return (CInitializer*)DArray_nextAndGet(arr);
}

static inline void CInitializerList_free(CInitializerList* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}