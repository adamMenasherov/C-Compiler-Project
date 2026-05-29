#pragma once

/*
 * Enums shared across C-AST, TACKY, and ASM lowering layers.
 * Keep names stable to avoid churn in parser/type-check/backend code.
 */
typedef enum {
    CONST_INT,
    CONST_LONG,
    CONST_UNSIGNED_INT,
    CONST_UNSIGNED_LONG,
    CONST_FLOATING_POINT,
} constantType;

typedef enum {
    UNARY_COMPLEMENT,
    UNARY_NEGATE,
    UNARY_NOT,
    UNARY_INCREMENT,
    UNARY_DECREMENT,
    UNARY_INCREMENT_PREFIX,
    UNARY_INCREMENT_POSTFIX,
    UNARY_DECREMENT_PREFIX,
    UNARY_DECREMENT_POSTFIX,
    UNARY_NOT_UNARY_OP
} unaryType;

typedef enum {
    BIN_SUBTRACT,
    BIN_ADD,
    BIN_MULTIPLY,
    BIN_DIVIDE,
    BIN_REMAINDER,
    BIN_AND,
    BIN_OR,
    BIN_EQUALS,
    BIN_NOT_EQUALS,
    BIN_LESS_THAN,
    BIN_LESS_EQUAL,
    BIN_GREATER_THAN,
    BIN_GREATER_EQUAL,
    BIN_NOT_BINARY_OP,
    BIN_BITWISE_AND,
    BIN_BITWISE_OR,
    BIN_BITWISE_XOR,
    BIN_LEFT_SHIFT,
    BIN_RIGHT_SHIFT
} binType;
