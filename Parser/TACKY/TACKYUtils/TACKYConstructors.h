#pragma once

#include "../TACKYProgram.h"
#include "TACKYEmitters.h"

TACKYInstruction* createUnaryInstruction(unaryType type, TACKYValue* src, TACKYValue* dest);
TACKYInstruction* createBinaryInstruction(binType type, TACKYValue* src1, TACKYValue* src2, TACKYValue* dest);
TACKYInstruction* createReturnInstruction(TACKYValue* retVal);
TACKYInstruction* createJumpInstruction(TACKYInstructionType jumpType, char* label, TACKYValue* condition);
TACKYInstruction* createLabelInstruction(char* label);
TACKYInstruction* createCopyInstruction(TACKYValue* src, TACKYValue* dest);
TACKYInstruction* createSignExtendInstruction(TACKYValue* src, TACKYValue* dest);
TACKYInstruction* createZeroExtendInstruction(TACKYValue* src, TACKYValue* dest);
TACKYInstruction* createTruncateInstruction(TACKYValue* src, TACKYValue* dest);
TACKYInstruction* createDoubleIntCastInstruction(TACKYValue* src, TACKYValue* dest, TACKYInstructionType instType);
TACKYInstruction* createAddPtrInstruction(TACKYValue* base, TACKYValue* offset, int scale, TACKYValue* dest);
TACKYValue* createTackyValueFromConstant(uint64_t intValue, double doubleValue, constantType type);
TACKYValue* createTackyValueFromVar(CVar* var);
TACKYValue* createTackyValueFromConstantNode(CConstant* const_node);
TACKYValue* createVarValue(char* identifier);
TACKYInstruction* generateTACKYDoubleIntCast(TACKYValue* src, TACKYValue* dst, CType* srcType, CType* dstType, SymbolTable* symTable);
TACKYInstruction* createFunCall(char* functionName, TACKYValueArray* args, TACKYValue* resultVar);
TACKYInstructionList* createTACKYInstructionList();
TACKYConstant* CreateTackyConstantNode(uint64_t intValue, double doubleValue, constantType type);
void addInstructionToList(TACKYInstructionList* list, TACKYInstruction* instruction);
TACKYValue* copyTackyValue(TACKYValue* original);
TACKYInstruction* createGetAddressInstruction(TACKYValue* src, TACKYValue* dest);
TACKYInstruction* createLoadInstruction(TACKYValue* src_ptr, TACKYValue* dest);
TACKYInstruction* createStoreInstruction(TACKYValue* src, TACKYValue* dst_ptr);
TACKYProgram* createTACKYProgram();
TACKYFunction* createTACKYFunction(char* function_name, IdentifierArray* parameters, TACKYInstructionList* instruction_list, int global);
TACKYTopLevel* createTACKYTopLevelFromFunction(TACKYFunction* tackyFunc, SymbolTable* symTable);
TACKYTopLevel* createTACKYTopLevelFromStaticVar(TACKYStaticVar* staticVar);
TACKYInstruction* createCopyToOffsetInstruction(TACKYValue* src, TACKYValue* dest, int offset);
TACKYStaticVar* createTACKYStaticVar(char* identifier, int global, initialValueStaticInitType staticInitType);
ExpResult* createExpResult(expResultType type, TACKYValue* val);