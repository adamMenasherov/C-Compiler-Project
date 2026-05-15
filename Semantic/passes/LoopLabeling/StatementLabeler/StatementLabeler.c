#include "StatementLabeler.h"
#include "../../../../Parser/generateUtils.h"
#include "../../../../DataStructures/DynamicArray/DynamicArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void handleLabelIf(CStatement* stmt, char* currentLabel) {
    labelStatement(stmt->stmt.if_stmt->then, currentLabel);
    if (stmt->stmt.if_stmt->else_stmt) {
        labelStatement(stmt->stmt.if_stmt->else_stmt, currentLabel);
    }
}

static void handleLabelCompound(CStatement* stmt, char* currentLabel) {
    CBlock* block = stmt->stmt.compound_stmt->block;
    if (!block) return;
    DArray_forEach(block->items, elem,
    {
        CBlockItem* blockItem = (CBlockItem*)elem;
        if (blockItem && blockItem->type == BLOCK_ITEM_STMT) {
            labelStatement(blockItem->item.stmt, currentLabel);
        }
    });
}

static void handleLabelFor(CStatement* stmt, char* currentLabel) {
    char* loopLabel = generateLoopName();
    stmt->stmt.for_stmt->identifier = strdup(loopLabel);
    labelStatement(stmt->stmt.for_stmt->body, loopLabel);
}

static void handleLabelWhile(CStatement* stmt, char* currentLabel) {
    char* whileLabel = generateLoopName();
    stmt->stmt.while_stmt->identifier = strdup(whileLabel);
    labelStatement(stmt->stmt.while_stmt->body, whileLabel);
}

static void handleLabelDoWhile(CStatement* stmt, char* currentLabel) {
    char* doWhileLabel = generateLoopName();
    stmt->stmt.do_while_stmt->identifier = strdup(doWhileLabel);
    labelStatement(stmt->stmt.do_while_stmt->body, doWhileLabel);
}

static void handleLabelBreak(CStatement* stmt, char* currentLabel) {
    if (!currentLabel) {
        fprintf(stderr, "Semantic Error: 'break' statement not within a loop\n");
        exit(1);
    }
    stmt->stmt.break_stmt->identifier = strdup(currentLabel);
}

static void handleLabelContinue(CStatement* stmt, char* currentLabel) {
    if (!currentLabel) {
        fprintf(stderr, "Semantic Error: 'continue' statement not within a loop\n");
        exit(1);
    }
    stmt->stmt.continue_stmt->identifier = strdup(currentLabel);
}

typedef void (*StmtLabeler)(CStatement*, char*);

static const StmtLabeler stmtLabelers[] = {
    [STMT_IF]       = handleLabelIf,
    [STMT_COMPOUND] = handleLabelCompound,
    [STMT_FOR]      = handleLabelFor,
    [STMT_WHILE]    = handleLabelWhile,
    [STMT_DO_WHILE] = handleLabelDoWhile,
    [STMT_BREAK]    = handleLabelBreak,
    [STMT_CONTINUE] = handleLabelContinue,
};

void labelStatement(CStatement* stmt, char* currentLabel) {
    if (!stmt) return;
    if (stmt->type >= sizeof(stmtLabelers) / sizeof(stmtLabelers[0])) return;
    StmtLabeler handler = stmtLabelers[stmt->type];
    if (!handler) return;
    handler(stmt, currentLabel);
}
