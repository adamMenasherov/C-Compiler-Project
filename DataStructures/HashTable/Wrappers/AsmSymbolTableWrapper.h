#pragma once

#include "../HashTable.h"
#include "Parser/ASM-Instructions/ASMInstructionsUtilities/ASMInstructionsGeneralUtils.h"

typedef enum {
    ASM_SYMTAB_OBJ_ENTRY,
    ASM_SYMTAB_FUN_ENTRY
} ASMSymTabEntryType;

typedef struct {
    char* identifier;
    ASMSymTabEntryType entryType;
    union {
        struct {
            ASMType assemblyType;
            int isStatic;
        } objEntry;
        struct {
            int defined;
        } funEntry;
    };
} ASMSymTabEntry;

typedef HashTable ASMSymbolTable;
typedef void (*AsmSymbolTableForEachFn)(ASMSymTabEntry* entry, void* userData);

ASMSymbolTable* createAsmSymbolTable();
int asmSymbolTableInsert(ASMSymbolTable* table, const char* identifier, ASMSymTabEntryType entryType, ASMType assemblyType, int isStatic, int defined);
ASMSymTabEntry* asmSymbolTableLookup(ASMSymbolTable* table, const char* identifier);
int asmSymbolTableContains(ASMSymbolTable* table, const char* identifier);
int asmSymbolTableRemove(ASMSymbolTable* table, const char* identifier);
void freeAsmSymbolTable(ASMSymbolTable* table);
void asmSymbolTablePrint(ASMSymbolTable* table);
void asmSymbolTableForEach(ASMSymbolTable* table, AsmSymbolTableForEachFn callback, void* userData);
ASMSymbolTable* convertFrontEndSymTableToASMSymTable(SymbolTable* symTable);
