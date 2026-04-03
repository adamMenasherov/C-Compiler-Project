#include "ASM-ASTNodes.h"
#include "ASM-ASTNodesUtilities/ASM-ASTNodesConstructors.h"
#include <stdlib.h>
#include <string.h>
#include "../../../ASM-File-Generation/ASM_AST_fix.h"

ASMProgram* parseASMprogram(TACKYProgram* tacky_prog) {
    ASMProgram* asm_prog = malloc(sizeof(ASMProgram));
    if (!asm_prog) return NULL;
    
    asm_prog->function_def = parseASMfunction(tacky_prog->function_def);
    return asm_prog;
}

ASMFunction* parseASMfunction(TACKYFunction* tacky_func) {
    ASMFunction* asm_func = malloc(sizeof(ASMFunction));
    if (!asm_func) return NULL;
    
    asm_func->function_name = tacky_func->function_name; 
    asm_func->inst = createASMInstructionList();
    asm_func->pseudoTable = createHashTable();

    while (tacky_func->instruction_list->cursor < tacky_func->instruction_list->currSize) {
        parseASMInstruction(tacky_func->instruction_list, asm_func->inst);
    }
    parseASMReturn(tacky_func->inst, asm_func->inst);
    pseudoToStackPositions(asm_func->inst, asm_func->pseudoTable);
    
    return asm_func;
}

void parseASMReturn(TACKYReturn* tacky_ret, ASMInstructionList* instruction_list) {
    ASMInstruction* mov_inst = createMovInstruction
                        (tackyValueToASMOperand(tacky_ret->val), createRegisterOperand(AX));
    addASMInstructionAtEnd(instruction_list, mov_inst);

    ASMInstruction* ret_inst = calloc(1, sizeof(ASMInstruction));
    if (!ret_inst) return;
    ret_inst->type = ASM_RET;
    addASMInstructionAtEnd(instruction_list, ret_inst);
}


void parseASMInstruction(TACKYInstructionList* tackyInstList, ASMInstructionList* asmInstructionList) 
{
    if (tackyInstList->cursor >= tackyInstList->currSize) return; // No more instructions
    TACKYInstruction* instruction = tackyInstList->instructions[tackyInstList->cursor++];
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

        case TACKY_JUMP_IF_ZERO:
        case TACKY_JUMP_IF_NOT_ZERO: {
            parseCondJumpInstruction(instruction, asmInstructionList);
            break;
        }
        default: break; // Handle other instruction types as needed
    }
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
    if (instruction->type != TACKY_UNARY) return; // Not a unary instruction
    if (instruction->instValue.unaryOp.type == UNARY_NOT) {
        handleUnaryNot(instruction, asmInstructionList);
        return;
    }
    ASMInstruction* mov_inst = createMovInstruction(
                tackyValueToASMOperand(instruction->instValue.unaryOp.src), 
                tackyValueToASMOperand(instruction->instValue.unaryOp.dest)
            );
    addASMInstructionAtEnd(asmInstructionList, mov_inst);
    ASMInstruction* unary_inst = createASMUnaryInstruction(
        instruction->instValue.unaryOp.type, 
        instruction->instValue.unaryOp.dest);
    addASMInstructionAtEnd(asmInstructionList, unary_inst);
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