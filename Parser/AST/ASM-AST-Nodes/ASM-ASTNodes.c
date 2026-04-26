#include "ASM-ASTNodes.h"
#include "ASM-ASTNodesUtilities/ASM-ASTNodesConstructors.h"
#include "ASM-ASTNodesUtilities/ASM-ASTNodesFree.h"
#include <stdlib.h>
#include <string.h>
#include "../../../ASM-File-Generation/ASM_AST_fix.h"
#include "../../../Parser/TACKY/TACKYUtils/TACKYConstructors.h"

ASMProgram* parseASMprogram(TACKYProgram* tacky_prog) {
    ASMProgram* asm_prog = malloc(sizeof(ASMProgram));
    if (!asm_prog) return NULL;
    asm_prog->function_defs = ASMFunctionArray_create();

    for (int i = 0; i < TACKYTopLevelArray_size(tacky_prog->topLevels); i++) {
        TACKYTopLevel* topLevel = TACKYTopLevelArray_get(tacky_prog->topLevels, i);
        if (!topLevel || topLevel->type != TACKY_FUNC) continue;

        TACKYFunction* tacky_func = topLevel->topLevel.function;
        ASMFunction* asm_func = parseASMfunction(tacky_func);
        if (!asm_func) {
            freeASMProgram(asm_prog);
            return NULL;
        }
        ASMFunctionArray_append(asm_prog->function_defs, asm_func);
    }
    return asm_prog;
}

ASMFunction* parseASMfunction(TACKYFunction* tacky_func) {
    ASMFunction* asm_func = malloc(sizeof(ASMFunction));
    if (!asm_func) return NULL;
    
    asm_func->function_name = strdup(tacky_func->function_name);
    if (!asm_func->function_name) {
        free(asm_func);
        return NULL;
    }
    asm_func->inst = createASMInstructionList();
    asm_func->pseudoTable = createCharIntMap();

    if (!asm_func->inst || !asm_func->pseudoTable) {
        free(asm_func->function_name);
        free(asm_func->inst);
        free(asm_func->pseudoTable);
        free(asm_func);
        return NULL;
    }

    addArgsAsInstructionsToFunc(tacky_func->parameters, asm_func->inst);
    while (InstructionArray_getCursor(tacky_func->instruction_list) < InstructionArray_size(tacky_func->instruction_list)) {
        parseASMInstruction(tacky_func->instruction_list, asm_func->inst);
    }
    addASMInstructionAtEnd(
        asm_func->inst,
        createMovInstruction(createImmediateOperand(0), createRegisterOperand(AX))
    );
    addASMInstructionAtEnd(asm_func->inst, createASMReturnInstruction());
    pseudoToStackPositions(asm_func->inst, asm_func->pseudoTable);
    
    return asm_func;
}

void parseASMReturn(TACKYValue* tacky_ret, ASMInstructionList* instruction_list) {
    ASMInstruction* mov_inst = createMovInstruction
                        (tackyValueToASMOperand(tacky_ret), createRegisterOperand(AX));
    addASMInstructionAtEnd(instruction_list, mov_inst);

    ASMInstruction* ret_inst = createASMReturnInstruction();
    addASMInstructionAtEnd(instruction_list, ret_inst);
}

void addArgsAsInstructionsToFunc(IdentifierArray* params, ASMInstructionList* asmInstructionList) {
    int lenArgs = IdentifierArray_size(params);
    for (int i = 0; i < lenArgs && i < 6; i++) {
        char* arg = IdentifierArray_get(params, i);
        ASMInstruction* mov_arg_inst = createMovInstruction(
            createRegisterOperand(argResigters[i]),
            tackyValueToASMOperand(createVarValue(arg)));
        addASMInstructionAtEnd(asmInstructionList, mov_arg_inst);
    }
    int stackPos = 16;
    for (int i = 6; i < lenArgs; i++) {
        char* arg = IdentifierArray_get(params, i);
        ASMInstruction* movInst = createMovInstruction(
            createStackOperand(stackPos), 
            tackyValueToASMOperand(createVarValue(arg))); 
        addASMInstructionAtEnd(asmInstructionList, movInst);
        stackPos += 8;
    }
}


void parseASMInstruction(TACKYInstructionList* tackyInstList, ASMInstructionList* asmInstructionList) 
{
    if (InstructionArray_getCursor(tackyInstList) >= InstructionArray_size(tackyInstList)) return; // No more instructions
    TACKYInstruction* instruction = InstructionArray_nextAndGet(tackyInstList);
    if (!instruction) return;
    switch(instruction->type) {
        case TACKY_UNARY: {
            parseASMUnaryInstruction(instruction, asmInstructionList);
            break;
        }
        case TACKY_BINARY: {
            parseASMBinaryInstruction(instruction, asmInstructionList);
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
                tackyValueToASMOperand(instruction->instValue.copy.src), 
                tackyValueToASMOperand(instruction->instValue.copy.dest));
            addASMInstructionAtEnd(asmInstructionList, mov_inst);
            break;
        }

        case TACKY_RETURN: {
            parseASMReturn(instruction->instValue.returnVal.retVal, asmInstructionList);
            break;
        }
        case TACKY_JUMP_IF_ZERO:
        case TACKY_JUMP_IF_NOT_ZERO: {
            parseCondJumpInstruction(instruction, asmInstructionList);
            break;
        }
        case TACKY_FUNCTION_CALL: {
            parseFunctionCallInstruction(instruction, asmInstructionList);
            break;
        }
        default: break; // Handle other instruction types as needed
    }
}

void parseFunctionCallInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList) {
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
            tackyValueToASMOperand(arg), 
            createRegisterOperand(argResigters[i]));
        addASMInstructionAtEnd(asmInstructionList, mov_arg_inst);
    }

    for (int i = lenArgs - 1; i >= 6; i--) {
        TACKYValue* arg = TACKYValueArray_get(instruction->instValue.funCall.args, i);
        ASMOperand* argOp = tackyValueToASMOperand(arg);
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
    ASMOperand* retDest = tackyValueToASMOperand(instruction->instValue.funCall.resultVar);
    addASMInstructionAtEnd(asmInstructionList, 
        createMovInstruction(createRegisterOperand(AX), retDest)); 
}


void parseCondJumpInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList) {
    ASMCondCode cond = (instruction->type == TACKY_JUMP_IF_ZERO) ? ASM_COND_CODE_E : ASM_COND_CODE_NE;
    ASMInstruction* cmp_inst = createASMCmpInstruction(
        createImmediateOperand(0),
        tackyValueToASMOperand(instruction->instValue.condJump.condition)
    );
    addASMInstructionAtEnd(asmInstructionList, cmp_inst);

    ASMInstruction* jmpcc_inst = createASMJumpCCInstruction(cond, instruction->instValue.condJump.label);
    addASMInstructionAtEnd(asmInstructionList, jmpcc_inst);
}


void handleUnaryNot(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList) {
    // For NOT operation, move source to destination, then apply NOT
    ASMInstruction* cmp_inst = createASMCmpInstruction(
        createImmediateOperand(0),
        tackyValueToASMOperand(instruction->instValue.unaryOp.src)
    );
    addASMInstructionAtEnd(asmInstructionList, cmp_inst);

    ASMInstruction* mov_inst = createMovInstruction(
        createImmediateOperand(0),
        tackyValueToASMOperand(instruction->instValue.unaryOp.dest)
    );
    addASMInstructionAtEnd(asmInstructionList, mov_inst);

    ASMInstruction* setcc_inst = createASMSetCCInstruction(ASM_COND_CODE_E, 
                                tackyValueToASMOperand(instruction->instValue.unaryOp.dest));
    addASMInstructionAtEnd(asmInstructionList, setcc_inst);
}


void parseASMUnaryInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList) {
    if (instruction->type != TACKY_UNARY) return; 
    if (instruction->instValue.unaryOp.type == UNARY_NOT) {
        handleUnaryNot(instruction, asmInstructionList);
        return;
    }

    if (isIncrementDecrementOp(instruction->instValue.unaryOp.type)) {
        ASMInstruction* unary_inst = createASMUnaryInstruction(
            instruction->instValue.unaryOp.type,
            instruction->instValue.unaryOp.src);  
        addASMInstructionAtEnd(asmInstructionList, unary_inst);

        ASMInstruction* mov_inst = createMovInstruction(
            tackyValueToASMOperand(instruction->instValue.unaryOp.src),
            tackyValueToASMOperand(instruction->instValue.unaryOp.dest));
        addASMInstructionAtEnd(asmInstructionList, mov_inst);
    } else {
        ASMInstruction* mov_inst = createMovInstruction(
            tackyValueToASMOperand(instruction->instValue.unaryOp.src), 
            tackyValueToASMOperand(instruction->instValue.unaryOp.dest));
        addASMInstructionAtEnd(asmInstructionList, mov_inst);

        ASMInstruction* unary_inst = createASMUnaryInstruction(
            instruction->instValue.unaryOp.type, 
            instruction->instValue.unaryOp.dest);
        addASMInstructionAtEnd(asmInstructionList, unary_inst);
    }
}

void parseASMBinaryInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList) {
    if (instruction->instValue.binaryOp.binaryOpType == BIN_DIVIDE 
                    || instruction->instValue.binaryOp.binaryOpType == BIN_REMAINDER) {
        handleDivideModuloCase(instruction, asmInstructionList);
        return;
    }
    if (isRelationalOp(instruction->instValue.binaryOp.binaryOpType)) {
        handleBinaryRelationalOp(instruction, asmInstructionList);
        return;
    }

    ASMInstruction* mov_inst = createMovInstruction(
        tackyValueToASMOperand(instruction->instValue.binaryOp.src1), 
        tackyValueToASMOperand(instruction->instValue.binaryOp.dest));
    addASMInstructionAtEnd(asmInstructionList, mov_inst);
    
    ASMInstruction* binary_inst = createASMBinaryInstruction(
        instruction->instValue.binaryOp.binaryOpType,
        tackyValueToASMOperand(instruction->instValue.binaryOp.src2),
        tackyValueToASMOperand(instruction->instValue.binaryOp.dest)
    );
    addASMInstructionAtEnd(asmInstructionList, binary_inst);
}


void handleBinaryRelationalOp(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList) {
    ASMInstruction* cmp_inst = createASMCmpInstruction(
        tackyValueToASMOperand(instruction->instValue.binaryOp.src2),
        tackyValueToASMOperand(instruction->instValue.binaryOp.src1)
    );
    addASMInstructionAtEnd(asmInstructionList, cmp_inst);

    ASMInstruction* movInst = createMovInstruction(
        createImmediateOperand(0),
        tackyValueToASMOperand(instruction->instValue.binaryOp.dest)
    );
    addASMInstructionAtEnd(asmInstructionList, movInst);

    ASMCondCode cond = getCondCodeForRelationalOp(instruction->instValue.binaryOp.binaryOpType);
    ASMInstruction* setcc_inst = createASMSetCCInstruction(cond, 
                                tackyValueToASMOperand(instruction->instValue.binaryOp.dest));
    addASMInstructionAtEnd(asmInstructionList, setcc_inst);

}

void handleDivideModuloCase(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList) {
    ASMInstruction* mov_inst = createMovInstruction(
                tackyValueToASMOperand(instruction->instValue.binaryOp.src1), 
                createRegisterOperand(AX));
    addASMInstructionAtEnd(asmInstructionList, mov_inst);


    ASMInstruction* cdq_inst = calloc(1, sizeof(ASMInstruction));
    if (!cdq_inst) return;
    cdq_inst->type = ASM_CDQ;
    addASMInstructionAtEnd(asmInstructionList, cdq_inst);

    // Create IDIV instruction
    ASMInstruction* idiv_inst = createIdivInstruction(
        tackyValueToASMOperand(instruction->instValue.binaryOp.src2));
    addASMInstructionAtEnd(asmInstructionList, idiv_inst);

    if (instruction->instValue.binaryOp.binaryOpType == BIN_REMAINDER) {
        // Move remainder from RDX to destination for modulo
        ASMInstruction* mov_modulo_result = createMovInstruction(
            createRegisterOperand(DX), 
            tackyValueToASMOperand(instruction->instValue.binaryOp.dest));
        addASMInstructionAtEnd(asmInstructionList, mov_modulo_result);
    } else {
        // Move quotient from RAX to destination for division
        ASMInstruction* mov_div_result = createMovInstruction(
            createRegisterOperand(AX), 
            tackyValueToASMOperand(instruction->instValue.binaryOp.dest));
        addASMInstructionAtEnd(asmInstructionList, mov_div_result);
    }
}