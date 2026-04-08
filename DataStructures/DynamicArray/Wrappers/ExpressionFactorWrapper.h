#pragma once
#include "../DynamicArray.h"
#include <stdlib.h>

/**
 * @file ExpressionFactorWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store CFactor pointers
 *
 * Note: This file assumes CFactor is already defined by the caller.
 */

typedef DynamicArray ExpressionFactorArray;

static inline ExpressionFactorArray* ExpressionFactorArray_create(void) {
    ExpressionFactorArray* arr = (ExpressionFactorArray*)malloc(sizeof(ExpressionFactorArray));
    if (!arr) return NULL;

    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }

    return arr;
}

static inline int ExpressionFactorArray_append(ExpressionFactorArray* arr, CFactor* factor) {
    return DArray_append(arr, (void*)factor);
}

static inline CFactor* ExpressionFactorArray_get(ExpressionFactorArray* arr, int index) {
    return (CFactor*)DArray_get(arr, index);
}

static inline int ExpressionFactorArray_set(ExpressionFactorArray* arr, int index, CFactor* factor) {
    return DArray_set(arr, index, (void*)factor);
}

static inline int ExpressionFactorArray_size(ExpressionFactorArray* arr) {
    return DArray_size(arr);
}

static inline int ExpressionFactorArray_capacity(ExpressionFactorArray* arr) {
    return DArray_capacity(arr);
}

static inline void ExpressionFactorArray_resetCursor(ExpressionFactorArray* arr) {
    DArray_resetCursor(arr);
}

static inline int ExpressionFactorArray_getCursor(ExpressionFactorArray* arr) {
    return DArray_getCursor(arr);
}

static inline void ExpressionFactorArray_setCursor(ExpressionFactorArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

static inline CFactor* ExpressionFactorArray_nextAndGet(ExpressionFactorArray* arr) {
    return (CFactor*)DArray_nextAndGet(arr);
}

static inline void ExpressionFactorArray_free(ExpressionFactorArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}
