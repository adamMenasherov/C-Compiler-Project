#include "TACKYProgram.h"
#include "TACKY-Parsers/ParserInclude.h"
#include "TACKYUtils/TACKYConstructors.h"
#include "../generateUtils.h"
#include "../../Semantic/utils/SemanticUtils/SemanticUtils.h"

TACKYFunction* parseTACKYFunction(CDeclaration* func, SymbolTable* symTable) {
    if (func->type != DECL_FUNC) return NULL;
    if (!func->decl.functionDecl.body) return NULL;
    int global = isGlobalFunction(func, symTable);
    TACKYInstructionList* list = createTACKYInstructionList();
    parseBlock(func->decl.functionDecl.body, list, symTable);
    return createTACKYFunction(func->decl.functionDecl.identifier,
        func->decl.functionDecl.parameters, list, global);
}

TACKYProgram* parseTACKYProgram(CProgram* program, SymbolTable* symTable) {
    TACKYProgram* tackyProg = createTACKYProgram();
    if (!tackyProg) return NULL;
    for (int i = 0; i < CDeclarationArray_size(program->function_def); i++) {
        CDeclaration* decl = CDeclarationArray_get(program->function_def, i);
        if (!decl) continue;
        TACKYTopLevel* topLevel = createTACKYTopLevelFromFunction(
            parseTACKYFunction(decl, symTable), symTable);
        if (!topLevel) continue;
        TACKYTopLevelArray_append(tackyProg->topLevels, topLevel);
    }
    symbolTableForEach(symTable, convertSymbolsToTACKY, tackyProg->topLevels);
    return tackyProg;
}

TACKYValue* makeTACKYVariable(specifierType type, SymbolTable* symTable) {
    char* temp_name = generateTempName();
    identifierAttrs* attrs = createIdentifierAttrs(IDENTIFIER_LOCAL_ATTR, 0, NULL, 1);
    symbolTableInsert(symTable, temp_name, specifierTypeToIdentifierType(type), NULL, 0, attrs);
    TACKYValue* val = createVarValue(temp_name);
    free(temp_name);
    return val;
}