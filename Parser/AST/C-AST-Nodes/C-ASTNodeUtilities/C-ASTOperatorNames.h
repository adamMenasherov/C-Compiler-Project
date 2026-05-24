#pragma once

#include "C-ASTExpressionTypes.h"

static inline const char* getUnaryOpName(unaryType type) {
    switch (type) {
        case UNARY_COMPLEMENT:   return "Complement";
        case UNARY_NEGATE:       return "Negate";
        case UNARY_NOT:          return "Not";
        case UNARY_INCREMENT_PREFIX:  return "IncrementPrefix";
        case UNARY_INCREMENT_POSTFIX: return "IncrementPostfix";
        case UNARY_DECREMENT_PREFIX:  return "DecrementPrefix";
        case UNARY_DECREMENT_POSTFIX: return "DecrementPostfix";
        case UNARY_DECREMENT:  return "Decrement";
        case UNARY_INCREMENT:  return "Increment";
        case UNARY_NOT_UNARY_OP: return "Not";
        default:                 return "Unknown";
    }
}

static inline const char* getBinaryOpName(binType type) {
    switch (type) {
        case BIN_ADD:           return "Add";
        case BIN_SUBTRACT:      return "Subtract";
        case BIN_MULTIPLY:      return "Multiply";
        case BIN_DIVIDE:        return "Divide";
        case BIN_REMAINDER:     return "Modulo";
        case BIN_AND:           return "And";
        case BIN_OR:            return "Or";
        case BIN_EQUALS:        return "Equals";
        case BIN_NOT_EQUALS:    return "NotEquals";
        case BIN_LESS_THAN:     return "LessThan";
        case BIN_LESS_EQUAL:    return "LessEqual";
        case BIN_GREATER_THAN:  return "GreaterThan";
        case BIN_GREATER_EQUAL: return "GreaterEqual";
        case BIN_BITWISE_AND:   return "BitwiseAnd";
        case BIN_BITWISE_OR:    return "BitwiseOr";
        case BIN_BITWISE_XOR:   return "BitwiseXor";
        case BIN_LEFT_SHIFT:    return "LeftShift";
        case BIN_RIGHT_SHIFT:   return "RightShift";
        default:                return "Unknown";
    }
}

static inline const char* getSpecifierTypeName(specifierType type) {
    switch (type) {
        case SPEC_INT: return "int";
        case SPEC_NULL: return "no spec";
        case SPEC_LONG: return "long";
        case SPEC_STATIC: return "static";
        case SPEC_EXTERN: return "extern";
        default: return "Unknown";
    }
}