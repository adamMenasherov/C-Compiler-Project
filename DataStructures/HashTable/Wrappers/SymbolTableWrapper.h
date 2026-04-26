#pragma once

#include "../HashTable.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"

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
            int paramCount;   // Number of parameters for the function
            int isDefined; // Whether the function has a body (is defined) or just declared
        } funcInfo;
    };
} IdentifierTypeInfo;

typedef HashTable SymbolTable;
typedef void (*SymbolTableForEachFn)(IdentifierTypeInfo* entry, void* userData);

SymbolTable* createSymbolTable();
int symbolTableInsert(SymbolTable* table, const char* identifier, IdentifierType type, int paramCount, int isDefined, identifierAttrs* attrs);
IdentifierTypeInfo* symbolTableLookup(SymbolTable* table, const char* identifier);
identifierAttrs* createIdentifierAttrs(identifierAttrsType attrType, int global, initialValue* initValue, int defined);
int symbolTableContains(SymbolTable* table, const char* identifier);
initialValue* createInitialValue(initialValueType type, int intValue);
int symbolTableRemove(SymbolTable* table, const char* identifier);
void freeSymbolTable(SymbolTable* table);
void symbolTablePrint(SymbolTable* table);
int isGlobalFunction(CDeclaration* decl, SymbolTable* symTable);

/* Template iteration utility: applies callback to every entry in every bucket chain. */
void symbolTableForEach(SymbolTable* table, SymbolTableForEachFn callback, void* userData);
