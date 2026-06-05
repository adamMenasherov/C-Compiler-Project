#pragma once

typedef enum {
    CTYPE_INT,
    CTYPE_LONG,
    CTYPE_UINT,
    CTYPE_ULONG,
    CTYPE_VOID,
    CTYPE_DOUBLE,
    CTYPE_FUN,
    CTYPE_POINTER,
    CTYPE_ARRAY
} CTypeKind;

typedef struct CType CType;

struct CType {
    CTypeKind kind;
    union {
        struct {
            CType* params[MAX_PARAMS];
            int paramCnt;
            CType* ret;
        } fun;
        struct {
            CType* referenced;
        } pointer;
        struct {
            CType* elementType;
            int size;
        } array;
    };
};

typedef enum {
    STORAGE_CLASS_NONE,
    STORAGE_CLASS_STATIC,
    STORAGE_CLASS_EXTERN
} StorageClass;

typedef struct {
    unsigned int isLong : 1;
    unsigned int isInt : 1;
    unsigned int isDouble : 1;
    unsigned int isSigned : 1;
    unsigned int isUnsigned : 1;
    unsigned int isVoid : 1;
    unsigned int isStatic : 1;
    unsigned int isExtern : 1;
} TypeFlags;

/* Forward declarations of CType constructors — available to any file that sees CType */
CType* C_CreateType(CTypeKind kind);
CType* C_CreatePointerType(CType* referenced);
