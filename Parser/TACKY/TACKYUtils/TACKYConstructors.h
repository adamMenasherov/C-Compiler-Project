#pragma once

#include "../TACKY_AST.h"

TACKYInstruction* createUnaryInstruction(unaryType type, TACKYValue* src, TACKYValue* dest);
TACKYInstruction* createBinaryInstruction(binType type, TACKYValue* src1, TACKYValue* src2, TACKYValue* dest);
TACKYInstruction* createReturnInstruction(TACKYValue* retVal);
TACKYInstruction* createJumpInstruction(TACKYInstructionType jumpType, char* label, TACKYValue* condition);
TACKYInstruction* createLabelInstruction(char* label);
TACKYInstruction* createCopyInstruction(TACKYValue* src, TACKYValue* dest);
TACKYValue* createTackyValueFromConstant(int val);
TACKYValue* createTackyValueFromVar(CVar* var);
TACKYValue* createTackyValueFromConstantNode(CConstant* const_node);
TACKYValue* createVarValue(char* identifier);
TACKYInstructionList* createTACKYInstructionList();
TACKYConstant* CreateTackyConstantNode(int val);
void addInstructionToList(TACKYInstructionList* list, TACKYInstruction* instruction);
char* generateTempName();
char* generateFalseLabel();
char* generateTrueLabel();
char* generateEndLabel();
char* generateE2Label();
char* generateElseLabel();
char* generateResultVarName();
TACKYValue* copyTackyValue(TACKYValue* original);