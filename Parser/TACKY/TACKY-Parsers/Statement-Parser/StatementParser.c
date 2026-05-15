#include "StatementParser.h"
#include "../Block-Parser/BlockParser.h"
#include "../If-Parser/ifParser.h"
#include "../Loop-Parser/LoopParser.h"
#include "../../TACKYUtils/TACKYEmitters.h"
#include "../../TACKYUtils/TACKYConstructors.h"
#include "../../../generateUtils.h"

static void handleStmtExpression(CStatement* stmt, TACKYInstructionList* list) {
    int isPostfix = 0;
    emit_TACKY(stmt->stmt.exp, list, &isPostfix);
    if (isPostfix) addInstructionToList(list, emitUnaryPostfixInstruction(stmt->stmt.exp));
}

static void handleStmtReturn(CStatement* stmt, TACKYInstructionList* list) {
    parseTACKYReturn(stmt->stmt.ret, list);
}

static void handleStmtBreak(CStatement* stmt, TACKYInstructionList* list) {
    addInstructionToList(list,
        createJumpInstruction(TACKY_JUMP,
            generateBreakLabelFromLoopLabel(stmt->stmt.break_stmt->identifier), NULL));
}

static void handleStmtContinue(CStatement* stmt, TACKYInstructionList* list) {
    addInstructionToList(list,
        createJumpInstruction(TACKY_JUMP,
            generateContinueLabelFromLoopLabel(stmt->stmt.continue_stmt->identifier), NULL));
}

static void handleStmtIf(CStatement* stmt, TACKYInstructionList* list) {
    parseIfStatementInstructions(stmt->stmt.if_stmt, list);
}

static void handleStmtCompound(CStatement* stmt, TACKYInstructionList* list) {
    parseBlock(stmt->stmt.compound_stmt->block, list);
}

static void handleStmtWhile(CStatement* stmt, TACKYInstructionList* list) {
    parseWhileLoopInstructions(stmt->stmt.while_stmt, list);
}

static void handleStmtDoWhile(CStatement* stmt, TACKYInstructionList* list) {
    parseDoWhileLoopInstructions(stmt->stmt.do_while_stmt, list);
}

static void handleStmtFor(CStatement* stmt, TACKYInstructionList* list) {
    parseForLoopInstructions(stmt->stmt.for_stmt, list);
}

static void handleSwitchStatement(CStatement* stmt, TACKYInstructionList* list) {
    CSwitch* switch_stmt = stmt->stmt.switch_stmt;
    int isPostfixUnary = 0;
    TACKYValue* switchExp = emit_TACKY(switch_stmt->switchExp, list, &isPostfixUnary);
    if (isPostfixUnary) addInstructionToList(list, emitUnaryPostfixInstruction(switch_stmt->switchExp));
    parseSwitchStatementInstructions(switch_stmt, list, switchExp);
}

typedef void (*StmtHandler)(CStatement*, TACKYInstructionList*);
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

void parseStatementInstructions(CStatement* stmt, TACKYInstructionList* list) {
    if (!stmt) return;
    if (stmt->type == STMT_NULL) return;
    stmtHandlers[stmt->type](stmt, list);
}


void parseTACKYReturn(CReturn* returnNode, TACKYInstructionList* instructionList) {
    TACKYValue* ret_val = emit_TACKY(returnNode->exp, instructionList, NULL);
    TACKYInstruction* ret_inst = createReturnInstruction(ret_val);
    addInstructionToList(instructionList, ret_inst);
}