#pragma once
#include "../../TACKY_AST.h"


/**
 * @brief Converts symbol table entries for global variables into TACKY top-level variable declarations, adding them to the provided top-level array.
 * 
 * @param symbol The symbol table entry to convert, containing information about the identifier, its type, and attributes
 * @param userData A pointer to the TACKYTopLevelArray to which generated top-level items are appended
 */ 
void convertSymbolsToTACKY(IdentifierTypeInfo* symbol, void* userData);