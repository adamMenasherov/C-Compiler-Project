#pragma once

#include <stdlib.h>
#include <string.h>

/**
 * @file DynamicArray.h
 * @brief Generic dynamic array implementation (similar to C++ std::vector)
 * 
 * This provides a unified, type-safe generic dynamic array that can hold
 * pointers to any type of element. The implementation uses macro-based generics
 * to provide compile-time type safety while maintaining code reusability.
 * 
 * Features:
 * - Automatic memory management (grows by 2x when capacity exceeded)
 * - Initial capacity of 16 elements
 * - O(1) append operations (amortized)
 * - Optional cursor for iteration
 * - Type-safe macro declarations for each element type
 */

/**
 * @brief Internal generic dynamic array structure
 * 
 * This is the base structure used by all dynamic arrays.
 * Do NOT use this directly - use the macro-based wrappers instead.
 */
typedef struct {
    void** data;          ///< Pointer to array of void pointers
    int capacity;         ///< Total allocated capacity
    int size;             ///< Current number of elements
    int cursor;           ///< Current position for iteration (optional)
} DynamicArray;

/**
 * @brief Initialize a dynamic array
 * 
 * @param arr Pointer to DynamicArray structure
 * @param initial_capacity Initial capacity (typically 16)
 * @return 1 on success, 0 on failure
 */
static inline int DArray_init(DynamicArray* arr, int initial_capacity) {
    if (!arr || initial_capacity <= 0) return 0;
    
    arr->data = malloc(sizeof(void*) * initial_capacity);
    if (!arr->data) return 0;
    
    arr->capacity = initial_capacity;
    arr->size = 0;
    arr->cursor = 0;
    
    return 1;
}

/**
 * @brief Append an element to the array
 * 
 * Automatically reallocates if capacity is exceeded (grows by 2x)
 * 
 * @param arr Pointer to DynamicArray
 * @param element Element to append (cast to void*)
 * @return 1 on success, 0 on failure
 */
static inline int DArray_append(DynamicArray* arr, void* element) {
    if (!arr) return 0;
    
    // Check if we need to resize
    if (arr->size >= arr->capacity) {
        int new_capacity = arr->capacity * 2;
        void** new_data = realloc(arr->data, sizeof(void*) * new_capacity);
        if (!new_data) return 0;
        
        arr->data = new_data;
        arr->capacity = new_capacity;
    }
    
    arr->data[arr->size++] = element;
    return 1;
}

/**
 * @brief Get element at index
 * 
 * @param arr Pointer to DynamicArray
 * @param index Index of element
 * @return Pointer to element at index, NULL if out of bounds
 */
static inline void* DArray_get(DynamicArray* arr, int index) {
    if (!arr || index < 0 || index >= arr->size) return NULL;
    return arr->data[index];
}

/**
 * @brief Set element at index
 * 
 * @param arr Pointer to DynamicArray
 * @param index Index to set
 * @param element Element to set (cast to void*)
 * @return 1 on success, 0 on failure
 */
static inline int DArray_set(DynamicArray* arr, int index, void* element) {
    if (!arr || index < 0 || index >= arr->size) return 0;
    arr->data[index] = element;
    return 1;
}

/**
 * @brief Get current size of array
 * 
 * @param arr Pointer to DynamicArray
 * @return Number of elements in array
 */
static inline int DArray_size(DynamicArray* arr) {
    return arr ? arr->size : 0;
}

/**
 * @brief Get current capacity of array
 * 
 * @param arr Pointer to DynamicArray
 * @return Total allocated capacity
 */
static inline int DArray_capacity(DynamicArray* arr) {
    return arr ? arr->capacity : 0;
}

/**
 * @brief Reset cursor to beginning
 * 
 * @param arr Pointer to DynamicArray
 */
static inline void DArray_resetCursor(DynamicArray* arr) {
    if (arr) arr->cursor = 0;
}

/**
 * @brief Get current cursor position
 * 
 * @param arr Pointer to DynamicArray
 * @return Cursor position
 */
static inline int DArray_getCursor(DynamicArray* arr) {
    return arr ? arr->cursor : 0;
}

/**
 * @brief Set cursor position
 * 
 * @param arr Pointer to DynamicArray
 * @param pos New cursor position
 */
static inline void DArray_setCursor(DynamicArray* arr, int pos) {
    if (arr) arr->cursor = pos;
}

/**
 * @brief Advance cursor and return current element
 * 
 * @param arr Pointer to DynamicArray
 * @return Element at cursor position, or NULL if cursor >= size
 */
static inline void* DArray_nextAndGet(DynamicArray* arr) {
    if (!arr || arr->cursor >= arr->size) return NULL;
    return arr->data[arr->cursor++];
}

/**
 * @brief Free array data (but not the array structure itself)
 * 
 * Note: This does NOT free the elements themselves - the caller is responsible
 * for freeing elements if they are dynamically allocated.
 * 
 * @param arr Pointer to DynamicArray
 */
static inline void DArray_free(DynamicArray* arr) {
    if (arr && arr->data) {
        free(arr->data);
        arr->data = NULL;
        arr->capacity = 0;
        arr->size = 0;
        arr->cursor = 0;
    }
}

/**
 * @brief Iterate over array elements
 * 
 * Helper macro for iterating. Usage:
 *   DArray_forEach(arr, element, {
 *       // Do something with element
 *   })
 * 
 * Note: element is cast as void*, cast to appropriate type in loop body
 */
#define DArray_forEach(arr, element, body) \
    do { \
        DArray_resetCursor(arr); \
        void* element; \
        while ((element = DArray_nextAndGet(arr)) != NULL) { \
            body \
        } \
    } while(0)
