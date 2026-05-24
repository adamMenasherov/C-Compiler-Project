#include "LocalVarTypeChecker.h"
#include "../../ExpressionTypeChecker/ExpressionTypeChecker.h"
#include "../../../../../Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeConstructors.h"
#include "../../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

static CFactor* convertInitializerToDeclType(CFactor* expr, specifierType targetType) {
    if (!expr || expr->valueType == targetType) return expr;

    CCast* castNode = C_CreateCast(targetType, expr);
    CFactor* castExpr = C_CreateFactorFromCast(castNode);
    if (!castExpr) {
        fprintf(stderr, "Semantic Error: Failed to create cast for declaration initializer\n");
        exit(1);
    }
    setType(castExpr, targetType);
    return castExpr;
}

static void handleExternLocalVar(CDeclaration* decl, SymbolTable* symbolTable) {
    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP) {
        fprintf(stderr, "Semantic Error: Local external variable '%s' cannot have an initializer\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    if (symbolTableContains(symbolTable, decl->decl.variableDecl.identifier)) {
        IdentifierTypeInfo* existing = symbolTableLookup(symbolTable, decl->decl.variableDecl.identifier);
        if (existing->type == TYPE_FUNCTION) {
            fprintf(stderr, "Semantic Error: Function '%s' redeclared as variable\n",
                decl->decl.variableDecl.identifier);
            exit(1);
        }
        return;
    }
    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, specifierTypeToIdentifierType(decl->decl.variableDecl.varType), NULL, 1,
        createIdentifierAttrs(IDENTIFIER_STATIC_ATTR, 1, createIntInitialValue(INITIAL_NO_VALUE, 0), 0));
}

static initialValue* resolveStaticLocalInitValue(CDeclaration* decl) {
    if (decl->decl.variableDecl.declType == VAR_DECL_WITHOUT_EXP)
        return createInitialValue(STATIC_INIT_INT, INITIAL_WITH_VALUE, 0);

    if (decl->decl.variableDecl.varType &&
        decl->decl.variableDecl.varType == SPEC_INT &&
        decl->decl.variableDecl.exp &&
        decl->decl.variableDecl.exp->type == FACTOR_CONSTANT)
        return createInitialValue(STATIC_INIT_INT, INITIAL_WITH_VALUE, decl->decl.variableDecl.exp->exp.cnst->val);

    fprintf(stderr, "Semantic Error: Non-constant initializer for local static variable '%s'\n",
        fromTempToOrigin(decl->decl.variableDecl.identifier));
    exit(1);
}

static void handleStaticLocalVar(CDeclaration* decl, SymbolTable* symbolTable) {
    initialValue* initValue = resolveStaticLocalInitValue(decl);
    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, specifierTypeToIdentifierType(decl->decl.variableDecl.varType), NULL, 0,
        createIdentifierAttrs(IDENTIFIER_STATIC_ATTR, 0, initValue, 0));
}

static void handleRegularLocalVar(CDeclaration* decl, SymbolTable* symbolTable) {
    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP && decl->decl.variableDecl.exp) {
        CFactor* init = decl->decl.variableDecl.exp;
        typeCheckExpression(init, symbolTable);
        decl->decl.variableDecl.exp = convertInitializerToDeclType(
            init,
            decl->decl.variableDecl.varType
        );
    }
    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, specifierTypeToIdentifierType(decl->decl.variableDecl.varType), NULL, 0,
        createIdentifierAttrs(IDENTIFIER_LOCAL_ATTR, 0, NULL, 0));
}


typedef void (*LocalVarHandler)(CDeclaration*, SymbolTable*);
static const LocalVarHandler localVarHandlers[] = {
    [SPEC_EXTERN] = handleExternLocalVar,
    [SPEC_STATIC] = handleStaticLocalVar,
    [SPEC_NULL] = handleRegularLocalVar,
};

void typeCheckLocalVariableDeclaration(CDeclaration* decl, SymbolTable* symbolTable) {
    if (!decl) return;
    LocalVarHandler handler = localVarHandlers[decl->decl.variableDecl.storageClass];
    if (!handler) return;
    handler(decl, symbolTable);
}
