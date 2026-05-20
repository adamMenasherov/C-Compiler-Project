#pragma once
#include "../../TACKYProgram.h"

void parseStatementInstructions(CStatement* stmt, TACKYInstructionList* list);

/**
 * Converts a C AST return statement into a TACKY return representation.
 * @param returnNode The C AST return statement to convert
 * @return A pointer to the generated TACKYReturn
 */
void parseTACKYReturn(CReturn* returnNode, TACKYInstructionList* instructionList);