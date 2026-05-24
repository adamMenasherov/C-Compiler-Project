#pragma once

#include "../TACKYProgram.h"

TACKYValue* emit_TACKY(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary, SymbolTable* symTable);
TACKYValue* emit_TACKYUnary(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary, SymbolTable* symTable);
TACKYValue* emit_TACKYBinary(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);
TACKYValue* emit_TACKYAssignment(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);
TACKYValue* emit_TACKYConditional(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);
TACKYValue* emit_TACKYFunctionCall(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);
TACKYValue* emit_TACKYCast(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);

TACKYInstruction* emitUnaryPostfixInstruction(CFactor* exp, SymbolTable* symTable);
TACKYValue* shortCircuitTACKYInstruction(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);