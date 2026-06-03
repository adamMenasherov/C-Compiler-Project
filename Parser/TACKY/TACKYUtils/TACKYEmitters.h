#pragma once

/**
 * @file TACKYEmitters.h
 * @brief This file handles the emission of TACKY instruction for CFactors.
 * It deals with the various types of CFactor, having emit_TACKY as the entry point and routing from it to the 
 * appropriate function. 
 */

#include "../TACKYProgram.h"

typedef enum {
    EXP_RESULT_PLAIN_OP,
    EXP_RESULT_DEREF_POINTER_OP
} expResultType;

typedef struct {
    expResultType type;
    TACKYValue* val;
} ExpResult;

ExpResult* emit_TACKY(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary, SymbolTable* symTable);
ExpResult* emit_TACKYUnary(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary, SymbolTable* symTable);
ExpResult* emit_TACKYBinary(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);
ExpResult* emit_TACKYAssignment(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);
ExpResult* emit_TACKYConditional(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);
ExpResult* emit_TACKYFunctionCall(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);
ExpResult* emit_TACKYCast(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);
TACKYValue* emit_TACKYAndConvert(CFactor* val, TACKYInstructionList* instruction_list, SymbolTable* symTable);
TACKYInstruction* emitUnaryPostfixInstruction(CFactor* exp, SymbolTable* symTable);
ExpResult* shortCircuitTACKYInstruction(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable);