#pragma once
#include "../../C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "../../../../../Lexer/Tokens/tokenList.h"
#include "../../../../../DataStructures/DynamicArray/Wrappers/CDeclarationWrapper.h"


CDeclaration* C_parseVarDeclaration(TokenList* tokens, char* identifier, specifierType varType, specifierType storageClass);
CDeclaration* C_parseDeclaration(TokenList* tokens);
CDeclarationArray* C_parseDeclarations(TokenList* tokens);
IdentifierArray* C_parseFuncParameters(TokenList* tokens, CFuncType* funcType);
CDeclaration* C_parseFunction(TokenList* tokens, char* identifier, specifierType funcType, specifierType storageClass);
void C_parseTypeAndStorageClass(TokenList* tokens, specifierType* type, specifierType* storageClass);
specifierType C_parseType(TypeFlags typeFound);