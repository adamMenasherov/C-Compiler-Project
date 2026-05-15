#pragma once
#include "../DynamicArray.h"

typedef struct CDeclaration CDeclaration;

/**
 * @file CDeclarationWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store CDeclaration pointers
 *
 * Note: This file assumes CDeclaration is already defined by the caller.
 */

typedef DynamicArray CDeclarationArray;

static inline CDeclarationArray* CDeclarationArray_create(void) {
    CDeclarationArray* arr = (CDeclarationArray*)malloc(sizeof(CDeclarationArray));
    if (!arr) return NULL;

    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }

    return arr;
}

static inline int CDeclarationArray_append(CDeclarationArray* arr, CDeclaration* declaration) {
    return DArray_append(arr, (void*)declaration);
}

static inline CDeclaration* CDeclarationArray_get(CDeclarationArray* arr, int index) {
    return (CDeclaration*)DArray_get(arr, index);
}

static inline int CDeclarationArray_set(CDeclarationArray* arr, int index, CDeclaration* declaration) {
    return DArray_set(arr, index, (void*)declaration);
}

static inline int CDeclarationArray_size(CDeclarationArray* arr) {
    return DArray_size(arr);
}

static inline int CDeclarationArray_capacity(CDeclarationArray* arr) {
    return DArray_capacity(arr);
}

static inline void CDeclarationArray_resetCursor(CDeclarationArray* arr) {
    DArray_resetCursor(arr);
}

static inline int CDeclarationArray_getCursor(CDeclarationArray* arr) {
    return DArray_getCursor(arr);
}

static inline void CDeclarationArray_setCursor(CDeclarationArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

static inline CDeclaration* CDeclarationArray_nextAndGet(CDeclarationArray* arr) {
    return (CDeclaration*)DArray_nextAndGet(arr);
}

static inline void CDeclarationArray_free(CDeclarationArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}
