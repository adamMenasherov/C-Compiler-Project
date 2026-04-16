#include "TACKY_AST.h"
#include "TACKYUtils/TACKYEmitters.h"
#include "TACKYUtils/TACKYConstructors.h"
#include "../generateUtils.h"
#include <stdlib.h>

TACKYFunction* parseTACKYFunction(CFunction* func) {
    TACKYInstructionList* instruction_list = createTACKYInstructionList();
    TACKYFunction* tackyFunc = malloc(sizeof(TACKYFunction));
    if (!tackyFunc) return NULL;

    parseBlock(func->block, instruction_list);
    addInstructionToList(instruction_list, createReturnInstruction(createTackyValueFromConstant(0))); 

    tackyFunc->function_name = func->function_name;
    tackyFunc->instruction_list = instruction_list;
    return tackyFunc;
}

TACKYProgram* parseTACKYProgram(CProgram* program) {
    TACKYFunction* function_def = parseTACKYFunction(program->function_def);
    TACKYProgram* tackyProg = malloc(sizeof(TACKYProgram));
    if (!tackyProg) return NULL;

    tackyProg->function_def = function_def;
    return tackyProg;
}

void parseTACKYReturn(CReturn* returnNode, TACKYInstructionList* instructionList) {
    TACKYValue* ret_val = emit_TACKY(returnNode->exp, instructionList, NULL);
    TACKYInstruction* ret_inst = createReturnInstruction(ret_val);
    addInstructionToList(instructionList, ret_inst);
}

void parseBlockItemInstructions(CBlockItem* blockItem, TACKYInstructionList* instructionList) {
    switch(blockItem->type) {
        case BLOCK_ITEM_DECL:
            if (blockItem->item.decl->declType == VAR_DECL_WITH_EXP) {
                char* varName = blockItem->item.decl->identifier;
                int isPostfixUnary = 0;
                TACKYValue* src = emit_TACKY(blockItem->item.decl->exp, instructionList, &isPostfixUnary);
                TACKYValue* dst = createVarValue(varName);
                addInstructionToList(instructionList,
                    createCopyInstruction(src, dst));

                if (isPostfixUnary) addInstructionToList(instructionList,
                    emitUnaryPostfixInstruction(blockItem->item.decl->exp));
            }
            break;
        case BLOCK_ITEM_STMT:
            parseStatementInstructions(blockItem->item.stmt, instructionList);
            break;
    }
}

void parseStatementInstructions(CStatement* stmt, TACKYInstructionList* instructionList) {
    int isPostfixUnary = 0;
    switch(stmt->type) {
        case STMT_EXPRESSION: {
            emit_TACKY(stmt->stmt.exp, instructionList, &isPostfixUnary);
            if (isPostfixUnary) addInstructionToList(instructionList,
                emitUnaryPostfixInstruction(stmt->stmt.exp));
            break;
        }   
        case STMT_RETURN: {
            parseTACKYReturn(stmt->stmt.ret, instructionList);
            break;
        }
        case STMT_BREAK:
            addInstructionToList(instructionList,
                createJumpInstruction(TACKY_JUMP, 
                    generateBreakLabelFromLoopLabel(stmt->stmt.break_stmt->identifier), NULL));
            break;
        case STMT_CONTINUE:
            addInstructionToList(instructionList,
                createJumpInstruction(TACKY_JUMP, 
                    generateContinueLabelFromLoopLabel(stmt->stmt.continue_stmt->identifier), NULL));
            break;
        case STMT_NULL:
            break;
        case STMT_IF:
            parseIfStatementInstructions(stmt->stmt.if_stmt, instructionList);
            break;
        case STMT_COMPOUND:
            parseBlock(stmt->stmt.compound_stmt->block, instructionList);
            break;
        case STMT_WHILE: parseWhileLoopInstructions(stmt->stmt.while_stmt, instructionList);
            break;
        case STMT_DO_WHILE: parseDoWhileLoopInstructions(stmt->stmt.do_while_stmt, instructionList);
            break;
        case STMT_FOR:
            parseForLoopInstructions(stmt->stmt.for_stmt, instructionList);
            break;
    }
}

void parseForLoopInstructions(CForLoop* forLoop, TACKYInstructionList* instructionList) {
    char* start = forLoop->identifier;
    char* continueLabel = generateContinueLabelFromLoopLabel(forLoop->identifier);
    char* breakLabel = generateBreakLabelFromLoopLabel(forLoop->identifier);
    parseForLoopInitInstructions(forLoop->init, instructionList);
    addInstructionToList(instructionList, createLabelInstruction(start));

    if (forLoop->condition) {
        int isPostfixUnary = 0;
        TACKYValue* condition = emit_TACKY(forLoop->condition, instructionList, &isPostfixUnary);
        if (isPostfixUnary) addInstructionToList(instructionList,
            emitUnaryPostfixInstruction(forLoop->condition));
        addInstructionToList(instructionList,
            createJumpInstruction(TACKY_JUMP_IF_ZERO, breakLabel, condition));
    }
    
    parseStatementInstructions(forLoop->body, instructionList);
    addInstructionToList(instructionList,
        createLabelInstruction(continueLabel));

    int isPostfixUnary = 0;
    emit_TACKY(forLoop->post, instructionList, &isPostfixUnary);
    if (isPostfixUnary) addInstructionToList(instructionList,
        emitUnaryPostfixInstruction(forLoop->post));    

    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP, start, NULL));
    addInstructionToList(instructionList,
        createLabelInstruction(breakLabel));
}

void parseDoWhileLoopInstructions(CLoop* loop, TACKYInstructionList* instructionList) {
    char* start = loop->identifier;
    char* continueLabel = generateContinueLabelFromLoopLabel(loop->identifier);
    char* breakLabel = generateBreakLabelFromLoopLabel(loop->identifier);
    addInstructionToList(instructionList, createLabelInstruction(start));
    parseStatementInstructions(loop->body, instructionList);
    addInstructionToList(instructionList, createLabelInstruction(continueLabel));

    int isPostfixUnary = 0;
    TACKYValue* condition = emit_TACKY(loop->condition, instructionList, &isPostfixUnary);
    if (isPostfixUnary) addInstructionToList(instructionList,
        emitUnaryPostfixInstruction(loop->condition));

    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP_IF_NOT_ZERO, start, condition));
    addInstructionToList(instructionList,
        createLabelInstruction(breakLabel));
}


void parseForLoopInitInstructions(CForInit* init, TACKYInstructionList* instructionList) {
    if (!init) return;
    switch (init->type) {
        case FOR_INIT_DECL:
            if (init->decl->declType == VAR_DECL_WITH_EXP) {
                char* varName = init->decl->identifier;
                int isPostfixUnary = 0;
                TACKYValue* src = emit_TACKY(init->decl->exp, instructionList, &isPostfixUnary);
                TACKYValue* dst = createVarValue(varName);
                addInstructionToList(instructionList,
                    createCopyInstruction(src, dst));

                if (isPostfixUnary) addInstructionToList(instructionList,
                    emitUnaryPostfixInstruction(init->decl->exp));
            }
            break;
        case FOR_INIT_EXP:
            emit_TACKY(init->exp, instructionList, NULL);
            break;
        case FOR_INIT_WITHOUT:
            break;
    }
}


void parseWhileLoopInstructions(CLoop* loop, TACKYInstructionList* instructionList) {
    char* continueLabel = generateContinueLabelFromLoopLabel(loop->identifier);
    char* breakLabel = generateBreakLabelFromLoopLabel(loop->identifier);
    addInstructionToList(instructionList, createLabelInstruction(continueLabel));
    
    int isPostfixUnary = 0;
    TACKYValue* condition = emit_TACKY(loop->condition, instructionList, &isPostfixUnary);
    if (isPostfixUnary) addInstructionToList(instructionList,
        emitUnaryPostfixInstruction(loop->condition));

    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP_IF_ZERO, breakLabel, condition));
    parseStatementInstructions(loop->body, instructionList);
    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP, continueLabel, NULL));
    addInstructionToList(instructionList,
        createLabelInstruction(breakLabel));
}

void parseIfStatementInstructions(CIf* if_stmt, TACKYInstructionList* instructionList) {
    int isPostfixUnary = 0;
    char* endLabel = generateEndLabel();
    TACKYValue* cond = emit_TACKY(if_stmt->condition, instructionList, &isPostfixUnary);
    
    switch(if_stmt->type) {
        case IF_WITH_ELSE: {
            char* elseLabel = generateElseLabel();
            addInstructionToList(instructionList, 
                createJumpInstruction(TACKY_JUMP_IF_ZERO, elseLabel, cond));
            parseStatementInstructions(if_stmt->then, instructionList);
            addInstructionToList(instructionList,
                createJumpInstruction(TACKY_JUMP, endLabel, NULL));
            addInstructionToList(instructionList,
                createLabelInstruction(elseLabel));
            parseStatementInstructions(if_stmt->else_stmt, instructionList);
            addInstructionToList(instructionList,
                createLabelInstruction(endLabel));
            break;
        }
        case IF_WITHOUT_ELSE: {
            addInstructionToList(instructionList, 
                createJumpInstruction(TACKY_JUMP_IF_ZERO, endLabel, cond));
            parseStatementInstructions(if_stmt->then, instructionList);
            addInstructionToList(instructionList,
                createLabelInstruction(endLabel));
            break;
        }   
    }
}

void parseBlock(CBlock* block, TACKYInstructionList* instructionList) {
    DArray_forEach(block->items, elem,
    {
        parseBlockItemInstructions((CBlockItem*)elem, instructionList);
    });
}