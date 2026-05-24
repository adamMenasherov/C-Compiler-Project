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
        int isPostfixUnary = 0;
        TACKYValue* condition = emit_TACKY(forLoop->condition, instructionList, &isPostfixUnary, symTable);
        if (isPostfixUnary) addInstructionToList(instructionList,
            emitUnaryPostfixInstruction(forLoop->condition, symTable));
        addInstructionToList(instructionList,
            createJumpInstruction(TACKY_JUMP_IF_ZERO, breakLabel, condition));
    }
    
    parseStatementInstructions(forLoop->body, instructionList, symTable);
    addInstructionToList(instructionList,
        createLabelInstruction(continueLabel));

    int isPostfixUnary = 0;
    emit_TACKY(forLoop->post, instructionList, &isPostfixUnary, symTable);
    if (isPostfixUnary) addInstructionToList(instructionList,
        emitUnaryPostfixInstruction(forLoop->post, symTable));    

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

    int isPostfixUnary = 0;
    TACKYValue* condition = emit_TACKY(loop->condition, instructionList, &isPostfixUnary, symTable);
    if (isPostfixUnary) addInstructionToList(instructionList,
        emitUnaryPostfixInstruction(loop->condition, symTable));

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
                int isPostfixUnary = 0;
                TACKYValue* src = emit_TACKY(init->decl->decl.variableDecl.exp, instructionList, &isPostfixUnary, symTable);
                TACKYValue* dst = createVarValue(varName);
                addInstructionToList(instructionList,
                    createCopyInstruction(src, dst));

                if (isPostfixUnary) addInstructionToList(instructionList,
                    emitUnaryPostfixInstruction(init->decl->decl.variableDecl.exp, symTable));
            }
            break;
        case FOR_INIT_EXP:
            emit_TACKY(init->exp, instructionList, NULL, symTable);
            break;
        case FOR_INIT_WITHOUT:
            break;
    }
}


void parseWhileLoopInstructions(CLoop* loop, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    char* continueLabel = generateContinueLabelFromLoopLabel(loop->identifier);
    char* breakLabel = generateBreakLabelFromLoopLabel(loop->identifier);
    addInstructionToList(instructionList, createLabelInstruction(continueLabel));
    
    int isPostfixUnary = 0;
    TACKYValue* condition = emit_TACKY(loop->condition, instructionList, &isPostfixUnary, symTable);
    if (isPostfixUnary) addInstructionToList(instructionList,
        emitUnaryPostfixInstruction(loop->condition, symTable));

    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP_IF_ZERO, breakLabel, condition));
    parseStatementInstructions(loop->body, instructionList, symTable);
    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP, continueLabel, NULL));
    addInstructionToList(instructionList,
        createLabelInstruction(breakLabel));
}