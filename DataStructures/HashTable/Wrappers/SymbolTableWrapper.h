#pragma once

#include "../HashTable.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"

typedef enum {
    STATIC_INIT_INT,
    STATIC_INIT_LONG,
    STATIC_INIT_UNSIGNED_INT,
    STATIC_INIT_UNSIGNED_LONG,
    STATIC_INIT_DOUBLE
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
    union {
        uint64_t intVal;
        double doubleVal;
    } val;
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
    CType* type;
    identifierAttrs* attrs;
    union {
        struct {
            char* uniqueName;
        } varInfo;
        struct {
            char* uniqueName;
            int isDefined;
        } funcInfo;
    };
} IdentifierTypeInfo;

typedef HashTable SymbolTable;
typedef void (*SymbolTableForEachFn)(IdentifierTypeInfo* entry, void* userData);

SymbolTable* createSymbolTable();
int symbolTableInsert(SymbolTable* table, const char* identifier, CType* type, int isDefined, identifierAttrs* attrs);
IdentifierTypeInfo* symbolTableLookup(SymbolTable* table, const char* identifier);
identifierAttrs* createIdentifierAttrs(identifierAttrsType attrType, int global, initialValue* initValue, int defined);
int symbolTableContains(SymbolTable* table, const char* identifier);
initialValue* createInitialValue(initialValueStaticInitType staticInitType, initialValueType type, uint64_t intValue, double doubleValue);
initialValue* createIntInitialValue(initialValueType type, long intValue);
initialValue* createDoubleInitialValue(initialValueType type, double doubleValue);
int symbolTableRemove(SymbolTable* table, const char* identifier);
void freeSymbolTable(SymbolTable* table);
void symbolTablePrint(SymbolTable* table);
int isGlobalFunction(CDeclaration* decl, SymbolTable* symTable);
void symbolTableForEach(SymbolTable* table, SymbolTableForEachFn callback, void* userData);
