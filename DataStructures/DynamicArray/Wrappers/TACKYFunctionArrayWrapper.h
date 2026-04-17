#pragma once

#include "../DynamicArray.h"
#include <stdlib.h>

typedef struct TACKYFunction TACKYFunction;
void freeTackyFunction(TACKYFunction* func);

/**
 * @file TACKYFunctionArrayWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store TACKYFunction pointers
 */

typedef DynamicArray TACKYFunctionArray;

static inline TACKYFunctionArray* TACKYFunctionArray_create(void) {
    TACKYFunctionArray* arr = (TACKYFunctionArray*)malloc(sizeof(TACKYFunctionArray));
    if (!arr) return NULL;

    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }

    return arr;
}

static inline int TACKYFunctionArray_append(TACKYFunctionArray* arr, TACKYFunction* function) {
    return DArray_append(arr, (void*)function);
}

static inline TACKYFunction* TACKYFunctionArray_get(TACKYFunctionArray* arr, int index) {
    return (TACKYFunction*)DArray_get(arr, index);
}

static inline int TACKYFunctionArray_set(TACKYFunctionArray* arr, int index, TACKYFunction* function) {
    return DArray_set(arr, index, (void*)function);
}

static inline int TACKYFunctionArray_size(TACKYFunctionArray* arr) {
    return DArray_size(arr);
}

static inline int TACKYFunctionArray_capacity(TACKYFunctionArray* arr) {
    return DArray_capacity(arr);
}

static inline void TACKYFunctionArray_resetCursor(TACKYFunctionArray* arr) {
    DArray_resetCursor(arr);
}

static inline int TACKYFunctionArray_getCursor(TACKYFunctionArray* arr) {
    return DArray_getCursor(arr);
}

static inline void TACKYFunctionArray_setCursor(TACKYFunctionArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

static inline TACKYFunction* TACKYFunctionArray_nextAndGet(TACKYFunctionArray* arr) {
    return (TACKYFunction*)DArray_nextAndGet(arr);
}

static inline void TACKYFunctionArray_free(TACKYFunctionArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}

static inline void TACKYFunctionArray_freeWithFunctions(TACKYFunctionArray* arr) {
    if (arr) {
        for (int i = 0; i < DArray_size(arr); i++) {
            TACKYFunction* function = TACKYFunctionArray_get(arr, i);
            if (function) freeTackyFunction(function);
        }
        TACKYFunctionArray_free(arr);
    }
}