#pragma once

#include "../DynamicArray.h"
#include <stdlib.h>

typedef struct ASMFunction ASMFunction;
void freeASMFunction(ASMFunction* func);

/**
 * @file ASMFunctionArrayWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store ASMFunction pointers
 */

typedef DynamicArray ASMFunctionArray;

static inline ASMFunctionArray* ASMFunctionArray_create(void) {
    ASMFunctionArray* arr = (ASMFunctionArray*)malloc(sizeof(ASMFunctionArray));
    if (!arr) return NULL;

    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }

    return arr;
}

static inline int ASMFunctionArray_append(ASMFunctionArray* arr, ASMFunction* function) {
    return DArray_append(arr, (void*)function);
}

static inline ASMFunction* ASMFunctionArray_get(ASMFunctionArray* arr, int index) {
    return (ASMFunction*)DArray_get(arr, index);
}

static inline int ASMFunctionArray_set(ASMFunctionArray* arr, int index, ASMFunction* function) {
    return DArray_set(arr, index, (void*)function);
}

static inline int ASMFunctionArray_size(ASMFunctionArray* arr) {
    return DArray_size(arr);
}

static inline int ASMFunctionArray_capacity(ASMFunctionArray* arr) {
    return DArray_capacity(arr);
}

static inline void ASMFunctionArray_resetCursor(ASMFunctionArray* arr) {
    DArray_resetCursor(arr);
}

static inline int ASMFunctionArray_getCursor(ASMFunctionArray* arr) {
    return DArray_getCursor(arr);
}

static inline void ASMFunctionArray_setCursor(ASMFunctionArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

static inline ASMFunction* ASMFunctionArray_nextAndGet(ASMFunctionArray* arr) {
    return (ASMFunction*)DArray_nextAndGet(arr);
}

static inline void ASMFunctionArray_free(ASMFunctionArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}

static inline void ASMFunctionArray_freeWithFunctions(ASMFunctionArray* arr) {
    if (arr) {
        for (int i = 0; i < DArray_size(arr); i++) {
            ASMFunction* function = ASMFunctionArray_get(arr, i);
            if (function) freeASMFunction(function);
        }
        ASMFunctionArray_free(arr);
    }
}
