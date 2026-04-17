#pragma once

#include "../DynamicArray.h"

/**
 * @file IdentifierWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store identifier strings (char*)
 */

typedef DynamicArray IdentifierArray;

static inline IdentifierArray* IdentifierArray_create(void) {
    IdentifierArray* arr = (IdentifierArray*)malloc(sizeof(IdentifierArray));
    if (!arr) return NULL;

    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }

    return arr;
}

static inline int IdentifierArray_append(IdentifierArray* arr, char* identifier) {
    return DArray_append(arr, (void*)identifier);
}

static inline char* IdentifierArray_get(IdentifierArray* arr, int index) {
    return (char*)DArray_get(arr, index);
}

static inline int IdentifierArray_set(IdentifierArray* arr, int index, char* identifier) {
    return DArray_set(arr, index, (void*)identifier);
}

static inline int IdentifierArray_size(IdentifierArray* arr) {
    return DArray_size(arr);
}

static inline int IdentifierArray_capacity(IdentifierArray* arr) {
    return DArray_capacity(arr);
}

static inline void IdentifierArray_resetCursor(IdentifierArray* arr) {
    DArray_resetCursor(arr);
}

static inline int IdentifierArray_getCursor(IdentifierArray* arr) {
    return DArray_getCursor(arr);
}

static inline void IdentifierArray_setCursor(IdentifierArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

static inline char* IdentifierArray_nextAndGet(IdentifierArray* arr) {
    return (char*)DArray_nextAndGet(arr);
}

static inline void IdentifierArray_free(IdentifierArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}

static inline IdentifierArray* IdentifierArray_copy(IdentifierArray* arr) {
    if (!arr) return NULL;

    IdentifierArray* copy = IdentifierArray_create();
    if (!copy) return NULL;

    for (int i = 0; i < IdentifierArray_size(arr); i++) {
        char* identifier = IdentifierArray_get(arr, i);
        if (identifier) {
            char* identifierCopy = strdup(identifier);
            if (!identifierCopy) {
                IdentifierArray_free(copy);
                return NULL;
            }
            if (!IdentifierArray_append(copy, identifierCopy)) {
                free(identifierCopy);
                IdentifierArray_free(copy);
                return NULL;
            }
        } else {
            if (!IdentifierArray_append(copy, NULL)) {
                IdentifierArray_free(copy);
                return NULL;
            }
        }
    }

    return copy;
}
