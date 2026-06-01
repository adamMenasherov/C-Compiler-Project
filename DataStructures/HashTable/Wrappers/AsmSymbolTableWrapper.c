#include "AsmSymbolTableWrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* getAsmTypeString(ASMType type) {
    switch (type) {
        case ASM_LONGWORD: return "longword";
        case ASM_QUADWORD: return "quadword";
        default:           return "unknown";
    }
}

static size_t hashAsmEntry(void* key) {
    ASMSymTabEntry* entry = (ASMSymTabEntry*)key;
    char* str;
    size_t hash = 0;

    if (!entry || !entry->identifier) return 0;

    str = entry->identifier;
    while (*str) {
        hash = hash * 31 + (unsigned char)(*str++);
    }

    return hash;
}

static int equalAsmEntry(void* lhs, void* rhs) {
    ASMSymTabEntry* left  = (ASMSymTabEntry*)lhs;
    ASMSymTabEntry* right = (ASMSymTabEntry*)rhs;

    if (!left || !right) return lhs == rhs;
    if (!left->identifier || !right->identifier) return left->identifier == right->identifier;
    return strcmp(left->identifier, right->identifier) == 0;
}

static ASMSymTabEntry* createAsmSymTabEntry(const char* identifier, ASMSymTabEntryType entryType, ASMType assemblyType, int isStatic, int isConstant, int defined) {
    ASMSymTabEntry* entry = calloc(1, sizeof(ASMSymTabEntry));
    if (!entry) return NULL;

    entry->identifier = strdup(identifier);
    if (!entry->identifier) {
        free(entry);
        return NULL;
    }

    entry->entryType = entryType;
    switch (entryType) {
        case ASM_SYMTAB_OBJ_ENTRY:
            entry->objEntry.assemblyType = assemblyType;
            entry->objEntry.isStatic = isStatic;
            entry->objEntry.isConstant = isConstant;
            break;
        case ASM_SYMTAB_FUN_ENTRY:
            entry->funEntry.defined = defined;
            break;
    }
    return entry;
}

void freeAsmSymTabEntry(void* data) {
    ASMSymTabEntry* entry = (ASMSymTabEntry*)data;
    if (!entry) return;
    free(entry->identifier);
    free(entry);
}

static void printAsmSymTabEntry(void* data) {
    ASMSymTabEntry* entry = (ASMSymTabEntry*)data;
    if (!entry) return;
    switch (entry->entryType) {
        case ASM_SYMTAB_OBJ_ENTRY:
            printf("{ identifier: \"%s\", type: OBJ, assemblyType: %s, isStatic: %s }",
                entry->identifier,
                getAsmTypeString(entry->objEntry.assemblyType),
                entry->objEntry.isStatic ? "true" : "false");
            break;
        case ASM_SYMTAB_FUN_ENTRY:
            printf("{ identifier: \"%s\", type: FUN, defined: %s }",
                entry->identifier,
                entry->funEntry.defined ? "true" : "false");
            break;
    }
}

ASMSymbolTable* createAsmSymbolTable() {
    return createHashTable(hashAsmEntry, equalAsmEntry);
}

int asmSymbolTableInsert(ASMSymbolTable* table, const char* identifier, ASMSymTabEntryType entryType, ASMType assemblyType, int isStatic,
     int isConstant, int defined) 
{
    ASMSymTabEntry probe;    // For checking whether the key already exists
    ASMSymTabEntry* existing; // To hold the existing entry if found
    ASMSymTabEntry* stored;   // The new entry to be stored if the key doesn't already exist

    if (!table || !identifier) return 0;

    probe.identifier = (char*)identifier;
    existing = (ASMSymTabEntry*)ht_getKey(table, &probe);
    if (existing) {
        existing->entryType = entryType;
        switch (entryType) {
            case ASM_SYMTAB_OBJ_ENTRY:
                existing->objEntry.assemblyType = assemblyType;
                existing->objEntry.isStatic = isStatic;
                existing->objEntry.isConstant = isConstant;
                return 1;
            case ASM_SYMTAB_FUN_ENTRY:
                existing->funEntry.defined = defined;
                return 1;
        }
    }

    stored = createAsmSymTabEntry(identifier, entryType, assemblyType, isStatic, isConstant, defined);
    if (!stored) return 0;

    if (!ht_insert(table, stored)) {
        freeAsmSymTabEntry(stored);
        return 0;
    }

    return 1;
}

ASMSymTabEntry* asmSymbolTableLookup(ASMSymbolTable* table, const char* identifier) {
    ASMSymTabEntry probe;

    if (!table || !identifier) return NULL;

    probe.identifier = (char*)identifier;
    return (ASMSymTabEntry*)ht_getKey(table, &probe);
}

int asmSymbolTableContains(ASMSymbolTable* table, const char* identifier) {
    return asmSymbolTableLookup(table, identifier) != NULL;
}

int asmSymbolTableRemove(ASMSymbolTable* table, const char* identifier) {
    ASMSymTabEntry probe;

    if (!table || !identifier) return 0;

    probe.identifier = (char*)identifier;
    return ht_delete(table, &probe, freeAsmSymTabEntry);
}

void freeAsmSymbolTable(ASMSymbolTable* table) {
    freeHashTable(table, freeAsmSymTabEntry);
}

void asmSymbolTablePrint(ASMSymbolTable* table) {
    ht_print(table, printAsmSymTabEntry);
}

void asmSymbolTableForEach(ASMSymbolTable* table, AsmSymbolTableForEachFn callback, void* userData) {
    if (!table || !callback) return;

    for (size_t i = 0; i < table->bucket_count; i++) {
        Entry* entry = table->buckets[i];
        while (entry) {
            callback((ASMSymTabEntry*)entry->key, userData);
            entry = entry->next;
        }
    }
}


ASMSymbolTable* convertFrontEndSymTableToASMSymTable(SymbolTable* symTable) {
    if (!symTable) return NULL;
    ASMSymbolTable* asmSymTable = createAsmSymbolTable();
    if (!asmSymTable) return NULL;

    for (size_t i = 0; i < symTable->bucket_count; i++) {
        Entry* entry = symTable->buckets[i];
        while (entry) {
            IdentifierTypeInfo* info = (IdentifierTypeInfo*)entry->key;
            int isStatic = info->attrs && info->attrs->global ? 0 : 1; // Treat global variables as non-static, local variables as static
            int isDefined = info->type == TYPE_FUNCTION ? info->funcInfo.isDefined : 0; // Only functions have a defined/undefined state
            ASMType asmType = convertIdentifierTypeToASMType(info);
            char* identifier = strdup(info->identifier);
            if (info->type == TYPE_FUNCTION) {
                asmSymbolTableInsert(asmSymTable, identifier, ASM_SYMTAB_FUN_ENTRY, 0, 0, 0, isDefined);
            } else {
                int isConstant = info->type == TYPE_DOUBLE && isDoubleLabel(info->identifier);
                asmSymbolTableInsert(asmSymTable, identifier, ASM_SYMTAB_OBJ_ENTRY, asmType, isStatic, isConstant, 0);
            }
            entry = entry->next;
        }
    }

    return asmSymTable;
}