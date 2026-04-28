#include "ASM-ASTNodes.h"
#include "ASM-ASTNodesUtilities/ASM-ASTNodesConstructors.h"
#include "ASM-ASTNodesUtilities/ASM-ASTNodesFree.h"
#include <stdlib.h>
#include <string.h>
#include "../../../ASM-File-Generation/ASM_AST_fix.h"
#include "../../../Parser/TACKY/TACKYUtils/TACKYConstructors.h"


ASMProgram* parseASMprogram(TACKYProgram* tacky_prog, SymbolTable* symTable) {
    ASMProgram* asm_prog = malloc(sizeof(ASMProgram));
    if (!asm_prog) return NULL;
    asm_prog->topLevels = ASMTopLevelArray_create();
    if (!asm_prog->topLevels) {
        free(asm_prog);
        return NULL;
    }

    for (int i = 0; i < TACKYTopLevelArray_size(tacky_prog->topLevels); i++) {
        TACKYTopLevel* topLevel = TACKYTopLevelArray_get(tacky_prog->topLevels, i);
        if (!topLevel) continue;

        switch(topLevel->type) {
            case TACKY_STATIC_VAR: {
                ASMTopLevel* asmTopLevel = createASMStaticVarFromTACKYStaticVar(topLevel->topLevel.staticVar);
                if (!asmTopLevel) continue;
                ASMTopLevelArray_append(asm_prog->topLevels, asmTopLevel);
                break;
            }
            case TACKY_FUNC: {
                ASMTopLevel* asmTopLevel = createASMTopLevelFromASMFunction(parseASMfunction(topLevel->topLevel.function, symTable));
                if (!asmTopLevel) continue;
                ASMTopLevelArray_append(asm_prog->topLevels, asmTopLevel);
                break;
            }
            default: break;
        }
    }
    return asm_prog;
}

ASMFunction* parseASMfunction(TACKYFunction* tacky_func, SymbolTable* symTable) {
    ASMFunction* asm_func = malloc(sizeof(ASMFunction));
    if (!asm_func) return NULL;
    
    asm_func->function_name = strdup(tacky_func->function_name);
    if (!asm_func->function_name) {
        free(asm_func);
        return NULL;
    }
    asm_func->inst = createASMInstructionList();
    asm_func->pseudoTable = createCharIntMap();
    asm_func->global = tacky_func->global;

    if (!asm_func->inst || !asm_func->pseudoTable) {
        free(asm_func->function_name);
        free(asm_func->inst);
        free(asm_func->pseudoTable);
        free(asm_func);
        return NULL;
    }

    addArgsAsInstructionsToFunc(tacky_func->parameters, asm_func->inst, symTable);
    while (InstructionArray_getCursor(tacky_func->instruction_list) < InstructionArray_size(tacky_func->instruction_list)) {
        parseASMInstruction(tacky_func->instruction_list, asm_func->inst, symTable);
    }
    addASMInstructionAtEnd(
        asm_func->inst,
        createMovInstruction(createImmediateOperand(0), createRegisterOperand(AX))
    );
    addASMInstructionAtEnd(asm_func->inst, createASMReturnInstruction());
    pseudoToStackPositions(asm_func->inst, asm_func->pseudoTable);
    
    return asm_func;
}

void parseASMReturn(TACKYValue* tacky_ret, ASMInstructionList* instruction_list, SymbolTable* symTable) {
    ASMInstruction* mov_inst = createMovInstruction
                        (tackyValueToASMOperand(tacky_ret, symTable), createRegisterOperand(AX));
    addASMInstructionAtEnd(instruction_list, mov_inst);

    ASMInstruction* ret_inst = createASMReturnInstruction();
    addASMInstructionAtEnd(instruction_list, ret_inst);
}

void addArgsAsInstructionsToFunc(IdentifierArray* params, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    int lenArgs = IdentifierArray_size(params);
    for (int i = 0; i < lenArgs && i < 6; i++) {
        char* arg = IdentifierArray_get(params, i);
        ASMInstruction* mov_arg_inst = createMovInstruction(
            createRegisterOperand(argResigters[i]),
            tackyValueToASMOperand(createVarValue(arg), symTable));
        addASMInstructionAtEnd(asmInstructionList, mov_arg_inst);
    }
    int stackPos = 16;
    for (int i = 6; i < lenArgs; i++) {
        char* arg = IdentifierArray_get(params, i);
        ASMInstruction* movInst = createMovInstruction(
            createStackOperand(stackPos), 
            tackyValueToASMOperand(createVarValue(arg), symTable)); 
        addASMInstructionAtEnd(asmInstructionList, movInst);
        stackPos += 8;
    }
}


void parseASMInstruction(TACKYInstructionList* tackyInstList, ASMInstructionList* asmInstructionList, SymbolTable* symTable) 
{
    if (InstructionArray_getCursor(tackyInstList) >= InstructionArray_size(tackyInstList)) return; // No more instructions
    TACKYInstruction* instruction = InstructionArray_nextAndGet(tackyInstList);
    if (!instruction) return;
    switch(instruction->type) {
        case TACKY_UNARY: {
            parseASMUnaryInstruction(instruction, asmInstructionList, symTable);
            break;
        }
        case TACKY_BINARY: {
            parseASMBinaryInstruction(instruction, asmInstructionList, symTable);
            break;
        }
        case TACKY_JUMP: {
            ASMInstruction* jmp_inst = createASMJumpInstruction(instruction->instValue.jump.label);
            addASMInstructionAtEnd(asmInstructionList, jmp_inst);
            break;
        }
        case TACKY_LABEL: {
            ASMInstruction* label_inst = createASMLabelInstruction(instruction->instValue.label.label);
            addASMInstructionAtEnd(asmInstructionList, label_inst);
            break;
        }
        case TACKY_COPY: {
            ASMInstruction* mov_inst = createMovInstruction(
                tackyValueToASMOperand(instruction->instValue.copy.src, symTable), 
                tackyValueToASMOperand(instruction->instValue.copy.dest, symTable));
            addASMInstructionAtEnd(asmInstructionList, mov_inst);
            break;
        }

        case TACKY_RETURN: {
            parseASMReturn(instruction->instValue.returnVal.retVal, asmInstructionList, symTable);
            break;
        }
        case TACKY_JUMP_IF_ZERO:
        case TACKY_JUMP_IF_NOT_ZERO: {
            parseCondJumpInstruction(instruction, asmInstructionList, symTable);
            break;
        }
        case TACKY_FUNCTION_CALL: {
            parseFunctionCallInstruction(instruction, asmInstructionList, symTable);
            break;
        }
        default: break; // Handle other instruction types as needed
    }
}

void parseFunctionCallInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    int lenArgs = TACKYValueArray_size(instruction->instValue.funCall.args);
    int stackArgs = (lenArgs > 6) ? (lenArgs - 6) : 0;
    int stackPadding = (stackArgs % 2) ? 8 : 0; 
    if (stackPadding) {
        addASMInstructionAtEnd(asmInstructionList, 
            createASMAllocateStackInstruction(stackPadding));
    }

    for (int i = 0; i < lenArgs && i < 6; i++) {
        TACKYValue* arg = TACKYValueArray_get(instruction->instValue.funCall.args, i);
        ASMInstruction* mov_arg_inst = createMovInstruction(
            tackyValueToASMOperand(arg, symTable), 
            createRegisterOperand(argResigters[i]));
        addASMInstructionAtEnd(asmInstructionList, mov_arg_inst);
    }

    for (int i = lenArgs - 1; i >= 6; i--) {
        TACKYValue* arg = TACKYValueArray_get(instruction->instValue.funCall.args, i);
        ASMOperand* argOp = tackyValueToASMOperand(arg, symTable);
        if (argOp->type == ASM_OP_IMMEDIATE) {
            ASMInstruction* push_inst = createASMPushInstruction(argOp);
            addASMInstructionAtEnd(asmInstructionList, push_inst);
        } 
        else {
            ASMInstruction* mov_arg_inst = createMovInstruction(
                argOp, 
                createRegisterOperand(AX)); // Use AX as temp register for pushing
            addASMInstructionAtEnd(asmInstructionList, mov_arg_inst);

            ASMInstruction* push_inst = createASMPushInstruction(createRegisterOperand(AX));
            addASMInstructionAtEnd(asmInstructionList, push_inst);
        }
    }

    addASMInstructionAtEnd(asmInstructionList, 
        createASMCallInstruction(instruction->instValue.funCall.functionName));
    
    int bytesToRemove = 8 * stackArgs + stackPadding;
    if (bytesToRemove) {
        addASMInstructionAtEnd(asmInstructionList, 
            createASMDeallocateStackInstruction(bytesToRemove));
    }
    ASMOperand* retDest = tackyValueToASMOperand(instruction->instValue.funCall.resultVar, symTable);
    addASMInstructionAtEnd(asmInstructionList, 
        createMovInstruction(createRegisterOperand(AX), retDest)); 
}


void parseCondJumpInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMCondCode cond = (instruction->type == TACKY_JUMP_IF_ZERO) ? ASM_COND_CODE_E : ASM_COND_CODE_NE;
    ASMInstruction* cmp_inst = createASMCmpInstruction(
        createImmediateOperand(0),
        tackyValueToASMOperand(instruction->instValue.condJump.condition, symTable)
    );
    addASMInstructionAtEnd(asmInstructionList, cmp_inst);

    ASMInstruction* jmpcc_inst = createASMJumpCCInstruction(cond, instruction->instValue.condJump.label);
    addASMInstructionAtEnd(asmInstructionList, jmpcc_inst);
}


void handleUnaryNot(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    // For NOT operation, move source to destination, then apply NOT
    ASMInstruction* cmp_inst = createASMCmpInstruction(
        createImmediateOperand(0),
        tackyValueToASMOperand(instruction->instValue.unaryOp.src, symTable)
    );
    addASMInstructionAtEnd(asmInstructionList, cmp_inst);

    ASMInstruction* mov_inst = createMovInstruction(
        createImmediateOperand(0),
        tackyValueToASMOperand(instruction->instValue.unaryOp.dest, symTable)
    );
    addASMInstructionAtEnd(asmInstructionList, mov_inst);

    ASMInstruction* setcc_inst = createASMSetCCInstruction(ASM_COND_CODE_E, 
                                tackyValueToASMOperand(instruction->instValue.unaryOp.dest, symTable));
    addASMInstructionAtEnd(asmInstructionList, setcc_inst);
}


void parseASMUnaryInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    if (instruction->type != TACKY_UNARY) return; 
    if (instruction->instValue.unaryOp.type == UNARY_NOT) {
        handleUnaryNot(instruction, asmInstructionList, symTable);
        return;
    }

    if (isIncrementDecrementOp(instruction->instValue.unaryOp.type)) {
        ASMInstruction* unary_inst = createASMUnaryInstruction(
            instruction->instValue.unaryOp.type,
            instruction->instValue.unaryOp.src,
            symTable);
        addASMInstructionAtEnd(asmInstructionList, unary_inst);

        ASMInstruction* mov_inst = createMovInstruction(
            tackyValueToASMOperand(instruction->instValue.unaryOp.src, symTable),
            tackyValueToASMOperand(instruction->instValue.unaryOp.dest, symTable));
        addASMInstructionAtEnd(asmInstructionList, mov_inst);
    } else {
        ASMInstruction* mov_inst = createMovInstruction(
            tackyValueToASMOperand(instruction->instValue.unaryOp.src, symTable), 
            tackyValueToASMOperand(instruction->instValue.unaryOp.dest, symTable));
        addASMInstructionAtEnd(asmInstructionList, mov_inst);

        ASMInstruction* unary_inst = createASMUnaryInstruction(
            instruction->instValue.unaryOp.type, 
            instruction->instValue.unaryOp.dest,
            symTable);
        addASMInstructionAtEnd(asmInstructionList, unary_inst);
    }
}

void parseASMBinaryInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    if (instruction->instValue.binaryOp.binaryOpType == BIN_DIVIDE 
                    || instruction->instValue.binaryOp.binaryOpType == BIN_REMAINDER) {
        handleDivideModuloCase(instruction, asmInstructionList, symTable);
        return;
    }
    if (isRelationalOp(instruction->instValue.binaryOp.binaryOpType)) {
        handleBinaryRelationalOp(instruction, asmInstructionList, symTable);
        return;
    }

    ASMInstruction* mov_inst = createMovInstruction(
        tackyValueToASMOperand(instruction->instValue.binaryOp.src1, symTable), 
        tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable));
    addASMInstructionAtEnd(asmInstructionList, mov_inst);
    
    ASMInstruction* binary_inst = createASMBinaryInstruction(
        instruction->instValue.binaryOp.binaryOpType,
        tackyValueToASMOperand(instruction->instValue.binaryOp.src2, symTable),
        tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable)
    );
    addASMInstructionAtEnd(asmInstructionList, binary_inst);
}


void handleBinaryRelationalOp(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMInstruction* cmp_inst = createASMCmpInstruction(
        tackyValueToASMOperand(instruction->instValue.binaryOp.src2, symTable),
        tackyValueToASMOperand(instruction->instValue.binaryOp.src1, symTable)
    );
    addASMInstructionAtEnd(asmInstructionList, cmp_inst);

    ASMInstruction* movInst = createMovInstruction(
        createImmediateOperand(0),
        tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable)
    );
    addASMInstructionAtEnd(asmInstructionList, movInst);

    ASMCondCode cond = getCondCodeForRelationalOp(instruction->instValue.binaryOp.binaryOpType);
    ASMInstruction* setcc_inst = createASMSetCCInstruction(cond, 
                                tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable));
    addASMInstructionAtEnd(asmInstructionList, setcc_inst);

}

void handleDivideModuloCase(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMInstruction* mov_inst = createMovInstruction(
                tackyValueToASMOperand(instruction->instValue.binaryOp.src1, symTable), 
                createRegisterOperand(AX));
    addASMInstructionAtEnd(asmInstructionList, mov_inst);


    ASMInstruction* cdq_inst = calloc(1, sizeof(ASMInstruction));
    if (!cdq_inst) return;
    cdq_inst->type = ASM_CDQ;
    addASMInstructionAtEnd(asmInstructionList, cdq_inst);

    // Create IDIV instruction
    ASMInstruction* idiv_inst = createIdivInstruction(
        tackyValueToASMOperand(instruction->instValue.binaryOp.src2, symTable));
    addASMInstructionAtEnd(asmInstructionList, idiv_inst);

    if (instruction->instValue.binaryOp.binaryOpType == BIN_REMAINDER) {
        // Move remainder from RDX to destination for modulo
        ASMInstruction* mov_modulo_result = createMovInstruction(
            createRegisterOperand(DX), 
            tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable));
        addASMInstructionAtEnd(asmInstructionList, mov_modulo_result);
    } else {
        // Move quotient from RAX to destination for division
        ASMInstruction* mov_div_result = createMovInstruction(
            createRegisterOperand(AX), 
            tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable));
        addASMInstructionAtEnd(asmInstructionList, mov_div_result);
    }
}

ASMTopLevel* createASMStaticVarFromTACKYStaticVar(TACKYStaticVar* tackyStaticVar) {
    ASMStaticVar* asmStaticVar = malloc(sizeof(ASMStaticVar));
    if (!asmStaticVar) return NULL;

    asmStaticVar->identifier = strdup(tackyStaticVar->identifier);
    if (!asmStaticVar->identifier) {
        free(asmStaticVar);
        return NULL;
    }
    asmStaticVar->global = tackyStaticVar->global;
    asmStaticVar->init = tackyStaticVar->init;

    return createTopLevel(ASM_TOP_LEVEL_STATIC_VAR, asmStaticVar);
}