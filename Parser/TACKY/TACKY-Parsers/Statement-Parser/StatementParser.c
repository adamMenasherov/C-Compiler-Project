#include "StatementParser.h"
#include "../Block-Parser/BlockParser.h"
#include "../If-Parser/ifParser.h"
#include "../Loop-Parser/LoopParser.h"
#include "../../TACKYUtils/TACKYEmitters.h"
#include "../../TACKYUtils/TACKYConstructors.h"
#include "../../../generateUtils.h"

static void handleStmtExpression(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    int isPostfix = 0;
    emit_TACKY(stmt->stmt.exp, list, &isPostfix, symTable);
    if (isPostfix) addInstructionToList(list, emitUnaryPostfixInstruction(stmt->stmt.exp, symTable));
}

static void handleStmtReturn(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    parseTACKYReturn(stmt->stmt.ret, list, symTable);
}

static void handleStmtBreak(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    (void)symTable;
    addInstructionToList(list,
        createJumpInstruction(TACKY_JUMP,
            generateBreakLabelFromLoopLabel(stmt->stmt.break_stmt->identifier), NULL));
}

static void handleStmtContinue(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    (void)symTable;
    addInstructionToList(list,
        createJumpInstruction(TACKY_JUMP,
            generateContinueLabelFromLoopLabel(stmt->stmt.continue_stmt->identifier), NULL));
}

static void handleStmtIf(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    parseIfStatementInstructions(stmt->stmt.if_stmt, list, symTable);
}

static void handleStmtCompound(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    parseBlock(stmt->stmt.compound_stmt->block, list, symTable);
}

static void handleStmtWhile(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    parseWhileLoopInstructions(stmt->stmt.while_stmt, list, symTable);
}

static void handleStmtDoWhile(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    parseDoWhileLoopInstructions(stmt->stmt.do_while_stmt, list, symTable);
}

static void handleStmtFor(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    parseForLoopInstructions(stmt->stmt.for_stmt, list, symTable);
}

static void handleSwitchStatement(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    CSwitch* switch_stmt = stmt->stmt.switch_stmt;
    int isPostfixUnary = 0;
    TACKYValue* switchExp = emit_TACKY(switch_stmt->switchExp, list, &isPostfixUnary, symTable);
    if (isPostfixUnary) addInstructionToList(list, emitUnaryPostfixInstruction(switch_stmt->switchExp, symTable));
    parseSwitchStatementInstructions(switch_stmt, list, switchExp, symTable);
}

typedef void (*StmtHandler)(CStatement*, TACKYInstructionList*, SymbolTable*);
static const StmtHandler stmtHandlers[] = {
    [STMT_EXPRESSION] = handleStmtExpression,
    [STMT_RETURN]     = handleStmtReturn,
    [STMT_BREAK]      = handleStmtBreak,
    [STMT_CONTINUE]   = handleStmtContinue,
    [STMT_SWITCH]     = handleSwitchStatement,
    [STMT_IF]         = handleStmtIf,
    [STMT_COMPOUND]   = handleStmtCompound,
    [STMT_WHILE]      = handleStmtWhile,
    [STMT_DO_WHILE]   = handleStmtDoWhile,
    [STMT_FOR]        = handleStmtFor,
};

void parseStatementInstructions(CStatement* stmt, TACKYInstructionList* list, SymbolTable* symTable) {
    if (!stmt) return;
    if (stmt->type == STMT_NULL) return;
    stmtHandlers[stmt->type](stmt, list, symTable);
}


void parseTACKYReturn(CReturn* returnNode, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    TACKYValue* ret_val = emit_TACKY(returnNode->exp, instructionList, NULL, symTable);
    TACKYInstruction* ret_inst = createReturnInstruction(ret_val);
    addInstructionToList(instructionList, ret_inst);
}