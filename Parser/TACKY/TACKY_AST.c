#include "TACKY_AST.h"
#include "TACKY-Parsers/ParserInclude.h"

TACKYFunction* parseTACKYFunction(CDeclaration* func, SymbolTable* symTable) {
    if (func->type != DECL_FUNC) return NULL;
    if (!func->decl.functionDecl.body) return NULL;
    int global = isGlobalFunction(func, symTable);
    TACKYInstructionList* list = createTACKYInstructionList();
    parseBlock(func->decl.functionDecl.body, list);
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