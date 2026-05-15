#pragma once

#include "../DynamicArray.h"
#include "../../../Lexer/Tokens/token.h"

/**
 * @file TokenArrayWrapper.h
 * @brief Type-specific wrapper for DynamicArray to store Token pointers
 * 
 * This provides convenience macros and functions for working with
 * arrays of Token pointers, maintaining type safety.
 */

typedef DynamicArray TokenArray;

/**
 * @brief Create a new TokenArray
 * @return Pointer to newly allocated TokenArray, or NULL on failure
 */
static inline TokenArray* TokenArray_create(void) {
    TokenArray* arr = malloc(sizeof(TokenArray));
    if (!arr) return NULL;
    
    if (!DArray_init(arr, 16)) {
        free(arr);
        return NULL;
    }
    
    return arr;
}

/**
 * @brief Append a token to the array
 * @param arr Token array
 * @param token Token pointer to append
 * @return 1 on success, 0 on failure
 */
static inline int TokenArray_append(TokenArray* arr, Token* token) {
    return DArray_append(arr, (void*)token);
}

/**
 * @brief Get token at index
 * @param arr Token array
 * @param index Index of token
 * @return Token pointer at index, or NULL if out of bounds
 */
static inline Token* TokenArray_get(TokenArray* arr, int index) {
    return (Token*)DArray_get(arr, index);
}

/**
 * @brief Set token at index
 * @param arr Token array
 * @param index Index to set
 * @param token Token pointer to set
 * @return 1 on success, 0 on failure
 */
static inline int TokenArray_set(TokenArray* arr, int index, Token* token) {
    return DArray_set(arr, index, (void*)token);
}

/**
 * @brief Get current size of array
 * @param arr Token array
 * @return Number of tokens in array
 */
static inline int TokenArray_size(TokenArray* arr) {
    return DArray_size(arr);
}

/**
 * @brief Get current capacity of array
 * @param arr Token array
 * @return Total allocated capacity
 */
static inline int TokenArray_capacity(TokenArray* arr) {
    return DArray_capacity(arr);
}

/**
 * @brief Reset cursor to beginning
 * @param arr Token array
 */
static inline void TokenArray_resetCursor(TokenArray* arr) {
    DArray_resetCursor(arr);
}

/**
 * @brief Get cursor position
 * @param arr Token array
 * @return Current cursor position
 */
static inline int TokenArray_getCursor(TokenArray* arr) {
    return DArray_getCursor(arr);
}

/**
 * @brief Set cursor position
 * @param arr Token array
 * @param pos New cursor position
 */
static inline void TokenArray_setCursor(TokenArray* arr, int pos) {
    DArray_setCursor(arr, pos);
}

/**
 * @brief Advance cursor and return current token
 * @param arr Token array
 * @return Token pointer at cursor position, or NULL if at end
 */
static inline Token* TokenArray_nextAndGet(TokenArray* arr) {
    return (Token*)DArray_nextAndGet(arr);
}

/**
 * @brief Free token array (but not tokens themselves)
 * 
 * Note: This does NOT free the tokens - you must free them separately
 * or use TokenArray_freeWithTokens() if you want to free all tokens too.
 * 
 * @param arr Token array
 */
static inline void TokenArray_free(TokenArray* arr) {
    if (arr) {
        DArray_free(arr);
        free(arr);
    }
}

/**
 * @brief Free token array and all contained tokens
 * @param arr Token array
 */
static inline void TokenArray_freeWithTokens(TokenArray* arr) {
    if (arr) {
        for (int i = 0; i < DArray_size(arr); i++) {
            Token* tok = TokenArray_get(arr, i);
            if (tok) {
                free(tok->value);
                free(tok);
            }
        }
        TokenArray_free(arr);
    }
}