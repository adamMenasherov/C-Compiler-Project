#pragma once
#include "../../TACKYProgram.h"


void parseIfStatementInstructions(CIf* if_stmt, TACKYInstructionList* instructionList);
void parseIfStatementWithElseInstructions(CIf* if_stmt, TACKYInstructionList* instructionList, TACKYValue* cond);
void parseIfStatementWithoutElseInstructions(CIf* if_stmt, TACKYInstructionList* instructionList, TACKYValue* cond);
void parseSwitchStatementInstructions(CSwitch* switch_stmt, TACKYInstructionList* instructionList, TACKYValue* switchExp);