#pragma once
#include "../../C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "../../../../../Lexer/Tokens/tokenList.h"
#include "../../../../../DataStructures/DynamicArray/Wrappers/CDeclarationWrapper.h"


CDeclaration* C_parseVarDeclaration(TokenList* tokens, char* identifier, CType* varType, StorageClass storageClass);
CDeclaration* C_parseDeclaration(TokenList* tokens);
CDeclarationArray* C_parseDeclarations(TokenList* tokens);
void processDeclarator(CDeclarator* decl, CType* baseType, CType** declType, char** identifier,
    IdentifierArray* funcParams);
IdentifierArray* C_parseFuncParameters(TokenList* tokens, CType* funcType);
CDeclaration* C_parseFunction(TokenList* tokens, char* identifier, CType* funcType,
                              IdentifierArray* parameters, StorageClass storageClass);
void C_parseTypeAndStorageClass(TokenList* tokens, CType** type, StorageClass* storageClass);
CType* C_parseType(TypeFlags typeFound);
CDeclarator* C_parseDeclarator(TokenList* tokens);
