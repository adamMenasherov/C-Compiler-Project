#include "ifParser.h"
#include "../Statement-Parser/StatementParser.h"
#include "../../TACKYUtils/TACKYEmitters.h"
#include "../../TACKYUtils/TACKYConstructors.h"
#include "../../../generateUtils.h"
#include "../ParserInclude.h"


typedef void (*ParseIfStatementFunc)(CIf* if_stmt, TACKYInstructionList* instructionList, TACKYValue* cond, SymbolTable* symTable);
ParseIfStatementFunc ifStatementParsers[] = {
    [IF_WITH_ELSE] = parseIfStatementWithElseInstructions,
    [IF_WITHOUT_ELSE] = parseIfStatementWithoutElseInstructions
};


void parseIfStatementInstructions(CIf* if_stmt, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    TACKYValue* cond = emit_TACKYAndConvert(if_stmt->condition, instructionList, symTable);
    ifStatementParsers[if_stmt->type](if_stmt, instructionList, cond, symTable);
}

void parseIfStatementWithElseInstructions(CIf* if_stmt, TACKYInstructionList* instructionList, TACKYValue* cond, SymbolTable* symTable) {
    char* endLabel = generateEndLabel();
    char* elseLabel = generateElseLabel();
    addInstructionToList(instructionList, 
        createJumpInstruction(TACKY_JUMP_IF_ZERO, elseLabel, cond));
    parseStatementInstructions(if_stmt->then, instructionList, symTable);
    addInstructionToList(instructionList,
        createJumpInstruction(TACKY_JUMP, endLabel, NULL));
    addInstructionToList(instructionList,
        createLabelInstruction(elseLabel));
    parseStatementInstructions(if_stmt->else_stmt, instructionList, symTable);
    addInstructionToList(instructionList,
        createLabelInstruction(endLabel));
}

void parseIfStatementWithoutElseInstructions(CIf* if_stmt, TACKYInstructionList* instructionList, TACKYValue* cond, SymbolTable* symTable) {
    char* endLabel = generateEndLabel();
    addInstructionToList(instructionList, 
    createJumpInstruction(TACKY_JUMP_IF_ZERO, endLabel, cond));
    parseStatementInstructions(if_stmt->then, instructionList, symTable);
    addInstructionToList(instructionList,
    createLabelInstruction(endLabel));
}

void parseSwitchStatementInstructions(CSwitch* switch_stmt, TACKYInstructionList* instructionList, TACKYValue* switchExp, SymbolTable* symTable) {
    char* endLabel = generateEndLabel();
    char* defaultLabel = switch_stmt->defaultCase ? generateDefaultLabel() : endLabel;
    char* caseLabels[MAX_CASES];
    int isWithDefault = switch_stmt->defaultCase != NULL;

    for (int i = 0; i < switch_stmt->caseCount; i++) {
        caseLabels[i] = generateCaseLabel(switch_stmt->cases[i]->matchVal->value.intValue);
        TACKYValue* tmpVar = makeTACKYVariable(C_CreateType(CTYPE_INT), symTable);
        TACKYValue* caseExp = createTackyValueFromConstantNode(switch_stmt->cases[i]->matchVal);
        addInstructionToList(instructionList,
            createBinaryInstruction(BIN_EQUALS, switchExp, caseExp, tmpVar));
        addInstructionToList(instructionList,
            createJumpInstruction(TACKY_JUMP_IF_NOT_ZERO, caseLabels[i], tmpVar)); 
    }    

    if (isWithDefault) {
        addInstructionToList(instructionList,
            createJumpInstruction(TACKY_JUMP, defaultLabel, NULL));
    } else {
        addInstructionToList(instructionList,
            createJumpInstruction(TACKY_JUMP, endLabel, NULL));
    }

    for (int i = 0; i < switch_stmt->caseCount; i++) {
        addInstructionToList(instructionList,
            createLabelInstruction(caseLabels[i]));
        parseStatementInstructions(switch_stmt->cases[i]->body, instructionList, symTable);
        if (switch_stmt->cases[i]->hasBreak) {
            addInstructionToList(instructionList,
                createJumpInstruction(TACKY_JUMP, endLabel, NULL));
        }
    }
    if (isWithDefault) {
        addInstructionToList(instructionList,
            createLabelInstruction(defaultLabel));
        parseStatementInstructions(switch_stmt->defaultCase, instructionList, symTable);
    }
    addInstructionToList(instructionList,
        createLabelInstruction(endLabel));
}