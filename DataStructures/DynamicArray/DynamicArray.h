#pragma once

#include <stdlib.h>
#include <string.h>


typedef struct {
    void** data;          ///< Pointer to array of void pointers
    int capacity;         ///< Total allocated capacity
    int size;             ///< Current number of elements
    int cursor;           ///< Current position for iteration (optional)
} DynamicArray;


static inline int DArray_init(DynamicArray* arr, int initial_capacity) {
    if (!arr || initial_capacity <= 0) return 0;
    
    arr->data = malloc(sizeof(void*) * initial_capacity);
    if (!arr->data) return 0;
    
    arr->capacity = initial_capacity;
    arr->size = 0;
    arr->cursor = 0;
    
    return 1;
}


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


static inline void* DArray_get(DynamicArray* arr, int index) {
    if (!arr || index < 0 || index >= arr->size) return NULL;
    return arr->data[index];
}


static inline int DArray_set(DynamicArray* arr, int index, void* element) {
    if (!arr || index < 0 || index >= arr->size) return 0;
    arr->data[index] = element;
    return 1;
}


static inline int DArray_size(DynamicArray* arr) {
    return arr ? arr->size : 0;
}


static inline int DArray_capacity(DynamicArray* arr) {
    return arr ? arr->capacity : 0;
}

static inline void DArray_resetCursor(DynamicArray* arr) {
    if (arr) arr->cursor = 0;
}


static inline int DArray_getCursor(DynamicArray* arr) {
    return arr ? arr->cursor : 0;
}


static inline void DArray_setCursor(DynamicArray* arr, int pos) {
    if (arr) arr->cursor = pos;
}


static inline void* DArray_nextAndGet(DynamicArray* arr) {
    if (!arr || arr->cursor >= arr->size) return NULL;
    return arr->data[arr->cursor++];
}


static inline void DArray_free(DynamicArray* arr) {
    if (arr && arr->data) {
        free(arr->data);
        arr->data = NULL;
        arr->capacity = 0;
        arr->size = 0;
        arr->cursor = 0;
    }
}


#define DArray_forEach(arr, element, body) \
    do { \
        DArray_resetCursor(arr); \
        void* element; \
        while ((element = DArray_nextAndGet(arr)) != NULL) { \
            body \
        } \
    } while(0)
