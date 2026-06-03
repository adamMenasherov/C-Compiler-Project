#include "LoopParser.h"
#include "../Statement-Parser/StatementParser.h"
#include "../../TACKYUtils/TACKYEmitters.h"
#include "../../TACKYUtils/TACKYConstructors.h"
#include "../../../generateUtils.h"

void parseForLoopInstructions(CForLoop* forLoop, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    char* start = forLoop->identifier;
    char* continueLabel = generateContinueLabelFromLoopLabel(forLoop->identifier);
    char* breakLabel = generateBreakLabelFromLoopLabel(forLoop->identifier);
    parseForLoopInitInstructions(forLoop->init, instructionList, symTable);
    addInstructionToList(instructionList, createLabelInstruction(start));

    if (forLoop->condition) {
        TACKYValue* condition = emit_TACKYAndConvert(forLoop->condition, instructionList, symTable);
        addInstructionToList(instructionList,
            createJumpInstruction(TACKY_JUMP_IF_ZERO, breakLabel, condition));
    }

    parseStatementInstructions(forLoop->body, instructionList, symTable);
    addInstructionToList(instructionList,
        createLabelInstruction(continueLabel));

    emit_TACKYAndConvert(forLoop->post, instructionList, symTable);    

    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP, start, NULL));
    addInstructionToList(instructionList,
        createLabelInstruction(breakLabel));
}

void parseDoWhileLoopInstructions(CLoop* loop, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    char* start = loop->identifier;
    char* continueLabel = generateContinueLabelFromLoopLabel(loop->identifier);
    char* breakLabel = generateBreakLabelFromLoopLabel(loop->identifier);
    addInstructionToList(instructionList, createLabelInstruction(start));
    parseStatementInstructions(loop->body, instructionList, symTable);
    addInstructionToList(instructionList, createLabelInstruction(continueLabel));

    TACKYValue* condition = emit_TACKYAndConvert(loop->condition, instructionList, symTable);
    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP_IF_NOT_ZERO, start, condition));
    addInstructionToList(instructionList,
        createLabelInstruction(breakLabel));
}


void parseForLoopInitInstructions(CForInit* init, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    if (!init) return;
    switch (init->type) {
        case FOR_INIT_DECL:
            if (init->decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP) {
                char* varName = init->decl->decl.variableDecl.identifier;
                TACKYValue* src = emit_TACKYAndConvert(init->decl->decl.variableDecl.exp, instructionList, symTable);
                TACKYValue* dst = createVarValue(varName);
                addInstructionToList(instructionList,
                    createCopyInstruction(src, dst));
            }
            break;
        case FOR_INIT_EXP:
            emit_TACKYAndConvert(init->exp, instructionList, symTable);
            break;
        case FOR_INIT_WITHOUT:
            break;
    }
}


void parseWhileLoopInstructions(CLoop* loop, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    char* continueLabel = generateContinueLabelFromLoopLabel(loop->identifier);
    char* breakLabel = generateBreakLabelFromLoopLabel(loop->identifier);
    addInstructionToList(instructionList, createLabelInstruction(continueLabel));
    
    TACKYValue* condition = emit_TACKYAndConvert(loop->condition, instructionList, symTable);
    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP_IF_ZERO, breakLabel, condition));
    parseStatementInstructions(loop->body, instructionList, symTable);
    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP, continueLabel, NULL));
    addInstructionToList(instructionList,
        createLabelInstruction(breakLabel));
}