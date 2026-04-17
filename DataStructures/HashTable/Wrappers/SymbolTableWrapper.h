#pragma once

#include "../HashTable.h"

typedef enum {
    TYPE_INT,
    TYPE_FUNCTION
} IdentifierType;

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
    initialValueType type;
    union {
        int intValue;
    } value;
} initialValue;

typedef struct {
    identifierAttrsType attrType;
    union {
        struct {
            int defined;
            int global;
        } funAttr;
        struct {
            initialValue initValue;
            int global;
        } staticAttr;
    } attrs;
} identifierAttrs;


typedef struct {
    char* identifier;
    IdentifierType type;
    identifierAttrs attrs;
    union {
        struct {
            char* uniqueName; // The unique name assigned to the variable
        } varInfo;
        struct {
            char* uniqueName; // The unique name assigned to the function
            int paramCount;   // Number of parameters for the function
            int isDefined; // Whether the function has a body (is defined) or just declared
        } funcInfo;
    };
} IdentifierTypeInfo;

typedef HashTable SymbolTable;

SymbolTable* createSymbolTable();
int symbolTableInsert(SymbolTable* table, const char* identifier, IdentifierType type, int paramCount, int isDefined);
IdentifierTypeInfo* symbolTableLookup(SymbolTable* table, const char* identifier);
int symbolTableContains(SymbolTable* table, const char* identifier);
int symbolTableRemove(SymbolTable* table, const char* identifier);
void freeSymbolTable(SymbolTable* table);
void symbolTablePrint(SymbolTable* table);
