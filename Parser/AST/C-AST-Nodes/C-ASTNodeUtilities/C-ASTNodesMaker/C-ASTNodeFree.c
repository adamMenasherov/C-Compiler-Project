#include "C-ASTNodeFree.h"


void C_freeProgram(CProgram* prog) {
    if (prog) {
        C_freeFunction(prog->function_def);
        free(prog);
    }
}
void C_freeFunction(CFunction* func) {
    if (func) {
        free(func->function_name);
        C_freeReturn(func->body);
        free(func);
    }
}
void C_freeReturn(CReturn* returnNode) {
    if (returnNode) {
        C_freeConstant(returnNode->exp);
        free(returnNode);
    }
}
void C_freeConstant(CConstant* constant) {
    if (constant) {
        free(constant);
    }
}
