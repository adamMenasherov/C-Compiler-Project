#pragma once

#include "../TACKYProgram.h"

TACKYValue* emit_TACKY(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary);
TACKYValue* emit_TACKYUnary(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary);
TACKYValue* emit_TACKYBinary(CFactor* exp, TACKYInstructionList* instruction_list);
TACKYValue* emit_TACKYAssignment(CFactor* exp, TACKYInstructionList* instruction_list);
TACKYValue* emit_TACKYConditional(CFactor* exp, TACKYInstructionList* instruction_list);
TACKYValue* emit_TACKYFunctionCall(CFactor* exp, TACKYInstructionList* instruction_list);

TACKYInstruction* emitUnaryPostfixInstruction(CFactor* exp);
TACKYValue* shortCircuitTACKYInstruction(CFactor* exp, TACKYInstructionList* instruction_list);