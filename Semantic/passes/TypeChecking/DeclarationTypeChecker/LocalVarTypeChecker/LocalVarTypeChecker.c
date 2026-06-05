#include "LocalVarTypeChecker.h"
#include "../../ExpressionTypeChecker/ExpressionTypeChecker.h"
#include "../../../../../Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeConstructors.h"
#include "../../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

static void handleExternLocalVar(CDeclaration* decl, SymbolTable* symbolTable) {
    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP) {
        fprintf(stderr, "Semantic Error: Local external variable '%s' cannot have an initializer\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    if (symbolTableContains(symbolTable, decl->decl.variableDecl.identifier)) {
        IdentifierTypeInfo* existing = symbolTableLookup(symbolTable, decl->decl.variableDecl.identifier);
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
        return;
    }
    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, decl->decl.variableDecl.varType, 1,
        createIdentifierAttrs(IDENTIFIER_STATIC_ATTR, 1, createInitialValue(INITIAL_NO_VALUE), 0));
}

static initialValue* resolveStaticLocalInitValue(CDeclaration* decl) {
    initialValueStaticInitType staticInitType = convertSpecTypeToStaticInitType(decl->decl.variableDecl.varType);

    if (decl->decl.variableDecl.declType == VAR_DECL_WITHOUT_EXP) {
        initialValue* iv = createInitialValue(INITIAL_WITH_VALUE);
        iv->value.staticInitVal[iv->value.size++] = createStaticInitialVal(staticInitType, 0, 0.0);
        return iv;
    }

    if (decl->decl.variableDecl.init &&
        decl->decl.variableDecl.init->type == INIT_SINGLE &&
        decl->decl.variableDecl.init->init.singleInit->type == FACTOR_CONSTANT) {
        CConstant* cnst = decl->decl.variableDecl.init->init.singleInit->exp.cnst;
        initialValue* iv = createInitialValue(INITIAL_WITH_VALUE);
        if (cnst->type == CONST_FLOATING_POINT) {
            iv->value.staticInitVal[iv->value.size++] = createStaticInitialVal(STATIC_INIT_DOUBLE, 0, cnst->value.doubleValue);
        } else {
            uint64_t val = cnst->value.intValue;
            convertValFromType(&val, decl->decl.variableDecl.varType);
            iv->value.staticInitVal[iv->value.size++] = createStaticInitialVal(staticInitType, val, 0.0);
        }
        return iv;
    }

    fprintf(stderr, "Semantic Error: Non-constant initializer for local static variable '%s'\n",
        fromTempToOrigin(decl->decl.variableDecl.identifier));
    exit(1);
}

static void handleStaticLocalVar(CDeclaration* decl, SymbolTable* symbolTable) {
    initialValue* initValue = resolveStaticLocalInitValue(decl);
    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, decl->decl.variableDecl.varType, 0,
        createIdentifierAttrs(IDENTIFIER_STATIC_ATTR, 0, initValue, 0));
}

static void handleRegularLocalVar(CDeclaration* decl, SymbolTable* symbolTable) {
    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, decl->decl.variableDecl.varType, 0,
        createIdentifierAttrs(IDENTIFIER_LOCAL_ATTR, 0, NULL, 0));

    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP && decl->decl.variableDecl.init) {
        typeCheckInit(decl->decl.variableDecl.varType, decl->decl.variableDecl.init, symbolTable);
    }
}

typedef void (*LocalVarHandler)(CDeclaration*, SymbolTable*);
static const LocalVarHandler localVarHandlers[] = {
    [STORAGE_CLASS_EXTERN] = handleExternLocalVar,
    [STORAGE_CLASS_STATIC] = handleStaticLocalVar,
    [STORAGE_CLASS_NONE]   = handleRegularLocalVar,
};

void typeCheckLocalVariableDeclaration(CDeclaration* decl, SymbolTable* symbolTable) {
    if (!decl) return;
    LocalVarHandler handler = localVarHandlers[decl->decl.variableDecl.storageClass];
    if (!handler) return;
    handler(decl, symbolTable);
}
