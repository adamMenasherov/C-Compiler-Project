#pragma once

typedef enum {
    SPEC_LONG,
    SPEC_INT,
    SPEC_UNSIGNED_INT,
    SPEC_UNSIGNED_LONG,
    SPEC_DOUBLE,
    SPEC_STATIC,
    SPEC_EXTERN,
    SPEC_POINTER,
    SPEC_NULL
} specifierType;

typedef struct {
    unsigned int isLong     : 1;
    unsigned int isInt      : 1;
    unsigned int isDouble   : 1;
    unsigned int isSigned   : 1;
    unsigned int isUnsigned : 1;
    unsigned int isStatic   : 1;
    unsigned int isExtern   : 1;
} TypeFlags;
