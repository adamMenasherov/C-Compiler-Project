#pragma once

#include "../TACKYProgram.h"

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
TACKYInstruction* createFunCall(char* functionName, TACKYValueArray* args, TACKYValue* resultVar);
TACKYInstructionList* createTACKYInstructionList();
TACKYConstant* CreateTackyConstantNode(int val);
void addInstructionToList(TACKYInstructionList* list, TACKYInstruction* instruction);
TACKYValue* copyTackyValue(TACKYValue* original);
TACKYProgram* createTACKYProgram();
TACKYFunction* createTACKYFunction(char* function_name, IdentifierArray* parameters, TACKYInstructionList* instruction_list, int global);
TACKYTopLevel* createTACKYTopLevelFromFunction(TACKYFunction* tackyFunc, SymbolTable* symTable);
TACKYTopLevel* createTACKYTopLevelFromStaticVar(TACKYStaticVar* staticVar);
TACKYStaticVar* createTACKYStaticVar(char* identifier, int global, int val);