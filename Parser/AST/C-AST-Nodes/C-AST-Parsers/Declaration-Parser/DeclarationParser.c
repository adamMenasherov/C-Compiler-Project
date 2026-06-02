#include "DeclarationParser.h"
#include <stdio.h>
#include <stdlib.h>
#include "../C-ParsersInclude.h"
#include "../../C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodePrinter.h"

#define PARSE_SPECIFIER(token, flag, label) \
    if (check(tokens, token)) { \
        if (flag) { \
            fprintf(stderr, "Parser Error: Duplicate specifier '" label "'\n"); \
            exit(1); \
        } \
        expect(tokens, token); \
        flag = 1; \
    }

static CDeclarator* C_parseDirectDeclarator(TokenList* tokens);
static CDeclarator* C_parseSimpleDeclarator(TokenList* tokens);
static CParamInfo*  C_parseParamList(TokenList* tokens, int* outCount);


CDeclaration* C_parseVarDeclaration(TokenList* tokens, char* identifier, CType* varType, StorageClass storageClass) {
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


CDeclaration* C_parseFunction(TokenList* tokens, char* identifier, CType* funcType,
                              IdentifierArray* parameters, StorageClass storageClass) {
    funcDeclType type = FUNC_DEF;
    CBlock* body;
    if (check(tokens, OPEN_BRACE)) {
        expect(tokens, OPEN_BRACE);
        body = C_parseBlock(tokens);
        expect(tokens, CLOSE_BRACE);
    } else {
        type = FUNC_DECL;
        body = NULL;
        expect(tokens, SEMICOLON);
    }
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
    CType *baseType, *declType;
    StorageClass storageClass;
    char* identifier = NULL;
    IdentifierArray* funcParams = IdentifierArray_create();

    C_parseTypeAndStorageClass(tokens, &baseType, &storageClass);
    CDeclarator* decl = C_parseDeclarator(tokens);          // now includes (params)
    processDeclarator(decl, baseType, &declType, &identifier, funcParams);

    if (declType->kind == CTYPE_FUN) {
        return C_parseFunction(tokens, identifier, declType, funcParams, storageClass);
    } else {
        return C_parseVarDeclaration(tokens, identifier, declType, storageClass);
    }
}

void C_parseTypeAndStorageClass(TokenList* tokens, CType** type, StorageClass* storageClass) {
    TypeFlags typeFound = {0}, storageClassFound = {0};
    unsigned int typeResult = 0;

    while (tokensLeft(tokens) && checkSpecifier(tokens)) {
        PARSE_SPECIFIER(INT_KEYWORD,    typeFound.isInt,          "int")
        else PARSE_SPECIFIER(LONG_KEYWORD,   typeFound.isLong,    "long")
        else PARSE_SPECIFIER(DOUBLE_KEYWORD, typeFound.isDouble,  "double")
        else PARSE_SPECIFIER(STATIC_KEYWORD, storageClassFound.isStatic, "static")
        else PARSE_SPECIFIER(EXTERN_KEYWORD, storageClassFound.isExtern, "extern")
        else PARSE_SPECIFIER(UNSIGNED, typeFound.isUnsigned, "unsigned")
        else PARSE_SPECIFIER(SIGNED, typeFound.isSigned, "signed");
    }
    memcpy(&typeResult, &typeFound, sizeof(TypeFlags));

    if ((storageClassFound.isStatic + storageClassFound.isExtern) > 1) {
        fprintf(stderr, "Parser Error: Expected at most one storage class specifier\n");
        exit(1);
    }
    if (typeFound.isSigned && typeFound.isUnsigned) {
        fprintf(stderr, "Parser Error: Cannot combine 'signed' and 'unsigned' specifiers\n");
        exit(1);
    }

    if (typeFound.isDouble && (typeResult & (typeResult - 1))) {
        fprintf(stderr, "Parser Error: 'double' cannot be combined with other type specifiers\n");
        exit(1);
    }

    *type = C_parseType(typeFound);
    *storageClass = storageClassFound.isStatic ? STORAGE_CLASS_STATIC
                  : storageClassFound.isExtern ? STORAGE_CLASS_EXTERN
                  : STORAGE_CLASS_NONE;
}

CType* C_parseType(TypeFlags typeFound) {
    int signedUnsigned = typeFound.isUnsigned ? 1 : (typeFound.isSigned ? -1 : 0);
    if (typeFound.isDouble) return C_CreateType(CTYPE_DOUBLE);
    if (typeFound.isLong)   return C_CreateType(signedUnsigned == 1 ? CTYPE_ULONG : CTYPE_LONG);
    if (typeFound.isInt)    return C_CreateType(signedUnsigned == 1 ? CTYPE_UINT  : CTYPE_INT);
    /* bare unsigned/signed → int */
    return C_CreateType(signedUnsigned == 1 ? CTYPE_UINT : CTYPE_INT);
}


void processDeclarator(CDeclarator* decl, CType* baseType, CType** declType, char** identifier,
    IdentifierArray* funcParams) 
{
    if (!decl) {
        *declType = C_CreateTypeFromType(baseType);
        return;
    }
    switch (decl->type) {
        case DECLARATOR_IDENT:
            if (identifier) {
                *identifier = decl->decl.identifier;
            }
            *declType = C_CreateTypeFromType(baseType);
            return;
        case DECLARATOR_POINTER:
            baseType = C_CreatePointerType(baseType);
            processDeclarator(decl->decl.pointerDecl.pointed, baseType, declType, identifier, funcParams);
            return;
        case DECLARATOR_FUNC:
            if (decl->decl.func.funcDecl->type != DECLARATOR_IDENT) {
                fprintf(stderr, "Parser Error: Can't apply additional type derivations to a function type\n");
                exit(1);
            }
            for (int i = 0; i < decl->decl.func.paramCnt; i++) {
                CType* param_t;
                char* paramIdentifier;
                processDeclarator(decl->decl.func.params[i].decl, decl->decl.func.params[i].type, &param_t, NULL, NULL);
                if (param_t->kind == CTYPE_FUN) {
                    fprintf(stderr, "Parser Error: Function parameters cannot have function types\n");
                    exit(1);
                }
                paramIdentifier = C_getDeclaratorIdentifier(decl->decl.func.params[i].decl);
                IdentifierArray_append(funcParams, paramIdentifier ? paramIdentifier : "");
                decl->decl.func.params[i].type = param_t;
            }
            *declType = C_CreateFunType(decl->decl.func.params, decl->decl.func.paramCnt, baseType);
            if (identifier) {
                *identifier = C_getDeclaratorIdentifier(decl->decl.func.funcDecl);
            }
            return;
    }
}

CDeclarator* C_parseDeclarator(TokenList* tokens) {
    if (check(tokens, ASTERISK)) {                 
        expect(tokens, ASTERISK);
        return C_CreatePointerDeclarator(C_parseDeclarator(tokens));
    }
    return C_parseDirectDeclarator(tokens);
}

static CDeclarator* C_parseDirectDeclarator(TokenList* tokens) {
    CDeclarator* base = C_parseSimpleDeclarator(tokens);
    if (check(tokens, OPEN_PAREN)) {               
        int count;
        CParamInfo* params = C_parseParamList(tokens, &count);
        return C_CreateFunctionDeclarator(base, params, count);
    }
    return base;
}

static CDeclarator* C_parseSimpleDeclarator(TokenList* tokens) {
    if (check(tokens, OPEN_PAREN)) {               
        expect(tokens, OPEN_PAREN);
        CDeclarator* inner = C_parseDeclarator(tokens);
        expect(tokens, CLOSE_PAREN);
        return inner;                              
    }
    char* identifier = expectIdentifier(tokens);
    return C_CreateIdentDeclarator(identifier);
}

static CParamInfo* C_parseParamList(TokenList* tokens, int* outCount) {
    expect(tokens, OPEN_PAREN);
    int count = 0;
    CParamInfo* params = malloc(MAX_PARAMS * sizeof(CParamInfo));
    if (!params) { fprintf(stderr, "OOM in C_parseParamList\n"); exit(1); }

    while (!check(tokens, CLOSE_PAREN)) {
        CType* paramBaseType; 
        StorageClass sc;
        C_parseTypeAndStorageClass(tokens, &paramBaseType, &sc);
        if (sc != STORAGE_CLASS_NONE) {
            fprintf(stderr, "Parser Error: Function parameters cannot have storage class specifiers\n");
            exit(1);
        }
        CDeclarator* paramDecl = C_parseDeclarator(tokens);
        params[count].type = paramBaseType;
        params[count].decl = paramDecl;
        count++;

        if (check(tokens, COMMA)) {
            expect(tokens, COMMA);
            if (check(tokens, CLOSE_PAREN)) {
                fprintf(stderr, "Parser Error: Trailing comma in parameter list\n");
                exit(1);
            }
        }
    }
    expect(tokens, CLOSE_PAREN);
    *outCount = count;
    return params;
}
