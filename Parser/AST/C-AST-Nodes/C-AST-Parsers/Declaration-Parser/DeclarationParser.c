#include "DeclarationParser.h"
#include <stdio.h>
#include <stdlib.h>
#include "../C-ParsersInclude.h"

#define PARSE_SPECIFIER(token, flag, label) \
    if (check(tokens, token)) { \
        if (flag) { \
            fprintf(stderr, "Parser Error: Duplicate specifier '" label "'\n"); \
            exit(1); \
        } \
        expect(tokens, token); \
        flag = 1; \
    }



IdentifierArray* C_parseFuncParameters(TokenList* tokens, CFuncType* funcType) {
    int containsParam = 0, paramIdx = 0;
    IdentifierArray* params = IdentifierArray_create();
    specifierType paramType, storageClass;
    while (!check(tokens, CLOSE_PAREN)) {
        if (check(tokens, VOID_KEYWORD)) {
            expect(tokens, VOID_KEYWORD);
            if (!check(tokens, CLOSE_PAREN) || containsParam) {
                fprintf(stderr, "Parser Error: 'void' must be the only parameter if present\n");
                exit(1);
            }
            break;
        }

        C_parseTypeAndStorageClass(tokens, &paramType, &storageClass);
        funcType->func.params[paramIdx] = C_CreateType(paramType);
        char* identifier = expectIdentifier(tokens);
        IdentifierArray_append(params, identifier);
        if (!check(tokens, CLOSE_PAREN)) {
            expect(tokens, COMMA);
            if (check(tokens, CLOSE_PAREN)) {
                fprintf(stderr, "Parser Error: Trailing comma in parameter list\n");
                exit(1);
            }
        }
        paramIdx++;
        containsParam = 1;
    }
    funcType->func.paramCnt = paramIdx;
    return params;
}

CDeclaration* C_parseVarDeclaration(TokenList* tokens, char* identifier, specifierType varType, specifierType storageClass) {
    CFactor* fact = NULL;
    varDeclType type = VAR_DECL_WITHOUT_EXP;
    
    if (check(tokens, ONE_EQUAL)) {
        type = VAR_DECL_WITH_EXP;
        expect(tokens, ONE_EQUAL);
        fact = C_parseExpression(tokens, 0);
        expect(tokens, SEMICOLON);
    }
    else {
        expect(tokens, SEMICOLON);
    }
    
    return C_CreateVariableDeclaration(type, identifier, fact, varType, storageClass);
}


CDeclaration* C_parseFunction(TokenList* tokens, char* identifier, specifierType retType, specifierType storageClass) {
    funcDeclType type = FUNC_DEF;
    CBlock* body;
    CFuncType* funcType = C_CreateEmptyType(); 
    expect(tokens, OPEN_PAREN);
    IdentifierArray* parameters = C_parseFuncParameters(tokens, funcType);
    expect(tokens, CLOSE_PAREN);
    if (check(tokens, OPEN_BRACE)) {
        expect(tokens, OPEN_BRACE);
        body = C_parseBlock(tokens);
        expect(tokens, CLOSE_BRACE);
    } else {
        type = FUNC_DECL;
        body = NULL;
        expect(tokens, SEMICOLON);
    }

    funcType->func.ret = C_CreateType(retType);
    return C_CreateFunction(type, identifier, parameters, body, funcType, storageClass);
}

CDeclarationArray* C_parseDeclarations(TokenList* tokens) {
    CDeclarationArray* declArr = CDeclarationArray_create();
    while (checkSpecifier(tokens)) {
        CDeclaration* func = C_parseDeclaration(tokens);
        if (!func) return NULL;
        CDeclarationArray_append(declArr, func);
    }
    return declArr;
}

CDeclaration* C_parseDeclaration(TokenList* tokens) {
    specifierType type, storageClass;
    C_parseTypeAndStorageClass(tokens, &type, &storageClass);

    char* identifier = expectIdentifier(tokens);
    if (!identifier){
        fprintf(stderr, "Decleration: Expected identifier and got %s", TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array))->value);
        exit(1);
    }

    if (check(tokens, OPEN_PAREN)) {
        return C_parseFunction(tokens, identifier, type, storageClass);
    }
    else {
        return C_parseVarDeclaration(tokens, identifier, type, storageClass);
    }
}



void C_parseTypeAndStorageClass(TokenList* tokens, specifierType* type, specifierType* storageClass) {
    TypeFlags typeFound = {0}, storageClassFound = {0};

    while (tokensLeft(tokens) && checkSpecifier(tokens)) {
        PARSE_SPECIFIER(INT_KEYWORD,    typeFound.isInt,          "int")
        else PARSE_SPECIFIER(LONG_KEYWORD,   typeFound.isLong,    "long")
        else PARSE_SPECIFIER(STATIC_KEYWORD, storageClassFound.isStatic, "static")
        else PARSE_SPECIFIER(EXTERN_KEYWORD, storageClassFound.isExtern, "extern")
    }

    if ((storageClassFound.isStatic + storageClassFound.isExtern) > 1) {
        fprintf(stderr, "Parser Error: Expected at most one storage class specifier\n");
        exit(1);
    }

    *type = C_parseType(typeFound);
    *storageClass = storageClassFound.isStatic ? SPEC_STATIC
                  : storageClassFound.isExtern ? SPEC_EXTERN
                  : SPEC_NULL;
}

specifierType C_parseType(TypeFlags typeFound) {
    if (typeFound.isLong) return SPEC_LONG;
    if (typeFound.isInt) return SPEC_INT;
    return SPEC_NULL; // Should never reach here
}