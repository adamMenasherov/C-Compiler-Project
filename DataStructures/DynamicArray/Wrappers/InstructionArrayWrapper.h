#pragma once

#include "../DynamicArray.h"

/**
 * @file InstructionArrayWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store TACKY instruction pointers
 * 
 * This provides convenience macros and functions for working with
 * arrays of TACKYInstruction pointers, maintaining type safety.
 * 
 * Forward declaration is used to avoid circular includes.
 */

typedef struct TACKYInstruction TACKYInstruction;
typedef DynamicArray InstructionArray;

/**
 * @brief Create a new InstructionArray
 * @return Pointer to newly allocated InstructionArray, or NULL on failure
 */
static inline InstructionArray* InstructionArray_create(void) {
    InstructionArray* arr = malloc(sizeof(InstructionArray));
    if (!arr) return NULL;
    
    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }
    
    return arr;
}

/**
 * @brief Append an instruction to the array
 * @param arr Instruction array
 * @param inst Instruction pointer to append
 * @return 1 on success, 0 on failure
 */
static inline int InstructionArray_append(InstructionArray* arr, TACKYInstruction* inst) {
    return DArray_append(arr, (void*)inst);
}

/**
 * @brief Get instruction at index
 * @param arr Instruction array
 * @param index Index of instruction
 * @return Instruction pointer at index, or NULL if out of bounds
 */
static inline TACKYInstruction* InstructionArray_get(InstructionArray* arr, int index) {
    return (TACKYInstruction*)DArray_get(arr, index);
}

/**
 * @brief Set instruction at index
 * @param arr Instruction array
 * @param index Index to set
 * @param inst Instruction pointer to set
 * @return 1 on success, 0 on failure
 */
static inline int InstructionArray_set(InstructionArray* arr, int index, TACKYInstruction* inst) {
    return DArray_set(arr, index, (void*)inst);
}

/**
 * @brief Get current size of array
 * @param arr Instruction array
 * @return Number of instructions in array
 */
static inline int InstructionArray_size(InstructionArray* arr) {
    return DArray_size(arr);
}

/**
 * @brief Get current capacity of array
 * @param arr Instruction array
 * @return Total allocated capacity
 */
static inline int InstructionArray_capacity(InstructionArray* arr) {
    return DArray_capacity(arr);
}

/**
 * @brief Reset cursor to beginning
 * @param arr Instruction array
 */
static inline void InstructionArray_resetCursor(InstructionArray* arr) {
    DArray_resetCursor(arr);
}

/**
 * @brief Get cursor position
 * @param arr Instruction array
 * @return Current cursor position
 */
static inline int InstructionArray_getCursor(InstructionArray* arr) {
    return DArray_getCursor(arr);
}

/**
 * @brief Set cursor position
 * @param arr Instruction array
 * @param pos New cursor position
 */
static inline void InstructionArray_setCursor(InstructionArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

/**
 * @brief Advance cursor and return current instruction
 * @param arr Instruction array
 * @return Instruction pointer at cursor position, or NULL if at end
 */
static inline TACKYInstruction* InstructionArray_nextAndGet(InstructionArray* arr) {
    return (TACKYInstruction*)DArray_nextAndGet(arr);
}

/**
 * @brief Free instruction array (but not instructions themselves)
 * 
 * Note: This does NOT free the instructions - you must free them separately.
 * 
 * @param arr Instruction array
 */
static inline void InstructionArray_free(InstructionArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}