#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C-ASTNodes.h"
#include "C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeConstructors.h"
#include "DataStructures/DynamicArray/Wrappers/ExpressionFactorWrapper.h"
#include "DataStructures/DynamicArray/Wrappers/TokenArrayWrapper.h"


static CFactor* parsePostfixOperators(CFactor* factor, TokenList* tokens) {
    while (checkIncrementDecrement(tokens)) {
        unaryType type = prefixToPostfix(expectUnaryOp(tokens));
        factor = C_CreateFactorFromUnary(C_CreateUnary(type, factor));
    }

    return factor;
}

CFactor* C_parseFactor(TokenList* tokens) {
    if (check(tokens, IDENTIFIER) && lookAheadOne(tokens, OPEN_PAREN)) // Factor is a function call
    {
        char* identifier = expectIdentifier(tokens);
        expect(tokens, OPEN_PAREN);

        CFactor* fact =  parsePostfixOperators(C_CreateFactorFromFunctionCall(
            C_CreateFunctionCall(identifier, parseArgumentList(tokens))), tokens);
        expect(tokens, CLOSE_PAREN);
        return fact;
    }

    if (check(tokens, CONSTANT)) {
        return parsePostfixOperators(C_CreateFactorFromConstant(C_parseConstant(tokens)), tokens);
    }

    if (check(tokens, IDENTIFIER)) {
        char* identifier = expectIdentifier(tokens);
        return parsePostfixOperators(C_CreateFactorFromVar(C_CreateVar(identifier)), tokens);
    }

    else if (checkUnaryOp(tokens)) {
        unaryType type = expectUnaryOp(tokens);
        CFactor* inner_exp = C_parseFactor(tokens);
        return parsePostfixOperators(C_CreateFactorFromUnary(C_CreateUnary(type, inner_exp)), tokens);
    }

    else if (check(tokens, OPEN_PAREN)) {
        expect(tokens, OPEN_PAREN);
        CFactor* inner_exp = C_parseExpression(tokens, 0);
        expect(tokens, CLOSE_PAREN);
        return parsePostfixOperators(inner_exp, tokens);
    }
    else {
        fprintf(stderr, "Expression is invalid\n");
        exit(1);
    } 

    return NULL;
}


CFactor* C_parseExpression(TokenList* tokens, int min_prec) {
    CFactor* left = C_parseFactor(tokens);
    if (!left) return NULL;
    int isBinary = checkBinaryOp(tokens), previous_prec; 
    while (isBinary && (previous_prec = precedence(TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array)))) >= min_prec) {
        if (check(tokens, ONE_EQUAL)) {
            expect(tokens, ONE_EQUAL);
            CFactor* right = C_parseExpression(tokens, previous_prec);
            if (!right) return NULL;
            CAssignment* new_left = C_CreateAssignment(C_CreateCopyOfFactor(left), right);
            left = C_CreateFactorFromAssignment(new_left);
        }
        else if (checkCompoundAssignment(tokens)) {
            binType type = compoundAssignmentToBinType(TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array))->type);
            expectCompoundAssignment(tokens);
            CFactor* right = C_parseExpression(tokens, previous_prec);
            if (!right) return NULL;
            CAssignment* new_left = C_CreateAssignment(C_CreateCopyOfFactor(left), 
                C_CreateFactorFromBinary(C_CreateBinary(type, 
                    C_CreateCopyOfFactor(left), right)));
            left = C_CreateFactorFromAssignment(new_left);
        }
        else if (check(tokens, QUESTION_MARK)) {
            expect(tokens, QUESTION_MARK);
            CFactor* middle = C_parseConditionalMiddle(tokens);
            CFactor* right = C_parseExpression(tokens, previous_prec);
            CConditional* new_left = C_CreateConditional(C_CreateCopyOfFactor(left), middle, right);
            left = C_CreateFactorFromConditional(new_left);
        }
        else {
            binType type = expectBinaryOp(tokens);
            CFactor* right = C_parseExpression(tokens, previous_prec + 1);
            if (!right) return NULL;
            CBinary* new_left = C_CreateBinary(type, C_CreateCopyOfFactor(left), right);
            left = C_CreateFactorFromBinary(new_left);
        }
        
        isBinary = checkBinaryOp(tokens);
    }
    return left;   
}

CFactor* C_parseConditionalMiddle(TokenList* tokens) {
    CFactor* exp = C_parseExpression(tokens, 0);
    expect(tokens, COLON);
    return exp;
}

CConstant* C_parseConstant(TokenList* tokens) {
    int val = expectConstant(tokens);
    return C_CreateConstant(val);
}

CVar* C_parseVar(TokenList* tokens) {
    char* identifier = expectIdentifier(tokens);
    return C_CreateVar(identifier);
}

CStatement* C_parseStatement(TokenList* tokens) {
    if (check(tokens, RETURN_KEYWORD)) {
        return C_CreateStatement(STMT_RETURN, C_parseReturn(tokens));
    }
    if (check(tokens, IF_KEYWORD)) {
        return C_CreateStatement(STMT_IF, C_parseIf(tokens));
    }
    else if (check(tokens, SEMICOLON)) {
        expect(tokens, SEMICOLON);
        return C_CreateStatement(STMT_NULL, NULL);
    }
    else if (checkIsLoopStatement(tokens)) {
        return C_parseLoopStatement(tokens);
    }

    else if (check(tokens, OPEN_BRACE)) {
        expect(tokens, OPEN_BRACE);
        CBlock* blockItemType = C_parseBlock(tokens);
        expect(tokens, CLOSE_BRACE);
        return C_CreateStatement(STMT_COMPOUND, C_CreateCompound(blockItemType));
    }

    else if (checkFactorStart(tokens)) {
        CFactor* exp = C_parseExpression(tokens, 0);
        expect(tokens, SEMICOLON);
        return C_CreateStatement(STMT_EXPRESSION, exp);
    }
    else {
        fprintf(stderr, "Parser Error: Invalid statement\n");
        exit(1);
    }        
}


CStatement* C_parseLoopStatement(TokenList* tokens) {
    statementType type = loopStatementKeywordToStatementType(tokens);
    switch(type) {
        case STMT_BREAK: {
            expect(tokens, BREAK_KEYWORD);
            expect(tokens, SEMICOLON);
            return C_CreateStatement(STMT_BREAK, C_CreateLoopStmt());
        }
        case STMT_CONTINUE: {
            expect(tokens, CONTINUE_KEYWORD);
            expect(tokens, SEMICOLON);
            return C_CreateStatement(STMT_CONTINUE, C_CreateLoopStmt());
        }
        case STMT_WHILE: {
            return C_CreateStatement(STMT_WHILE, C_parseWhile(tokens));
        }
        case STMT_DO_WHILE: {
            return C_CreateStatement(STMT_DO_WHILE, C_parseDoWhile(tokens));
        }
        case STMT_FOR: {
            return C_CreateStatement(STMT_FOR, C_parseFor(tokens));
        }
        default:
            fprintf(stderr, "Invalid loop statement type in C_parseStatement\n");
            exit(1);
    }
}


CBlock* C_parseBlock(TokenList* tokens) {
    CBlock* block = C_CreateBlockEmpty();
    if (!block) return NULL;
    while (!check(tokens, CLOSE_BRACE)) {
        CBlockItem* item = C_parseBlockItem(tokens);
        if (!item) return NULL;
        BlockItemArray_append(block->items, item);
    }
    return block;
}


CIf* C_parseIf(TokenList* tokens) {
    expect(tokens, IF_KEYWORD);
    expect(tokens, OPEN_PAREN);
    CFactor* condition = C_parseExpression(tokens, 0);
    expect(tokens, CLOSE_PAREN);
    CStatement* then_stmt = C_parseStatement(tokens);
    CStatement* else_stmt = NULL;
    if (check(tokens, ELSE_KEYWORD)) {
        expect(tokens, ELSE_KEYWORD);
        else_stmt = C_parseStatement(tokens);
        return C_CreateIf(IF_WITH_ELSE, condition, then_stmt, else_stmt);
    }
    return C_CreateIf(IF_WITHOUT_ELSE, condition, then_stmt, else_stmt);
}

CForInit* C_parseForInit(TokenList* tokens) {
    if (check(tokens, INT_KEYWORD)) {
        expect(tokens, INT_KEYWORD);
        char* identifier = expectIdentifier(tokens);
        if (!identifier){
            fprintf(stderr, "Decleration: Expected identifier and got %s", TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array))->value);
            exit(1);
        }
        specifierType varType, storageClass;
        C_parseTypeAndStorageClass(tokens, &varType, &storageClass);
        CDeclaration* decl = C_parseVarDeclaration(tokens, identifier, varType, storageClass);
        return C_CreateForInit(FOR_INIT_DECL, decl);
    }
    else if (!check(tokens, SEMICOLON)) {
        CFactor* exp = C_parseExpression(tokens, 0);
        expect(tokens, SEMICOLON);
        return C_CreateForInit(FOR_INIT_EXP, exp);
    }
    else {
        expect(tokens, SEMICOLON);
        return C_CreateForInit(FOR_INIT_WITHOUT, NULL);
    }
}

CLoop* C_parseWhile(TokenList* tokens) {
    expect(tokens, WHILE_KEYWORD);
    expect(tokens, OPEN_PAREN);
    CFactor* condition = C_parseExpression(tokens, 0);
    expect(tokens, CLOSE_PAREN);
    CStatement* body = C_parseStatement(tokens);
    return C_CreateLoop(condition, body);
}

CLoop* C_parseDoWhile(TokenList* tokens) {
    expect(tokens, DO_KEYWORD);
    CStatement* body = C_parseStatement(tokens);
    expect(tokens, WHILE_KEYWORD);
    expect(tokens, OPEN_PAREN);
    CFactor* condition = C_parseExpression(tokens, 0);
    expect(tokens, CLOSE_PAREN);
    expect(tokens, SEMICOLON);
    return C_CreateLoop(condition, body);
}

CForLoop* C_parseFor(TokenList* tokens) {
    expect(tokens, FOR_KEYWORD);
    expect(tokens, OPEN_PAREN);
    CForInit* init = C_parseForInit(tokens);
    CFactor* condition = NULL;
    if (!check(tokens, SEMICOLON)) {
        condition = C_parseExpression(tokens, 0);
    }
    expect(tokens, SEMICOLON);
    CFactor* post = NULL;
    if (!check(tokens, CLOSE_PAREN)) {
        post = C_parseExpression(tokens, 0);
    }
    expect(tokens, CLOSE_PAREN);
    CStatement* body = C_parseStatement(tokens);
    return C_CreateForLoop(init, condition, post, body);
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

CBlockItem* C_parseBlockItem(TokenList* tokens) {
    blockItemType type;
    void* data;
    if (checkSpecifier(tokens)) {
        type = BLOCK_ITEM_DECL;
        CDeclaration* decl = C_parseDeclaration(tokens);
        data = decl;
    }    
    else {
        type = BLOCK_ITEM_STMT;
        CStatement* stmt = C_parseStatement(tokens);
        data = stmt;
    }  

    return C_CreateBlockItem(type, data);
}

CReturn* C_parseReturn(TokenList* tokens) {
    expect(tokens, RETURN_KEYWORD);
    CFactor* return_exp = C_parseExpression(tokens, 0);
    if (!return_exp) return NULL;

    expect(tokens, SEMICOLON);
    return C_CreateReturn(return_exp);
}

IdentifierArray* C_parseFuncParameters(TokenList* tokens) {
    int containsParam = 0;
    IdentifierArray* params = IdentifierArray_create();
    while (!check(tokens, CLOSE_PAREN)) {
        if (check(tokens, VOID_KEYWORD)) {
            expect(tokens, VOID_KEYWORD);
            if (!check(tokens, CLOSE_PAREN) || containsParam) {
                fprintf(stderr, "Parser Error: 'void' must be the only parameter if present\n");
                exit(1);
            }
            break;
        }

        expect(tokens, INT_KEYWORD);
        char* identifier = expectIdentifier(tokens);
        IdentifierArray_append(params, identifier);
        if (!check(tokens, CLOSE_PAREN)) {
            expect(tokens, COMMA);
            if (check(tokens, CLOSE_PAREN)) {
                fprintf(stderr, "Parser Error: Trailing comma in parameter list\n");
                exit(1);
            }
        }
        containsParam = 1;
    }
    return params;
}

CDeclaration* C_parseFunction(TokenList* tokens, char* identifier, specifierType funcType, specifierType storageClass) {
    funcDeclType type = FUNC_DEF;
    CBlock* body;
    expect(tokens, OPEN_PAREN);
    IdentifierArray* parameters = C_parseFuncParameters(tokens);
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

CProgram* C_parseProgram(TokenList* tokens) {
    CDeclarationArray* functions = C_parseDeclarations(tokens);
    if (!functions) return NULL;

    if (tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Invalid tokens left\n");
        exit(1);
    }

    return C_CreateProgram(functions);
}

void C_parseTypeAndStorageClass(TokenList* tokens, specifierType* type, specifierType* storageClass) {
    int types[10] = {0}, storageClasses[10] = {0};
    int typesLen = 0, storageClassesLen = 0;

    while (tokensLeft(tokens) && checkSpecifier(tokens)) 
    {
        if (check(tokens, INT_KEYWORD)) {
            expect(tokens, INT_KEYWORD);
            types[typesLen++] = SPEC_INT;
        }
        else if (check(tokens, STATIC_KEYWORD)) {
            expect(tokens, STATIC_KEYWORD);
            storageClasses[storageClassesLen++] = SPEC_STATIC;
        }
        else if (check(tokens, EXTERN_KEYWORD)) {
            expect(tokens, EXTERN_KEYWORD);
            storageClasses[storageClassesLen++] = SPEC_EXTERN;
        }
    }

    if (typesLen != 1) {
        fprintf(stderr, "Parser Error: Expected exactly one type specifier\n");
        exit(1);
    }

    if (storageClassesLen > 1) {
        fprintf(stderr, "Parser Error: Expected at most one storage class specifier\n");
        exit(1);
    }

    *type = types[0];
    *storageClass = (storageClassesLen == 1) ? storageClasses[0] : SPEC_NULL;
}


ExpressionFactorArray* parseArgumentList(TokenList* tokens) {
    ExpressionFactorArray* arguments = ExpressionFactorArray_create();
    if (check(tokens, CLOSE_PAREN)) {
        return arguments;
    }

    while (1) {
        CFactor* exp = C_parseExpression(tokens, 0);
        if (!exp) return NULL;
        ExpressionFactorArray_append(arguments, exp);
        if (check(tokens, CLOSE_PAREN)) {
            break;
        }
        expect(tokens, COMMA);
    }
    return arguments;
}