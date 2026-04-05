#include "TACKY_AST.h"
#include "TACKYUtils/TACKYEmitters.h"
#include "TACKYUtils/TACKYConstructors.h"
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
            if (blockItem->item.decl->declType == DECL_WITH_EXP) {
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
    switch(stmt->type) {
        case STMT_EXPRESSION:
            int isPostfixUnary = 0;
            emit_TACKY(stmt->stmt.exp, instructionList, &isPostfixUnary);
            if (isPostfixUnary) addInstructionToList(instructionList,
                emitUnaryPostfixInstruction(stmt->stmt.exp));
            break;
        case STMT_RETURN:
            parseTACKYReturn(stmt->stmt.ret, instructionList);
            break;
        case STMT_IF:
            parseIfStatementInstructions(stmt->stmt.if_stmt, instructionList);
            break;
        case STMT_COMPOUND:
            parseBlock(stmt->stmt.compound_stmt->block, instructionList);
            break;
        case STMT_NULL:
            break;
    }
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