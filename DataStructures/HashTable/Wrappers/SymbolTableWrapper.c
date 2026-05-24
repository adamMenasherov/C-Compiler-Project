#include "SymbolTableWrapper.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* getInitialValueString(initialValue* initValue) {
    if (!initValue) return "null";
    switch (initValue->type) {
        case INITIAL_TENTATIVE:
            return "tentative";
        case INITIAL_NO_VALUE:
            return "no_value";
        case INITIAL_WITH_VALUE:
            switch (initValue->value.staticInitVal.staticInitType) {
                case STATIC_INIT_INT:
                    return "int_value";
                case STATIC_INIT_LONG:
                    return "long_value";
                default:
                    return "unknown_static_init_type";
            }
        default:
            return "unknown_initial_value_type";
    }
}


static size_t hashIdentifier(void* key) {
    IdentifierTypeInfo* info = (IdentifierTypeInfo*)key;
    char* str;
    size_t hash = 0;

    if (!info || !info->identifier) return 0;

    str = info->identifier;
    while (*str) {
        hash = hash * 31 + (unsigned char)(*str++);
    }

    return hash;
}

int isGlobalFunction(CDeclaration* decl, SymbolTable* symTable) {
    if (!decl || decl->type != DECL_FUNC) return 0;
    IdentifierTypeInfo* existing = symbolTableLookup(symTable, decl->decl.functionDecl.identifier);
    return existing && existing->type == TYPE_FUNCTION && existing->attrs->global;
}


static int equalIdentifier(void* lhs, void* rhs) {
    IdentifierTypeInfo* left = (IdentifierTypeInfo*)lhs;
    IdentifierTypeInfo* right = (IdentifierTypeInfo*)rhs;

    if (!left || !right) return lhs == rhs;
    if (!left->identifier || !right->identifier) return left->identifier == right->identifier;
    return strcmp(left->identifier, right->identifier) == 0;
}

initialValue* createInitialValue(initialValueStaticInitType staticInitType, initialValueType type, int intValue) {
    initialValue* value = malloc(sizeof(initialValue));
    if (!value) return NULL;

    value->type = type;
    switch (type) {
        case INITIAL_TENTATIVE:
        case INITIAL_NO_VALUE:
            break;
        case INITIAL_WITH_VALUE:
            value->value.staticInitVal.staticInitType = staticInitType;
            value->value.staticInitVal.val = intValue;
            break;
    }
    return value;
}

initialValue* createIntInitialValue(initialValueType type, int intValue) {
    return createInitialValue(STATIC_INIT_INT, type, intValue);
}

static int getFunctionParamCount(const CFuncType* funcType) {
    int count = 0;
    if (!funcType) return 0;
    while (count < MAX_PARAMS && funcType->func.params[count] != NULL) count++;
    return count;
}

static IdentifierTypeInfo* createIdentifierTypeInfo(const char* identifier, IdentifierType type, CFuncType* funcType, int isDefined, identifierAttrs* attrs) {
    IdentifierTypeInfo* info = calloc(1, sizeof(IdentifierTypeInfo));
    if (!info) return NULL;

    info->identifier = strdup(identifier);
    if (!info->identifier) {
        free(info);
        return NULL;
    }

    info->attrs = attrs;
    info->type = type;
    switch (type) {
        case TYPE_INT:
        case TYPE_LONG:
            info->varInfo.uniqueName = NULL;
            break;
        case TYPE_FUNCTION:
            info->funcInfo.uniqueName = NULL;
            info->funcInfo.funcType = funcType;
            info->funcInfo.isDefined = isDefined;
            break;
    }
    return info;
}

identifierAttrs* createIdentifierAttrs(identifierAttrsType attrType, int global, initialValue* initValue, int defined) {
    identifierAttrs* attrs = malloc(sizeof(identifierAttrs));
    if (!attrs) return NULL;

    if (!initValue) attrs->attrs.staticAttr.initValue = createInitialValue(STATIC_INIT_INT, INITIAL_NO_VALUE, 0);
    else attrs->attrs.staticAttr.initValue = initValue;

    attrs->attrType = attrType;
    attrs->global = global;
    switch (attrType) {
        case IDENTIFIER_FUN_ATTR:
            attrs->attrs.funAttr.defined = defined;
            break;
        case IDENTIFIER_STATIC_ATTR:
            attrs->attrs.staticAttr.initValue = initValue;
            break;
        case IDENTIFIER_LOCAL_ATTR:
            break;
    }
    return attrs;
}


static IdentifierTypeInfo* copyIdentifierTypeInfo(const IdentifierTypeInfo* info) {
    IdentifierTypeInfo* copy;

    if (!info || !info->identifier) return NULL;

    copy = calloc(1, sizeof(IdentifierTypeInfo));
    if (!copy) return NULL;

    copy->identifier = strdup(info->identifier);
    if (!copy->identifier) {
        free(copy);
        return NULL;
    }

    copy->type = info->type;
    switch (info->type) {
        case TYPE_INT:
        case TYPE_LONG:
            copy->varInfo.uniqueName = info->varInfo.uniqueName ? strdup(info->varInfo.uniqueName) : NULL;
            if (info->varInfo.uniqueName && !copy->varInfo.uniqueName) {
                free(copy->identifier);
                free(copy);
                return NULL;
            }
            break;
        case TYPE_FUNCTION:
            copy->funcInfo.uniqueName = info->funcInfo.uniqueName ? strdup(info->funcInfo.uniqueName) : NULL;
            if (info->funcInfo.uniqueName && !copy->funcInfo.uniqueName) {
                free(copy->identifier);
                free(copy);
                return NULL;
            }
            copy->funcInfo.funcType = info->funcInfo.funcType;
            copy->funcInfo.isDefined = info->funcInfo.isDefined;
            break;
    }
    return copy;
}

void freeIdentifierTypeInfo(void* data) {
    IdentifierTypeInfo* info = (IdentifierTypeInfo*)data;
    if (!info) return;
    free(info->identifier);
    switch (info->type) {
        case TYPE_INT:
        case TYPE_LONG:
            free(info->varInfo.uniqueName);
            break;
        case TYPE_FUNCTION:
            free(info->funcInfo.uniqueName);
            break;
    }
    free(info);
}

void printIdentifierTypeInfo(void* data) {
    IdentifierTypeInfo* info = (IdentifierTypeInfo*)data;
    if (!info) return;
    switch (info->type) {
        case TYPE_INT:
        printf("{ identifier: \"%s\", type: INT, uniqueName: \"%s\" }, initialValue: %s }",
                info->identifier,
                info->varInfo.uniqueName ? info->varInfo.uniqueName : "",
                getInitialValueString(info->attrs->attrs.staticAttr.initValue));
            break;
        case TYPE_LONG:
            printf("{ identifier: \"%s\", type: LONG, uniqueName: \"%s\", initialValue: %s }",
                info->identifier,
                info->varInfo.uniqueName ? info->varInfo.uniqueName : "", 
             getInitialValueString(info->attrs->attrs.staticAttr.initValue));
            break;
        case TYPE_FUNCTION:
            printf("{ identifier: \"%s\", type: FUNCTION, uniqueName: \"%s\", paramCount: %d, isDefined: %s  }",
                info->identifier,
                info->funcInfo.uniqueName ? info->funcInfo.uniqueName : "",
                getFunctionParamCount(info->funcInfo.funcType),
                info->funcInfo.isDefined ? "true" : "false");
            break;
    }
}

SymbolTable* createSymbolTable() {
    return createHashTable(hashIdentifier, equalIdentifier);
}

int symbolTableInsert(SymbolTable* table, const char* identifier, IdentifierType type, CFuncType* funcType, int isDefined, identifierAttrs* attrs) {
    IdentifierTypeInfo probe; // For checking whether the key already exists
    IdentifierTypeInfo* existing; // To hold the existing entry if found
    IdentifierTypeInfo* stored; // The new entry to be stored if the key doesn't already exist

    if (!table || !identifier) return 0;

    probe.identifier = (char*)identifier;
    existing = (IdentifierTypeInfo*)ht_getKey(table, &probe);
    if (existing) {
        existing->attrs = attrs;
        switch (existing->type) {
            case TYPE_INT:
            case TYPE_LONG:
                free(existing->varInfo.uniqueName);
                break;
            case TYPE_FUNCTION:
                free(existing->funcInfo.uniqueName);
                break;
        }

        existing->type = type;
        switch (type) {
            case TYPE_INT:
            case TYPE_LONG:
                existing->varInfo.uniqueName = NULL;
                return 1;
            case TYPE_FUNCTION:
                existing->funcInfo.uniqueName = NULL;
                existing->funcInfo.funcType = funcType;
                existing->funcInfo.isDefined = isDefined;
                return 1;
        }
    }

    stored = createIdentifierTypeInfo(identifier, type, funcType, isDefined, attrs);
    if (!stored) return 0;

    if (!ht_insert(table, stored)) {
        freeIdentifierTypeInfo(stored);
        return 0;
    }

    return 1;
}

IdentifierTypeInfo* symbolTableLookup(SymbolTable* table, const char* identifier) {
    IdentifierTypeInfo probe;

    if (!table || !identifier) return NULL;

    probe.identifier = (char*)identifier;
    return (IdentifierTypeInfo*)ht_getKey(table, &probe);
}

int symbolTableContains(SymbolTable* table, const char* identifier) {
    return symbolTableLookup(table, identifier) != NULL;
}

int symbolTableRemove(SymbolTable* table, const char* identifier) {
    IdentifierTypeInfo probe;

    if (!table || !identifier) return 0;

    probe.identifier = (char*)identifier;
    return ht_delete(table, &probe, freeIdentifierTypeInfo);
}

void freeSymbolTable(SymbolTable* table) {
    freeHashTable(table, freeIdentifierTypeInfo);
}

void symbolTablePrint(SymbolTable* table) {
    ht_print(table, printIdentifierTypeInfo);
}

void symbolTableForEach(SymbolTable* table, SymbolTableForEachFn callback, void* userData) {
    if (!table || !callback) return;

    for (size_t i = 0; i < table->bucket_count; i++) {
        Entry* entry = table->buckets[i];
        while (entry) {
            callback((IdentifierTypeInfo*)entry->key, userData);
            entry = entry->next;
        }
    }
}
