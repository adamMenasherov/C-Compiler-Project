#pragma once

#include "../DynamicArray.h"
#include <stdlib.h>

typedef struct TACKYValue TACKYValue;
void freeTackyValue(TACKYValue* value);

/**
 * @file TACKYValueArrayWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store TACKYValue pointers
 */

typedef DynamicArray TACKYValueArray;

static inline TACKYValueArray* TACKYValueArray_create(void) {
    TACKYValueArray* arr = malloc(sizeof(TACKYValueArray));
    if (!arr) return NULL;

    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }

    return arr;
}

static inline int TACKYValueArray_append(TACKYValueArray* arr, TACKYValue* value) {
    return DArray_append(arr, (void*)value);
}

static inline TACKYValue* TACKYValueArray_get(TACKYValueArray* arr, int index) {
    return (TACKYValue*)DArray_get(arr, index);
}

static inline int TACKYValueArray_set(TACKYValueArray* arr, int index, TACKYValue* value) {
    return DArray_set(arr, index, (void*)value);
}

static inline int TACKYValueArray_size(TACKYValueArray* arr) {
    return DArray_size(arr);
}

static inline int TACKYValueArray_capacity(TACKYValueArray* arr) {
    return DArray_capacity(arr);
}

static inline void TACKYValueArray_resetCursor(TACKYValueArray* arr) {
    DArray_resetCursor(arr);
}

static inline int TACKYValueArray_getCursor(TACKYValueArray* arr) {
    return DArray_getCursor(arr);
}

static inline void TACKYValueArray_setCursor(TACKYValueArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

static inline TACKYValue* TACKYValueArray_nextAndGet(TACKYValueArray* arr) {
    return (TACKYValue*)DArray_nextAndGet(arr);
}

static inline void TACKYValueArray_free(TACKYValueArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}

static inline void TACKYValueArray_freeWithValues(TACKYValueArray* arr) {
    if (arr) {
        for (int i = 0; i < DArray_size(arr); i++) {
            TACKYValue* value = TACKYValueArray_get(arr, i);
            if (value) freeTackyValue(value);
        }
        TACKYValueArray_free(arr);
    }
}