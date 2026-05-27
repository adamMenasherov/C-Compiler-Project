#pragma once

#include "../HashTable.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"

typedef enum {
    TYPE_INT,
    TYPE_LONG,
    TYPE_UNSIGNED_INT,
    TYPE_UNSIGNED_LONG,
    TYPE_FUNCTION
} IdentifierType;

typedef enum {
    STATIC_INIT_INT,
    STATIC_INIT_LONG,
    STATIC_INIT_UNSIGNED_INT,
    STATIC_INIT_UNSIGNED_LONG
} initialValueStaticInitType;

typedef enum {
    IDENTIFIER_FUN_ATTR,
    IDENTIFIER_STATIC_ATTR,
    IDENTIFIER_LOCAL_ATTR
} identifierAttrsType;

typedef enum {
    INITIAL_TENTATIVE,
    INITIAL_WITH_VALUE,
    INITIAL_NO_VALUE
} initialValueType;

typedef struct {
    initialValueStaticInitType staticInitType;
    uint64_t val;
} staticInitialVal;


typedef struct {
    initialValueType type;
    union {
        staticInitialVal staticInitVal;
    } value;
} initialValue;


typedef struct {
    identifierAttrsType attrType;
    int global;
    union {
        struct {
            int defined;
        } funAttr;
        struct {
            initialValue* initValue;
        } staticAttr;
    } attrs;
} identifierAttrs;


typedef struct {
    char* identifier;
    IdentifierType type;
    identifierAttrs* attrs;
    union {
        struct {
            char* uniqueName; // The unique name assigned to the variable
        } varInfo;
        struct {
            char* uniqueName; // The unique name assigned to the function
            CFuncType* funcType; // Full function type metadata
            int isDefined; // Whether the function has a body (is defined) or just declared
        } funcInfo;
    };
} IdentifierTypeInfo;

typedef HashTable SymbolTable;
typedef void (*SymbolTableForEachFn)(IdentifierTypeInfo* entry, void* userData);

SymbolTable* createSymbolTable();
int symbolTableInsert(SymbolTable* table, const char* identifier, IdentifierType type, CFuncType* funcType, int isDefined, identifierAttrs* attrs);
IdentifierTypeInfo* symbolTableLookup(SymbolTable* table, const char* identifier);
identifierAttrs* createIdentifierAttrs(identifierAttrsType attrType, int global, initialValue* initValue, int defined);
int symbolTableContains(SymbolTable* table, const char* identifier);
initialValue* createInitialValue(initialValueStaticInitType staticInitType, initialValueType type, long intValue);
initialValue* createIntInitialValue(initialValueType type, long intValue);
int symbolTableRemove(SymbolTable* table, const char* identifier);
void freeSymbolTable(SymbolTable* table);
void symbolTablePrint(SymbolTable* table);
int isGlobalFunction(CDeclaration* decl, SymbolTable* symTable);
void symbolTableForEach(SymbolTable* table, SymbolTableForEachFn callback, void* userData);
