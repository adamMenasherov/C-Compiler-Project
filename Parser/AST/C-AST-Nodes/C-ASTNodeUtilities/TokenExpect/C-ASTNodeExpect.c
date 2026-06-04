#include "C-ASTNodeExpect.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include "../../../../../Lexer/Tokens/token.h"

static int isConstantType(TokenType type) {
    return type == CONSTANT || type == LONG_CONSTANT ||
        type == UNSIGNED_CONSTANT || type == UNSIGNED_LONG_CONSTANT ||
        type == FLOATING_POINT_CONSTANT;
}

static int isSpecifierType(TokenType type) {
    return type == INT_KEYWORD || type == LONG_KEYWORD || type == STATIC_KEYWORD || type == DOUBLE_KEYWORD ||
           type == EXTERN_KEYWORD || type == UNSIGNED || type == SIGNED || type == VOID_KEYWORD;
}

static int getConstantType(TokenType type) {
    switch(type) {
        case CONSTANT: return CONST_INT;
        case LONG_CONSTANT: return CONST_LONG;
        case UNSIGNED_CONSTANT: return CONST_UNSIGNED_INT;
        case UNSIGNED_LONG_CONSTANT: return CONST_UNSIGNED_LONG;
        case FLOATING_POINT_CONSTANT: return CONST_FLOATING_POINT;
        default:
            fprintf(stderr, "Parser Error: Expected constant token but got %s\n",
                    tokenTypeToToken(type));
            exit(1);
    }
}

int tokensLeft(TokenList* tokens) {
    return TokenArray_getCursor(tokens->array) < TokenArray_size(tokens->array);
}

int checkConstant(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array));
    return tok ? isConstantType(tok->type) : 0;
}

int check(TokenList* tokens, TokenType type) {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array));
    return tok ? tok->type == type : 0;
}

int lookAheadOne(TokenList* tokens, TokenType type) {
    if (TokenArray_getCursor(tokens->array) + 1 >= TokenArray_size(tokens->array)) return 0;
    Token* tok = TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array) + 1);
    return tok ? tok->type == type : 0;
}

int lookAheadOneType(TokenList* tokens) {
    if (TokenArray_getCursor(tokens->array) + 1 >= TokenArray_size(tokens->array)) return 0;
    Token* tok = TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array) + 1);
    if (!tok) return 0;
    return tok->type == INT_KEYWORD || tok->type == LONG_KEYWORD || tok->type == DOUBLE_KEYWORD ||
           tok->type == UNSIGNED || tok->type == SIGNED;
}

int checkCompoundAssignment(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array));
    return tok ? (isCompoundAssignment(tok)) : 0;
}

int checkBinaryOp(TokenList* tokens)  {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array));
    return tok ? isBinaryOp(tok) : 0;
}

int checkUnaryOp(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array));
    return tok ? isUnaryOp(tok) : 0;
}

int checkIncrementDecrement(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array));
    return tok ? (tok->type == TWO_PLUS || tok->type == TWO_HYPHENS) : 0;
}

int checkSpecifier(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array));
    return tok ? isSpecifierType(tok->type) : 0;
}
    
void expectConstant(TokenList* tokens, int* type, uint64_t* intValue, double* floatValue) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected constant value but got end of file\n");
        exit(1);
    }

    int cursor = TokenArray_getCursor(tokens->array);
    Token* tok = TokenArray_get(tokens->array, cursor);
    if (!tok || isConstantType(tok->type) == 0) {
        fprintf(stderr, "Parser Error: Expected constant value but got %s\n", 
            tok ? tok->value : "NULL");
        exit(1);
    }
    TokenArray_setCursor(tokens->array, cursor + 1);
    *type = getConstantType(tok->type);

    if (*type == CONST_FLOATING_POINT) {
        *floatValue = strtod(tok->value, NULL);
    }
    else {
        *intValue = strtoull(tok->value, NULL, 10);
    }
}

constantType determineConstantType(uint64_t val, constantType type) {
    if (type == CONST_UNSIGNED_LONG || type == CONST_LONG) return type;

    else if (type == CONST_UNSIGNED_INT) {
        if (val <= UINT_MAX) return type;   
        else return CONST_UNSIGNED_LONG;
    }
    else if (type == CONST_INT) {
        int64_t sval = (int64_t)val;        
        if (sval >= INT_MIN  && sval <= INT_MAX)  return CONST_INT;
        else if (sval >= LONG_MIN && sval <= LONG_MAX) return CONST_LONG;
        else return CONST_UNSIGNED_LONG;
    }
}

binType compoundAssignmentToBinType(TokenType type) {
    switch(type) {
        case PLUS_EQUAL: return BIN_ADD;
        case MINUS_EQUAL: return BIN_SUBTRACT;
        case STAR_EQUAL: return BIN_MULTIPLY;
        case SLASH_EQUAL: return BIN_DIVIDE;
        case PERCENT_EQUAL: return BIN_REMAINDER;
        case AMPERSAND_EQUAL: return BIN_BITWISE_AND;
        case BAR_EQUAL: return BIN_BITWISE_OR;
        case CARET_EQUAL: return BIN_BITWISE_XOR;
        case LEFT_SHIFT_EQUAL: return BIN_LEFT_SHIFT;
        case RIGHT_SHIFT_EQUAL: return BIN_RIGHT_SHIFT;
        default:
            fprintf(stderr, "Parser Error: Expected compound assignment operator but got %s\n",
                    tokenTypeToToken(type));
            exit(1);
    }
}

void expect(TokenList* tokens, TokenType type) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected %s but got end of file\n", 
            tokenTypeToToken(type));
        exit(1);
    }

    int cursor = TokenArray_getCursor(tokens->array);
    Token* tok = TokenArray_get(tokens->array, cursor);
    if (!tok || tok->type != type) {
        fprintf(stderr, "Parser Error: Expected %s but got %s\n", 
            tokenTypeToToken(type), tok ? tok->value : "NULL");
        exit(1);
    }
    TokenArray_setCursor(tokens->array, cursor + 1);
}

char* expectIdentifier(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected identifier but got end of file\n");
        exit(1);
    }
    int cursor = TokenArray_getCursor(tokens->array);
    Token* tok = TokenArray_get(tokens->array, cursor);
    if (!tok || tok->type != IDENTIFIER) {
        fprintf(stderr, "Parser Error: Expected identifier but got %s\n", 
            tok ? tok->value : "NULL");
        exit(1);
    }
    TokenArray_setCursor(tokens->array, cursor + 1);
    return tok->value;
}

void expectCompoundAssignment(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected compound assignment operator but got end of file\n");
        exit(1);
    }
    int cursor = TokenArray_getCursor(tokens->array);
    Token* tok = TokenArray_get(tokens->array, cursor);
    if (!tok || !isCompoundAssignment(tok)) {
        fprintf(stderr, "Parser Error: Expected compound assignment operator but got %s\n", 
            tok ? tok->value : "NULL");
        exit(1);
    }
    TokenArray_setCursor(tokens->array, cursor + 1);
}

int isUnaryOp(Token* tok) {
    switch(tok->type) {
        case TILDE:
        case HYPHEN:
        case EXCLAMATION:
        case TWO_PLUS:
        case TWO_HYPHENS:
        case AMPERSAND:
        case ASTERISK:
             break;
        default: return 0;
    }

    return 1;
}


int isBinaryOp(Token* tok) {
    switch(tok->type) {
        case HYPHEN:
        case PLUS:
        case ASTERISK:
        case SLASH:
        case PERCENT:
        case AMPERSAND:
        case BAR:
        case CARET:
        case TWO_AMPERSANDS:
        case TWO_BARS:
        case TWO_EQUALS:
        case NOT_EQUAL:
        case LESS_THAN:
        case GREATER_THAN:
        case LESS_EQUAL:
        case GREATER_EQUAL:
        case ONE_EQUAL:
        case PLUS_EQUAL:
        case MINUS_EQUAL:
        case STAR_EQUAL:
        case SLASH_EQUAL:
        case PERCENT_EQUAL:
        case AMPERSAND_EQUAL:
        case BAR_EQUAL:
        case CARET_EQUAL:
        case LEFT_SHIFT_EQUAL:
        case RIGHT_SHIFT_EQUAL:
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
        case QUESTION_MARK:
            break;
        default: return 0;
    }

    return 1;
}

unaryType expectUnaryOp(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected unary operator but got end of file\n");
        exit(1);
    }

    int cursor = TokenArray_getCursor(tokens->array);
    Token* tok = TokenArray_get(tokens->array, cursor);
    if (!tok || !isUnaryOp(tok)) {
        fprintf(stderr, "Parser Error: Expected unary operator but got %s\n",
                    tok ? tok->value : "NULL");
        exit(1);
    }

    TokenArray_setCursor(tokens->array, cursor + 1);
    return tokenTypeToUnaryType(tok->type);
}


binType expectBinaryOp(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected binary operator but got end of file\n");
        exit(1);
    }

    int cursor = TokenArray_getCursor(tokens->array);
    Token* tok = TokenArray_get(tokens->array, cursor);
    if (!tok || !isBinaryOp(tok)) {
        fprintf(stderr, "Parser Error: Expected binary operator but got %s\n",
                    tok ? tok->value : "NULL");
        exit(1);
    }

    TokenArray_setCursor(tokens->array, cursor + 1);
    return tokenTypeToBinType(tok->type);
}

binType tokenTypeToBinType(TokenType type) {
    switch(type) {
        case HYPHEN: return BIN_SUBTRACT;
        case PLUS: return BIN_ADD;
        case ASTERISK: return BIN_MULTIPLY;
        case SLASH: return BIN_DIVIDE;
        case PERCENT: return BIN_REMAINDER;
        case TWO_AMPERSANDS: return BIN_AND;
        case TWO_BARS: return BIN_OR;
        case TWO_EQUALS: return BIN_EQUALS;
        case NOT_EQUAL: return BIN_NOT_EQUALS;
        case LESS_THAN: return BIN_LESS_THAN;
        case GREATER_THAN: return BIN_GREATER_THAN;
        case LESS_EQUAL: return BIN_LESS_EQUAL;
        case GREATER_EQUAL: return BIN_GREATER_EQUAL;
        case AMPERSAND: return BIN_BITWISE_AND;
        case BAR: return BIN_BITWISE_OR;
        case CARET: return BIN_BITWISE_XOR;
        case LEFT_SHIFT: return BIN_LEFT_SHIFT;
        case RIGHT_SHIFT: return BIN_RIGHT_SHIFT;
        default:
            fprintf(stderr, "Parser Error: Expected binary operator but got %s\n",
                    tokenTypeToToken(type));
            exit(1);
    }    
}

statementType loopStatementKeywordToStatementType(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected loop statement keyword but got end of file\n");
        exit(1);
    }
    int cursor = TokenArray_getCursor(tokens->array);
    Token* tok = TokenArray_get(tokens->array, cursor);
    if (!tok) {
        fprintf(stderr, "Parser Error: Expected loop statement keyword but got NULL\n");
        exit(1);
    }
    TokenType type = tok->type;
    switch(type) {
        case WHILE_KEYWORD: return STMT_WHILE;
        case FOR_KEYWORD: return STMT_FOR;
        case DO_KEYWORD: return STMT_DO_WHILE;
        case BREAK_KEYWORD: return STMT_BREAK;
        case CONTINUE_KEYWORD: return STMT_CONTINUE;
        default:
            fprintf(stderr, "Parser Error: Expected loop statement keyword but got %s\n",
                    tokenTypeToToken(type));
            exit(1);
    }
}

int checkIsLoopStatement(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array));
    return tok ? isLoopStatement(tok) : 0;

}


int isPostfixUnaryOp(unaryType type) {
    switch(type) {
        case UNARY_INCREMENT_POSTFIX:
        case UNARY_DECREMENT_POSTFIX:
            break;
        default: return 0;
    }

    return 1;
}

int isLoopStatement(Token* tok) {
    switch(tok->type) {
        case BREAK_KEYWORD:
        case CONTINUE_KEYWORD:
        case WHILE_KEYWORD:
        case FOR_KEYWORD:
        case DO_KEYWORD:
            break;
        default: return 0;
    }

    return 1;
}


unaryType FromPostPreFixToRegular(unaryType type) {
    switch(type) {
        case UNARY_INCREMENT_POSTFIX:
        case UNARY_INCREMENT_PREFIX:
             return UNARY_INCREMENT;
        case UNARY_DECREMENT_POSTFIX:
        case UNARY_DECREMENT_PREFIX:
             return UNARY_DECREMENT;
        default: return type;
    }
}

int precedence(Token* tok) {
    switch (tok->type) {
        case ASTERISK:
        case SLASH:
        case PERCENT:
            return 50;
        case HYPHEN:
        case PLUS:
            return 45;
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
            return 40;
        case LESS_THAN:
        case GREATER_THAN:
        case LESS_EQUAL:
        case GREATER_EQUAL:
            return 35;
        case TWO_EQUALS:
        case NOT_EQUAL:
            return 30;
        case AMPERSAND:
            return 25;
        case CARET:
            return 20;
        case BAR:
            return 15;
        case TWO_AMPERSANDS:
            return 10;
        case TWO_BARS:
            return 5;
        case QUESTION_MARK:
            return 3;
        case ONE_EQUAL:
        case PLUS_EQUAL:
        case MINUS_EQUAL:
        case STAR_EQUAL:
        case SLASH_EQUAL:
        case PERCENT_EQUAL:
        case AMPERSAND_EQUAL:
        case BAR_EQUAL:
        case CARET_EQUAL:
        case LEFT_SHIFT_EQUAL:
        case RIGHT_SHIFT_EQUAL:
            return 1;
        default:
            fprintf(stderr, "Parser Error: Expected binary operator but got %s\n",
                    tokenTypeToToken(tok->type));
            exit(1);
    }
}

int isIncrementDecrementOp(unaryType type) {
    switch(type) {
        case UNARY_INCREMENT:
        case UNARY_DECREMENT:
            break;
        default: return 0;
    }

    return 1;
}

int isIncrementDecrementOpIncludingFix(unaryType type) {
    switch(type) {
        case UNARY_INCREMENT:
        case UNARY_DECREMENT:
        case UNARY_INCREMENT_PREFIX:
        case UNARY_INCREMENT_POSTFIX:
        case UNARY_DECREMENT_PREFIX:
        case UNARY_DECREMENT_POSTFIX:
            break;
        default: return 0;
    }

    return 1;
}

int checkFactorStart(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    return checkConstant(tokens) || check(tokens, IDENTIFIER) || checkUnaryOp(tokens) 
    || checkBinaryOp(tokens) 
    || check(tokens, OPEN_PAREN);
}

int isCompoundAssignment(Token* tok) {
    switch(tok->type) {
        case PLUS_EQUAL:
        case MINUS_EQUAL:
        case STAR_EQUAL:
        case SLASH_EQUAL:
        case PERCENT_EQUAL:
        case AMPERSAND_EQUAL:
        case BAR_EQUAL:
        case CARET_EQUAL:
        case LEFT_SHIFT_EQUAL:
        case RIGHT_SHIFT_EQUAL:
            return 1;
        default: return 0;
    }
}

CType* getFuncReturnType(CType* funcType) {
    if (!funcType || funcType->kind != CTYPE_FUN) return NULL;
    return funcType->fun.ret;
}