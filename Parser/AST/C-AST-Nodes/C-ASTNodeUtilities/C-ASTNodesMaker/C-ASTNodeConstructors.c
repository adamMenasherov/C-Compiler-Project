#include "C-ASTNodeConstructors.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


CConstant* C_CreateConstant(uint64_t intVal, double floatVal, constantType type) {
    CConstant* constant = malloc(sizeof(CConstant));
    if  (!constant) return NULL;

    if (type == CONST_FLOATING_POINT) {
        constant->value.doubleValue = floatVal;
    }
    else {
        constant->value.intValue = intVal;
    }
    constant->type = type;
    return constant;
}


CReturn* C_CreateReturn(CFactor* exp) {
    CReturn* returnNode = malloc(sizeof(CReturn));
    if (!returnNode) return NULL;
    returnNode->exp = exp;

    return returnNode;
}

CSwitch* C_CreateSwitch(CFactor* exp) {
    CSwitch* switchBlock = malloc(sizeof(CSwitch));
    if (!switchBlock) return NULL;
    switchBlock->caseCount = 0;
    switchBlock->switchExp = exp;
    switchBlock->defaultCase = NULL;
    return switchBlock;
}

CCase* C_CreateCase(CConstant* caseConst, CStatement* caseStmt, int hasBreak) {
    CCase* switchCase = malloc(sizeof(CCase));
    if (!switchCase) return NULL;
    switchCase->matchVal = caseConst;
    switchCase->body = caseStmt;
    switchCase->hasBreak = hasBreak;
    return switchCase;
}


CDeclaration* C_CreateFunction(funcDeclType type, char* identifier, IdentifierArray* parameters, CBlock* body, CType* funcType, StorageClass storageClass) {
    CDeclaration* func = calloc(1, sizeof(CDeclaration));
    if (!func) return NULL;
    func->type = DECL_FUNC;
    func->decl.functionDecl.declType = type;
    func->decl.functionDecl.identifier = strdup(identifier);
    func->decl.functionDecl.funcType = funcType;
    func->decl.functionDecl.storageClass = storageClass;
    func->decl.functionDecl.parameters = parameters;
    func->decl.functionDecl.body = body;

    return func;
}

CProgram* C_CreateProgram(CDeclarationArray* function_def) {
    CProgram* prog = malloc(sizeof(CProgram));
    if (!prog) return NULL;
    prog->function_def = function_def;

    return prog;
}

CCast* C_CreateCast(CType* castType, CFactor* exp) {
    CCast* cast = malloc(sizeof(CCast));
    if (!cast) return NULL;
    cast->targetType = castType;
    cast->exp = exp;

    return cast;
}

CSubscript* C_CreateSubscript(CFactor* array, CFactor* index) {
    CSubscript* subscript = malloc(sizeof(CSubscript));
    if (!subscript) return NULL;
    subscript->pointer = array;
    subscript->index = index;
    return subscript;
}


CUnary* C_CreateUnary(unaryType type, CFactor* exp) {
    CUnary* unary = malloc(sizeof(CUnary));
    if (!unary) return NULL;
    unary->exp = exp;
    unary->type = type;

    return unary;
}

CFactor* C_CreateFactor(factorType type, void * expVal) {
    CFactor* new_exp = calloc(1, sizeof(CFactor));
    if (!new_exp) return NULL;
    switch(type) {
        case FACTOR_UNARY: {
            new_exp->type = FACTOR_UNARY;
            new_exp->exp.unary = C_CreateUnary(((CUnary*)expVal)->type, C_CreateCopyOfFactor(((CUnary*)expVal)->exp));
            break;
        }

        case FACTOR_CONSTANT: {
            new_exp->type = FACTOR_CONSTANT;
            new_exp->exp.cnst = C_CreateConstant(((CConstant*)expVal)->value.intValue, ((CConstant*)expVal)->value.doubleValue, ((CConstant*)expVal)->type);
            break;
        }
        case FACTOR_BINARY: {
            new_exp->type = FACTOR_BINARY;
            new_exp->exp.binary = C_CreateBinary(((CBinary*)expVal)->type,
                                C_CreateCopyOfFactor(((CBinary*)expVal)->left), C_CreateCopyOfFactor(((CBinary*)expVal)->right));
            break;
        }
        case FACTOR_ASSIGNMENT: {
            new_exp->type = FACTOR_ASSIGNMENT;
            new_exp->exp.assignment = C_CreateAssignment(C_CreateCopyOfFactor(((CAssignment*)expVal)->exp1),
                                    C_CreateCopyOfFactor(((CAssignment*)expVal)->exp2));
            break;
        }
        case FACTOR_VAR: {
            new_exp->type = FACTOR_VAR;
            new_exp->exp.var = C_CreateVar(((CVar*)expVal)->identifier);
            break;
        }
        case FACTOR_CONDITIONAL: {
            new_exp->type = FACTOR_CONDITIONAL;
            new_exp->exp.conditional = C_CreateConditional(C_CreateCopyOfFactor(((CConditional*)expVal)->condition),
                                        C_CreateCopyOfFactor(((CConditional*)expVal)->then),
                                        C_CreateCopyOfFactor(((CConditional*)expVal)->else_stmt));
            break;
        }
        case FACTOR_FUNCTION_CALL: {
            new_exp->type = FACTOR_FUNCTION_CALL;
            new_exp->exp.funcCall = C_CreateFunctionCall(((CFunctionCall*)expVal)->identifier, ((CFunctionCall*)expVal)->arguments);
            break;
        }
        case FACTOR_CAST: {
            new_exp->type = FACTOR_CAST;
            new_exp->exp.cast = C_CreateCast(((CCast*)expVal)->targetType, C_CreateCopyOfFactor(((CCast*)expVal)->exp));
            break;
        }
        case FACTOR_DEREFERENCE:
        {
            new_exp->type = FACTOR_DEREFERENCE;
            new_exp->exp.pointerOp = C_CreateCopyOfFactor((CFactor*)expVal);
            break;
        }
        case FACTOR_ADDRESS_OF: {
            new_exp->type = FACTOR_ADDRESS_OF;
            new_exp->exp.pointerOp = C_CreateCopyOfFactor((CFactor*)expVal);
            break;
        }
        case FACTOR_SUBSCRIPT: {
            new_exp->type = FACTOR_SUBSCRIPT;
            new_exp->exp.subscript = C_CreateSubscript(C_CreateCopyOfFactor(((CSubscript*)expVal)->pointer),
                                                        C_CreateCopyOfFactor(((CSubscript*)expVal)->index));
            break;
        }
        default: {
            fprintf(stderr, "Invalid factor type in C_CreateFactor\n");
            free(new_exp);
            return NULL;
        }
    }
    return new_exp;
}

CBlock* C_CreateBlockFromBlockItems(CBlockItemList* items) {
    CBlock* block = malloc(sizeof(CBlock));
    if (!block) return NULL;
    block->items = items;
    return block;
}

CBlock* C_CreateBlockEmpty() {
    CBlock* block = malloc(sizeof(CBlock));
    if (!block) return NULL;
    block->items = BlockItemArray_create();
    if (!block->items) {
        free(block);
        return NULL;
    }
    return block;
}

CFunctionCall* C_CreateFunctionCall(char* identifier, ExpressionFactorArray* arguments) {
    CFunctionCall* funcCall = malloc(sizeof(CFunctionCall));
    if (!funcCall) return NULL;
    funcCall->identifier = strdup(identifier);
    funcCall->arguments = arguments;
    return funcCall;
}

CFactor* C_CreateFactorFromConstant(CConstant * exp) {
    return C_CreateFactor(FACTOR_CONSTANT, exp);
}


CFactor* C_CreateFactorFromFunctionCall(CFunctionCall* funcCall) {
    return C_CreateFactor(FACTOR_FUNCTION_CALL, funcCall);
}

CFactor* C_CreateFactorFromUnary(CUnary * exp) {
    return C_CreateFactor(FACTOR_UNARY, exp);
}

CFactor* C_CreateFactorFromCast(CCast* cast) {
    return C_CreateFactor(FACTOR_CAST, cast);
}

CFactor* C_CreateFactorFromVar(CVar* var) {
    return C_CreateFactor(FACTOR_VAR, var);
}

CFactor* C_CreateFactorFromBinary(CBinary * exp) {
    return C_CreateFactor(FACTOR_BINARY, exp);
}

CFactor* C_CreateFactorFromAssignment(CAssignment* assign) {
    return C_CreateFactor(FACTOR_ASSIGNMENT, assign);
}

CFactor* C_CreateFactorFromConditional(CConditional* conditional) {
    return C_CreateFactor(FACTOR_CONDITIONAL, conditional);
}

CFactor* C_CreateFactorFromSubscript(CSubscript* subscript) {
    return C_CreateFactor(FACTOR_SUBSCRIPT, subscript);
}

CIf* C_CreateIf(ifType type, CFactor* condition, CStatement* then, CStatement* else_stmt) {
    CIf* ifNode = malloc(sizeof(CIf));
    if (!ifNode) return NULL;
    ifNode->type = type;
    ifNode->condition = condition;
    ifNode->then = then;
    ifNode->else_stmt = else_stmt;

    return ifNode;
}


CStatement* C_CreateStatement(statementType type, void * stmtVal) {
    CStatement* stmt = malloc(sizeof(CStatement));
    if (!stmt) return NULL;
    stmt->type = type;
    switch(type) {
        case STMT_RETURN: {
            stmt->stmt.ret = (CReturn*)stmtVal;
            break;
        }
        case STMT_EXPRESSION: {
            stmt->stmt.exp = (CFactor*)stmtVal;
            break;
        }
        case STMT_NULL: {
            stmt->stmt.exp = NULL;
            break;
        }
        case STMT_IF: {
            stmt->stmt.if_stmt = (CIf*)stmtVal;
            break;
        }
        case STMT_SWITCH: {
            stmt->stmt.switch_stmt = (CSwitch*)stmtVal;
            break;
        }
        case STMT_COMPOUND: {
            stmt->stmt.compound_stmt = (CCompound*)stmtVal;
            break;
        }
        case STMT_WHILE: {
            stmt->stmt.while_stmt = (CLoop*)stmtVal;
            break;
        }
        case STMT_DO_WHILE: {
            stmt->stmt.do_while_stmt = (CLoop*)stmtVal;
            break;
        }
        case STMT_FOR: {
            stmt->stmt.for_stmt = (CForLoop*)stmtVal;
            break;
        }
        case STMT_BREAK:
        case STMT_CONTINUE: {
            stmt->stmt.break_stmt = (CLoopStmt*)stmtVal;
            break;
        }
        default: {
            fprintf(stderr, "Invalid statement type in C_CreateStatement\n");
            free(stmt);
            return NULL;
        }
    }
    return stmt;
}


CLoop* C_CreateLoop(CFactor* condition, CStatement* body) {
    CLoop* loop = calloc(1, sizeof(CLoop));
    if (!loop) return NULL;
    loop->condition = condition;
    loop->body = body;
    return loop;
}

CForLoop* C_CreateForLoop(CForInit* init, CFactor* condition, CFactor* post, CStatement* body) {
    CForLoop* forLoop = calloc(1, sizeof(CForLoop));
    if (!forLoop) return NULL;
    forLoop->init = init;
    forLoop->condition = condition;
    forLoop->post = post;
    forLoop->body = body;
    return forLoop;
}

CLoopStmt* C_CreateLoopStmt() {
    CLoopStmt* loopStmt = calloc(1, sizeof(CLoopStmt));
    if (!loopStmt) return NULL;
    loopStmt->identifier = generateLoopStmtIdentifier();
    return loopStmt;
}



CForInit* C_CreateForInit(forInitType type, void* initVal) {
    CForInit* forInit = calloc(1, sizeof(CForInit));
    if (!forInit) return NULL;
    forInit->type = type;
    switch (type) {
        case FOR_INIT_DECL: {
            forInit->decl = (CDeclaration*)initVal;
            break;
        }
        case FOR_INIT_EXP: {
            forInit->exp = (CFactor*)initVal;
            break;
        }
        case FOR_INIT_WITHOUT: {
            forInit->exp = NULL;
            break;
        }
        default: {
            fprintf(stderr, "Invalid for init type in C_CreateForInit\n");
            free(forInit);
            return NULL;
        }
    }
    return forInit;
}

CVar* C_CreateVar(char* identifier) {
    CVar* var = malloc(sizeof(CVar));
    if (!var) return NULL;
    var->identifier = strdup(identifier);

    return var;
}

CType* C_CreateTypeFromType(CType* original) {
    if (!original) return NULL;
    CType* copy = C_CreateType(original->kind);
    if (!copy) return NULL;

    switch (original->kind) {
        case CTYPE_FUN:
            copy->fun.paramCnt = original->fun.paramCnt;
            copy->fun.ret = C_CreateTypeFromType(original->fun.ret);
            for (int i = 0; i < original->fun.paramCnt && i < MAX_PARAMS; i++) {
                copy->fun.params[i] = C_CreateTypeFromType(original->fun.params[i]);
            }
            break;
        case CTYPE_POINTER:
            copy->pointer.referenced = C_CreateTypeFromType(original->pointer.referenced);
            break;
        case CTYPE_ARRAY:
            copy->array.elementType = C_CreateTypeFromType(original->array.elementType);
            copy->array.size = original->array.size;
            break;
        default:
            break;
    }
    return copy;
}

CType* C_CreateType(CTypeKind kind) {
    CType* ctype = calloc(1, sizeof(CType));
    if (!ctype) return NULL;
    ctype->kind = kind;
    return ctype;
}

CType* C_CreatePointerType(CType* referenced) {
    CType* ctype = calloc(1, sizeof(CType));
    if (!ctype) return NULL;
    ctype->kind = CTYPE_POINTER;
    ctype->pointer.referenced = referenced;
    return ctype;
}

CType* C_CreateFunType(CParamInfo* params, int paramCnt, CType* ret) {
    CType* ctype = calloc(1, sizeof(CType));
    if (!ctype) return NULL;
    ctype->kind = CTYPE_FUN;
    ctype->fun.paramCnt = paramCnt;
    ctype->fun.ret = ret;
    for (int i = 0; i < paramCnt && i < MAX_PARAMS; i++)
        ctype->fun.params[i] = C_CreateTypeFromParamInfo(params[i]);
    return ctype;
}

CType* C_CreateArrayType(CType* elementType, int size) {
    CType* ctype = calloc(1, sizeof(CType));
    if (!ctype) return NULL;
    ctype->kind = CTYPE_ARRAY;
    ctype->array.elementType = elementType;
    ctype->array.size = size;
    return ctype;
}

CType* C_CreateTypeFromParamInfo(CParamInfo param) {
    CType* ctype = C_CreateTypeFromType(param.type);
    if (!ctype) return NULL;
    return ctype;
}

CFactor* C_CreateAddrOfNode(CFactor* pointer) {
    return C_CreateFactor(FACTOR_ADDRESS_OF, pointer);
}

CFactor* C_CreateCopyOfFactor(CFactor* original) {
    if (!original) return NULL;
    CFactor* copy = NULL;
    switch (original->type) {
        case FACTOR_CONSTANT:
            copy = C_CreateFactorFromConstant(original->exp.cnst);
            break;
        case FACTOR_UNARY:
            copy = C_CreateFactorFromUnary(original->exp.unary);
            break;
        case FACTOR_BINARY:
            copy = C_CreateFactorFromBinary(original->exp.binary);
            break;
        case FACTOR_VAR:
            copy = C_CreateFactorFromVar(original->exp.var);
            break;
        case FACTOR_ASSIGNMENT:
            copy = C_CreateFactorFromAssignment(original->exp.assignment);
            break;
        case FACTOR_CONDITIONAL:
            copy = C_CreateFactorFromConditional(original->exp.conditional);
            break;
        case FACTOR_FUNCTION_CALL:
            copy = C_CreateFactorFromFunctionCall(original->exp.funcCall);
            break;
        case FACTOR_CAST:
            copy = C_CreateFactorFromCast(original->exp.cast);
            break;
        case FACTOR_DEREFERENCE:
            copy = C_CreateFactor(FACTOR_DEREFERENCE, original->exp.pointerOp);
            break;
        case FACTOR_ADDRESS_OF:
            copy = C_CreateFactor(FACTOR_ADDRESS_OF, original->exp.pointerOp);
            break;
        case FACTOR_SUBSCRIPT:
            copy = C_CreateFactorFromSubscript(original->exp.subscript);
            break;
        default:
            fprintf(stderr, "Invalid factor type in C_CreateCopyOfFactor\n");
            return NULL;
    }
    if (copy) copy->valueType = original->valueType;
    return copy;
}

CBinary* C_CreateBinary(binType type, CFactor * left, CFactor * right) {
    CBinary* binary = malloc(sizeof(CBinary));
    if (!binary) return NULL;
    binary->type = type;
    binary->left = left;
    binary->right = right;
    return binary;
}

CDeclaration* C_CreateVariableDeclaration(varDeclType type, char* iden, CInitializer* assign, CType* varType, StorageClass storageClass) {
    CDeclaration* decl = malloc(sizeof(CDeclaration));
    if (!decl) return NULL;
    decl->type = DECL_VAR;
    decl->decl.variableDecl.declType = type;
    decl->decl.variableDecl.init = assign;
    decl->decl.variableDecl.varType = varType;
    decl->decl.variableDecl.storageClass = storageClass;
    decl->decl.variableDecl.identifier = iden;
    return decl;
}

CInitializer* C_CreateSingleInit(CFactor* exp) {
    CInitializer* init = malloc(sizeof(CInitializer));
    if (!init) return NULL;
    init->type = INIT_SINGLE;
    init->init.singleInit = exp;
    return init;
}
CInitializer* C_CreateCompoundInit(CInitializerList* list) {
    CInitializer* init = malloc(sizeof(CInitializer));
    if (!init) return NULL;
    init->type = INIT_COMPOUND;
    init->init.compoundInit.initializers = list;
    return init;
}

CAssignment* C_CreateAssignment(CFactor* fact1, CFactor* fact2) {
    CAssignment* assign = malloc(sizeof(CAssignment));
    if (!assign) return NULL;
    assign->exp1 = fact1;
    assign->exp2 = fact2;
    return assign;
}

CBlockItem* C_CreateBlockItem(blockItemType type, void * stmtVal) {
    CBlockItem* blockItem = malloc(sizeof(CBlockItem));
    blockItem->type = type;
    if (!blockItem) return NULL;
    switch(type) {
        case BLOCK_ITEM_DECL: {
            CDeclaration* decl = (CDeclaration*)stmtVal;
            blockItem->item.decl = decl;
            break;
        }
        case BLOCK_ITEM_STMT: {
            CStatement* stmt = (CStatement*)stmtVal;
            blockItem->item.stmt = stmt;
            break;
        }
        default: {
            fprintf(stderr, "Invalid block item type in C_CreateBlockItem\n");
            free(blockItem);
            return NULL;
        }
    }
    return blockItem;
}


CConditional* C_CreateConditional(CFactor* condition, CFactor* then, CFactor* else_stmt) {
    CConditional* conditional = malloc(sizeof(CConditional));
    if (!conditional) return NULL;
    conditional->condition = condition;
    conditional->then = then;
    conditional->else_stmt = else_stmt;
    return conditional;
}

CCompound* C_CreateCompound(CBlock* block) {
    CCompound* compound = malloc(sizeof(CCompound));
    if (!compound) return NULL;
    compound->block = block;
    return compound;
}

char* generateLoopStmtIdentifier() {
    static int loopStmtCounter = 0;
    char* identifier = malloc(20);
    if (!identifier) return NULL;
    sprintf(identifier, "loop_stmt_%d", loopStmtCounter++);
    return identifier;
}

CDeclarator* C_CreateIdentDeclarator(char* identifier) {
    CDeclarator* decl = malloc(sizeof(CDeclarator));
    if (!decl) return NULL;
    decl->type = DECLARATOR_IDENT;
    decl->decl.identifier = identifier ? strdup(identifier) : NULL;
    return decl;
}
CDeclarator* C_CreatePointerDeclarator(CDeclarator* pointed) {
    CDeclarator* decl = malloc(sizeof(CDeclarator));
    if (!decl) return NULL;
    decl->type = DECLARATOR_POINTER;
    decl->decl.pointerDecl.pointed = pointed;
    return decl;
}

CDeclarator* C_CreateArrayDeclarator(CDeclarator* arrayDecl, int size) {
    CDeclarator* decl = malloc(sizeof(CDeclarator));
    if (!decl) return NULL;
    decl->type = DECLARATOR_ARRAY;
    decl->decl.array.arrayDecl = arrayDecl;
    decl->decl.array.size = size;
    return decl;
}

CDeclarator* C_CreateFunctionDeclarator(CDeclarator* funcDecl, CParamInfo* params, int paramCnt) {
    CDeclarator* decl = malloc(sizeof(CDeclarator));
    if (!decl) return NULL;
    decl->type = DECLARATOR_FUNC;
    decl->decl.func.funcDecl = funcDecl;
    decl->decl.func.paramCnt = paramCnt;
    for (int i = 0; i < paramCnt && i < MAX_PARAMS; i++) {
        decl->decl.func.params[i] = params[i];
    }
    return decl;
}