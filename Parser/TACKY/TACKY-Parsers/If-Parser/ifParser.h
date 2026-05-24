#pragma once
#include "../../TACKYProgram.h"


void parseIfStatementInstructions(CIf* if_stmt, TACKYInstructionList* instructionList, SymbolTable* symTable);
void parseIfStatementWithElseInstructions(CIf* if_stmt, TACKYInstructionList* instructionList, TACKYValue* cond, SymbolTable* symTable);
void parseIfStatementWithoutElseInstructions(CIf* if_stmt, TACKYInstructionList* instructionList, TACKYValue* cond, SymbolTable* symTable);
void parseSwitchStatementInstructions(CSwitch* switch_stmt, TACKYInstructionList* instructionList, TACKYValue* switchExp, SymbolTable* symTable);