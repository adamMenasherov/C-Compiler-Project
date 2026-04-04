#pragma once

#include "DynamicArray.h"

/**
 * @file BlockItemArrayWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store CBlockItem pointers
 * 
 * This provides convenience macros and functions for working with
 * arrays of CBlockItem pointers, maintaining type safety.
 * 
 * Note: This file assumes CBlockItem is already defined by the caller.
 * Include C-ASTExpressionTypes.h before including this file.
 */

// CBlockItem and BlockItemArray types are assumed to be defined elsewhere
// CBlockItem is defined in C-ASTExpressionTypes.h
typedef DynamicArray BlockItemArray;

/**
 * @brief Create a new BlockItemArray
 * @return Pointer to newly allocated BlockItemArray, or NULL on failure
 */
static inline BlockItemArray* BlockItemArray_create(void) {
    BlockItemArray* arr = malloc(sizeof(BlockItemArray));
    if (!arr) return NULL;
    
    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }
    
    return arr;
}

/**
 * @brief Append a block item to the array
 * @param arr Block item array
 * @param item Block item pointer to append
 * @return 1 on success, 0 on failure
 */
static inline int BlockItemArray_append(BlockItemArray* arr, CBlockItem* item) {
    return DArray_append(arr, (void*)item);
}

/**
 * @brief Get block item at index
 * @param arr Block item array
 * @param index Index of block item
 * @return Block item pointer at index, or NULL if out of bounds
 */
static inline CBlockItem* BlockItemArray_get(BlockItemArray* arr, int index) {
    return (CBlockItem*)DArray_get(arr, index);
}

/**
 * @brief Set block item at index
 * @param arr Block item array
 * @param index Index to set
 * @param item Block item pointer to set
 * @return 1 on success, 0 on failure
 */
static inline int BlockItemArray_set(BlockItemArray* arr, int index, CBlockItem* item) {
    return DArray_set(arr, index, (void*)item);
}

/**
 * @brief Get current size of array
 * @param arr Block item array
 * @return Number of block items in array
 */
static inline int BlockItemArray_size(BlockItemArray* arr) {
    return DArray_size(arr);
}

/**
 * @brief Get current capacity of array
 * @param arr Block item array
 * @return Total allocated capacity
 */
static inline int BlockItemArray_capacity(BlockItemArray* arr) {
    return DArray_capacity(arr);
}

/**
 * @brief Reset cursor to beginning
 * @param arr Block item array
 */
static inline void BlockItemArray_resetCursor(BlockItemArray* arr) {
    DArray_resetCursor(arr);
}

/**
 * @brief Get cursor position
 * @param arr Block item array
 * @return Current cursor position
 */
static inline int BlockItemArray_getCursor(BlockItemArray* arr) {
    return DArray_getCursor(arr);
}

/**
 * @brief Set cursor position
 * @param arr Block item array
 * @param pos New cursor position
 */
static inline void BlockItemArray_setCursor(BlockItemArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

/**
 * @brief Advance cursor and return current block item
 * @param arr Block item array
 * @return Block item pointer at cursor position, or NULL if at end
 */
static inline CBlockItem* BlockItemArray_nextAndGet(BlockItemArray* arr) {
    return (CBlockItem*)DArray_nextAndGet(arr);
}

/**
 * @brief Free block item array (but not items themselves)
 * 
 * Note: This does NOT free the block items - you must free them separately.
 * 
 * @param arr Block item array
 */
static inline void BlockItemArray_free(BlockItemArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}
