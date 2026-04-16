#pragma once

#include "../HashTable.h"

typedef enum {
    TYPE_INT,
    TYPE_FUNCTION
} IdentifierType;

typedef struct {
    char* identifier;
    IdentifierType type;
    union {
        struct {
            char* uniqueName; // The unique name assigned to the variable
        } varInfo;
        struct {
            char* uniqueName; // The unique name assigned to the function
            int paramCount;   // Number of parameters for the function
            int isDefined;
        } funcInfo;
    };
} IdentifierTypeInfo;

typedef HashTable IdentifierToTypeTable;

IdentifierToTypeTable* createIdentifierToTypeTable();
int identifierToTypeTableInsert(IdentifierToTypeTable* table, const char* identifier, IdentifierType type, int paramCount, int isDefined);
IdentifierTypeInfo* identifierToTypeTableLookup(IdentifierToTypeTable* table, const char* identifier);
int identifierToTypeTableContains(IdentifierToTypeTable* table, const char* identifier);
int identifierToTypeTableRemove(IdentifierToTypeTable* table, const char* identifier);
void freeIdentifierToTypeTable(IdentifierToTypeTable* table);
void identifierToTypeTablePrint(IdentifierToTypeTable* table);