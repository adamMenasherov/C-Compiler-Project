#include "SymbolTableWrapper.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTOperatorNames.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodePrinter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* getInitialValueTypeString(initialValue* initValue) {
    if (!initValue) return "null";
    switch (initValue->type) {
        case INITIAL_TENTATIVE:  return "tentative";
        case INITIAL_NO_VALUE:   return "no_value";
        case INITIAL_WITH_VALUE:
            if (initValue->value.size == 0) return "empty";
            if (initValue->value.size == 1) {
                switch (initValue->value.staticInitVal[0]->staticInitType) {
                    case STATIC_INIT_INT:           return "int_value";
                    case STATIC_INIT_LONG:          return "long_value";
                    case STATIC_INIT_UNSIGNED_INT:  return "uint_value";
                    case STATIC_INIT_UNSIGNED_LONG: return "ulong_value";
                    case STATIC_INIT_DOUBLE:        return "double_value";
                    case STATIC_INIT_ZERO_INIT:     return "zero_init";
                    default:                        return "unknown_type";
                }
            }
            return "array_value";
        default:
            return "unknown";
    }
}

static void getInitialValueString(initialValue* initValue, char* dest, size_t destSize) {
    if (!initValue || initValue->type != INITIAL_WITH_VALUE) {
        snprintf(dest, destSize, "n/a");
        return;
    }

    if (initValue->value.size == 0) {
        snprintf(dest, destSize, "[]");
        return;
    }

    if (initValue->value.size == 1) {
        staticInitialVal* v = initValue->value.staticInitVal[0];
        if (v->staticInitType == STATIC_INIT_DOUBLE)
            snprintf(dest, destSize, "%g", v->val.doubleVal);
        else
            snprintf(dest, destSize, "%lu", v->val.intVal);
        return;
    }

    size_t pos = 0;
    pos += snprintf(dest + pos, destSize - pos, "[");
    for (int i = 0; i < initValue->value.size && pos + 2 < destSize; i++) {
        if (i > 0) pos += snprintf(dest + pos, destSize - pos, ", ");
        staticInitialVal* v = initValue->value.staticInitVal[i];
        if (v->staticInitType == STATIC_INIT_DOUBLE)
            pos += snprintf(dest + pos, destSize - pos, "%g", v->val.doubleVal);
        else
            pos += snprintf(dest + pos, destSize - pos, "%lu", v->val.intVal);
    }
    snprintf(dest + pos, destSize - pos, "]");
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
    return existing && existing->type && existing->type->kind == CTYPE_FUN && existing->attrs->global;
}


static int equalIdentifier(void* lhs, void* rhs) {
    IdentifierTypeInfo* left = (IdentifierTypeInfo*)lhs;
    IdentifierTypeInfo* right = (IdentifierTypeInfo*)rhs;

    if (!left || !right) return lhs == rhs;
    if (!left->identifier || !right->identifier) return left->identifier == right->identifier;
    return strcmp(left->identifier, right->identifier) == 0;
}

initialValue* createInitialValue(initialValueType type) {
    initialValue* value = malloc(sizeof(initialValue));
    if (!value) return NULL;
    value->type = type;
    value->value.size = 0;
    return value;
}

staticInitialVal* createStaticInitialVal(initialValueStaticInitType staticInitType, uint64_t intVal, double doubleVal) {
    staticInitialVal* val = malloc(sizeof(staticInitialVal));
    if (!val) return NULL;

    val->staticInitType = staticInitType;
    if (staticInitType == STATIC_INIT_DOUBLE) {
        val->val.doubleVal = doubleVal;
    } else {
        val->val.intVal = intVal;
    }
    return val;
}

static IdentifierTypeInfo* createIdentifierTypeInfo(const char* identifier, CType* type, int isDefined, identifierAttrs* attrs) {
    IdentifierTypeInfo* info = calloc(1, sizeof(IdentifierTypeInfo));
    if (!info) return NULL;

    info->identifier = strdup(identifier);
    if (!info->identifier) {
        free(info);
        return NULL;
    }

    info->attrs = attrs;
    info->type = type;
    if (type && type->kind == CTYPE_FUN) {
        info->funcInfo.uniqueName = NULL;
        info->funcInfo.isDefined = isDefined;
    } else {
        info->varInfo.uniqueName = NULL;
    }
    return info;
}

identifierAttrs* createIdentifierAttrs(identifierAttrsType attrType, int global, initialValue* initValue, int defined) {
    identifierAttrs* attrs = malloc(sizeof(identifierAttrs));
    if (!attrs) return NULL;

    attrs->attrType = attrType;
    attrs->global = global;
    switch (attrType) {
        case IDENTIFIER_FUN_ATTR:
            attrs->attrs.funAttr.defined = defined;
            break;
        case IDENTIFIER_STATIC_ATTR:
            attrs->attrs.staticAttr.initValue = initValue
                ? initValue
                : createInitialValue(INITIAL_NO_VALUE);
            break;
        case IDENTIFIER_LOCAL_ATTR:
            break;
    }
    return attrs;
}



void freeIdentifierTypeInfo(void* data) {
    IdentifierTypeInfo* info = (IdentifierTypeInfo*)data;
    if (!info) return;
    free(info->identifier);
    if (info->type && info->type->kind == CTYPE_FUN) {
        free(info->funcInfo.uniqueName);
    } else {
        free(info->varInfo.uniqueName);
    }
    free(info);
}

void printIdentifierTypeInfo(void* data) {
    char initVal[1024];
    IdentifierTypeInfo* info = (IdentifierTypeInfo*)data;
    if (!info) return;
    char typeBuf[256];
    if (info->type && info->type->kind == CTYPE_FUN) {
        printf("{ Identifier: %s, Type: Function(%s), UniqueName: %s, IsDefined: %d }\n",
            info->identifier,
            info->type->fun.ret ? getCTypeName(info->type->fun.ret, typeBuf, sizeof(typeBuf)) : "null",
            info->funcInfo.uniqueName ? info->funcInfo.uniqueName : "null",
            info->funcInfo.isDefined);
    } else {
        initialValue* init = (info->attrs && info->attrs->attrType == IDENTIFIER_STATIC_ATTR)
            ? info->attrs->attrs.staticAttr.initValue
            : NULL;
        getInitialValueString(init, initVal, sizeof(initVal));
        printf("{ Identifier: %s, Type: %s, UniqueName: %s, initValType: %s, initVal: %s }\n",
            info->identifier,
            info->type ? getCTypeName(info->type, typeBuf, sizeof(typeBuf)) : "null",
            info->varInfo.uniqueName ? info->varInfo.uniqueName : "null",
            getInitialValueTypeString(init),
            initVal);
    }
}

SymbolTable* createSymbolTable() {
    return createHashTable(hashIdentifier, equalIdentifier);
}

int symbolTableInsert(SymbolTable* table, const char* identifier, CType* type, int isDefined, identifierAttrs* attrs) {
    IdentifierTypeInfo probe;
    IdentifierTypeInfo* existing;
    IdentifierTypeInfo* stored;

    if (!table || !identifier) return 0;

    probe.identifier = (char*)identifier;
    existing = (IdentifierTypeInfo*)ht_getKey(table, &probe);
    if (existing) {
        existing->attrs = attrs;
        if (existing->type && existing->type->kind == CTYPE_FUN) {
            free(existing->funcInfo.uniqueName);
            existing->funcInfo.uniqueName = NULL;
        } else {
            free(existing->varInfo.uniqueName);
            existing->varInfo.uniqueName = NULL;
        }

        existing->type = type;
        if (type && type->kind == CTYPE_FUN) {
            existing->funcInfo.uniqueName = NULL;
            existing->funcInfo.isDefined = isDefined;
        } else {
            existing->varInfo.uniqueName = NULL;
        }
        return 1;
    }

    stored = createIdentifierTypeInfo(identifier, type, isDefined, attrs);
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
