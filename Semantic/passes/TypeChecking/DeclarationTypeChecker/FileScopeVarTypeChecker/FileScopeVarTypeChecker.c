#include "FileScopeVarTypeChecker.h"
#include "../../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

static initialValueStaticInitType getDeclStaticInitType(CDeclaration* decl) {
    CType* t = decl->decl.variableDecl.varType;
    if (!t) { fprintf(stderr, "Semantic Error: NULL type for variable '%s'\n", decl->decl.variableDecl.identifier); exit(1); }
    while (t->kind == CTYPE_ARRAY) t = t->array.elementType;
    switch (t->kind) {
        case CTYPE_INT:     return STATIC_INIT_INT;
        case CTYPE_LONG:    return STATIC_INIT_LONG;
        case CTYPE_UINT:    return STATIC_INIT_UNSIGNED_INT;
        case CTYPE_ULONG:   return STATIC_INIT_UNSIGNED_LONG;
        case CTYPE_POINTER: return STATIC_INIT_UNSIGNED_LONG;
        case CTYPE_DOUBLE:  return STATIC_INIT_DOUBLE;
        default:
            fprintf(stderr, "Semantic Error: Unsupported file-scope declaration type for variable '%s'\n",
                decl->decl.variableDecl.identifier);
            exit(1);
    }
}

static void validateFileScopeType(CDeclaration* decl, CInitializer* init) {
    if (!init) return;
    if (init->type == INIT_SINGLE) {
        if (init->init.singleInit->type != FACTOR_CONSTANT) {
            fprintf(stderr, "Semantic Error: Only constant expressions are allowed for file-scope variable '%s'\n",
                decl->decl.variableDecl.identifier);
            exit(1);
        }
        return;
    }
    if (init->type == INIT_COMPOUND) {
        for (size_t i = 0; i < (size_t)init->init.compoundInit.initializers->size; i++) {
            validateFileScopeType(decl, init->init.compoundInit.initializers->data[i]);
        }
    }
}

static size_t countScalarElements(CType* t) {
    if (!t || t->kind != CTYPE_ARRAY) return 1;
    return (size_t)t->array.size * countScalarElements(t->array.elementType);
}

static void getInitValue(CInitializer* init, CType* targetType, initialValueStaticInitType staticInitType, initialValue* out) {
    if (init->type == INIT_COMPOUND) {
        CType* elemType = targetType->array.elementType;
        size_t elemScalars = countScalarElements(elemType);
        for (size_t i = 0; i < (size_t)init->init.compoundInit.initializers->size; i++) {
            int before = out->value.size;
            getInitValue(init->init.compoundInit.initializers->data[i], elemType, staticInitType, out);
            while ((size_t)(out->value.size - before) < elemScalars)
                out->value.staticInitVal[out->value.size++] = createStaticInitialVal(staticInitType, 0, 0.0);
        }
        /* zero-pad un-provided elements at this array level */
        for (size_t i = (size_t)init->init.compoundInit.initializers->size; i < (size_t)targetType->array.size; i++)
            for (size_t k = 0; k < elemScalars; k++)
                out->value.staticInitVal[out->value.size++] = createStaticInitialVal(staticInitType, 0, 0.0);
        return;
    }
    if (staticInitType == STATIC_INIT_DOUBLE) {
        CConstant* cnst = init->init.singleInit->exp.cnst;
        double val;
        if (cnst->type == CONST_FLOATING_POINT)
            val = cnst->value.doubleValue;
        else if (cnst->type == CONST_UNSIGNED_LONG)
            val = (double)(uint64_t)cnst->value.intValue;
        else if (cnst->type == CONST_UNSIGNED_INT)
            val = (double)(uint32_t)cnst->value.intValue;
        else
            val = (double)(int64_t)cnst->value.intValue;
        out->value.staticInitVal[out->value.size++] = createStaticInitialVal(STATIC_INIT_DOUBLE, 0, val);
    } else {
        uint64_t val = init->init.singleInit->exp.cnst->value.intValue;
        switch (staticInitType) {
            case STATIC_INIT_INT:           val = (uint64_t)(int32_t)val;  break;
            case STATIC_INIT_UNSIGNED_INT:  val = (uint64_t)(uint32_t)val; break;
            case STATIC_INIT_LONG:          val = (uint64_t)(int64_t)val;  break;
            default: break;
        }
        out->value.staticInitVal[out->value.size++] = createStaticInitialVal(staticInitType, val, 0.0);
    }
}

static initialValue* resolveFileScopeInitValue(CDeclaration* decl) {
    initialValueStaticInitType staticInitType = getDeclStaticInitType(decl);

    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP) {
        initialValue* initValue = createInitialValue(INITIAL_WITH_VALUE);
        getInitValue(decl->decl.variableDecl.init, decl->decl.variableDecl.varType, staticInitType, initValue);
        return initValue;
    }

    if (decl->decl.variableDecl.storageClass == STORAGE_CLASS_EXTERN)
        return createInitialValue(INITIAL_NO_VALUE);
    return createInitialValue(INITIAL_TENTATIVE);
}

static int resolveFileScopeLinkage(CDeclaration* decl, IdentifierTypeInfo* existing, int requestedGlobal) {
    if (!existing) return requestedGlobal;
    if (existing->type && existing->type->kind == CTYPE_FUN) {
        fprintf(stderr, "Semantic Error: Function '%s' redeclared as variable\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    if (!ctypeEqual(existing->type, decl->decl.variableDecl.varType)) {
        fprintf(stderr, "Semantic Error: Conflicting variable types for variable '%s'\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    if (decl->decl.variableDecl.storageClass == STORAGE_CLASS_EXTERN)
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
    validateFileScopeType(decl, decl->decl.variableDecl.init);

    initialValue* initValue = resolveFileScopeInitValue(decl);
    int global = decl->decl.variableDecl.storageClass == STORAGE_CLASS_STATIC ? 0 : 1;
    IdentifierTypeInfo* existing = symbolTableLookup(symbolTable, decl->decl.variableDecl.identifier);

    global = resolveFileScopeLinkage(decl, existing, global);
    initValue = resolveFileScopeExistingInit(decl, existing, initValue);

    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, decl->decl.variableDecl.varType, global,
        createIdentifierAttrs(IDENTIFIER_STATIC_ATTR, global, initValue, 0));
}
