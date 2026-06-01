#include "FileScopeVarTypeChecker.h"
#include "../../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

static initialValueStaticInitType getDeclStaticInitType(CDeclaration* decl) {
    switch (decl->decl.variableDecl.varType) {
        case SPEC_INT:
            return STATIC_INIT_INT;
        case SPEC_LONG:
            return STATIC_INIT_LONG;
        case SPEC_UNSIGNED_INT:
            return STATIC_INIT_UNSIGNED_INT;
        case SPEC_UNSIGNED_LONG:
            return STATIC_INIT_UNSIGNED_LONG;
        case SPEC_DOUBLE:
            return STATIC_INIT_DOUBLE;
        default:
            fprintf(stderr, "Semantic Error: Unsupported file-scope declaration type for variable '%s'\n",
                decl->decl.variableDecl.identifier);
            exit(1);
    }
}

static void validateFileScopeType(CDeclaration* decl) {
    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP && !isBasicType(decl->decl.variableDecl.exp->valueType)) {
        fprintf(stderr, "Semantic Error: Only constant variables can be initialized at file scope. Variable '%s' has invalid type\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }

    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP && decl->decl.variableDecl.exp->type != FACTOR_CONSTANT) {
        fprintf(stderr, "Semantic Error: Non-constant initializer for file-scope variable '%s'\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
}

static initialValue* resolveFileScopeInitValue(CDeclaration* decl) {
    initialValueStaticInitType staticInitType = getDeclStaticInitType(decl);

    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP) {
        if (staticInitType == STATIC_INIT_DOUBLE) {
            double val = decl->decl.variableDecl.exp->exp.cnst->value.doubleValue;
            return createDoubleInitialValue(INITIAL_WITH_VALUE, val);
        } else {
            uint64_t val = decl->decl.variableDecl.exp->exp.cnst->value.intValue;
            convertValFromType(&val, decl->decl.variableDecl.varType);
            return createInitialValue(staticInitType, INITIAL_WITH_VALUE, val, 0.0);
        }
    }
        
    if (decl->decl.variableDecl.storageClass == SPEC_EXTERN)
        return createInitialValue(staticInitType, INITIAL_NO_VALUE, 0, 0.0);
    return createInitialValue(staticInitType, INITIAL_TENTATIVE, 0, 0.0);
}

static int resolveFileScopeLinkage(CDeclaration* decl, IdentifierTypeInfo* existing, int requestedGlobal) {
    if (!existing) return requestedGlobal;
    if (existing->type == TYPE_FUNCTION) {
        fprintf(stderr, "Semantic Error: Function '%s' redeclared as variable\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    if (existing->type != specifierTypeToIdentifierType(decl->decl.variableDecl.varType)) {
        fprintf(stderr, "Semantic Error: Conflicting variable types for variable '%s'\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    if (decl->decl.variableDecl.storageClass == SPEC_EXTERN)
        return existing->attrs->global;
    if (existing->attrs->global != requestedGlobal) {
        fprintf(stderr, "Semantic Error: Conflicting variable linkage for variable '%s'\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    return requestedGlobal;
}

static initialValue* resolveFileScopeExistingInit(CDeclaration* decl, IdentifierTypeInfo* existing, initialValue* initValue) {
    if (!existing) return initValue;
    if (existing->attrs->attrs.staticAttr.initValue->type != INITIAL_WITH_VALUE) return initValue;
    if (initValue && initValue->type == INITIAL_WITH_VALUE) {
        fprintf(stderr, "Semantic Error: Multiple initializers for variable '%s'\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    return existing->attrs->attrs.staticAttr.initValue;
}

void typeCheckFileScopeVariableDeclaration(CDeclaration* decl, SymbolTable* symbolTable) {
    if (!decl || decl->type != DECL_VAR) return;
    validateFileScopeType(decl);
    IdentifierType type;

    initialValue* initValue = resolveFileScopeInitValue(decl);
    int global = decl->decl.variableDecl.storageClass == SPEC_STATIC ? 0 : 1;
    IdentifierTypeInfo* existing = symbolTableLookup(symbolTable, decl->decl.variableDecl.identifier);

    global = resolveFileScopeLinkage(decl, existing, global);
    initValue = resolveFileScopeExistingInit(decl, existing, initValue);
    type = specifierTypeToIdentifierType(decl->decl.variableDecl.varType);

    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, type, NULL, global,
        createIdentifierAttrs(IDENTIFIER_STATIC_ATTR, global, initValue, 0));
}
