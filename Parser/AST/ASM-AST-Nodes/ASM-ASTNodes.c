#include "ASM-ASTNodes.h"
#include <stdlib.h>
#include <string.h>
#include "../../../ASM-File-Generation/ASM_AST_fix.h"

ASMInstructionList* createASMInstructionList() {
    ASMInstructionList* list = malloc(sizeof(ASMInstructionList));
    if (!list) return NULL;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void addASMInstructionAtEnd(ASMInstructionList* list, ASMInstruction* instruction) {
    if (!list->head) {
        list->head = instruction;
        list->tail = instruction;
    } else {
        list->tail->next = instruction;
        list->tail = instruction;
    }
}

void addASMInstructionAtBeginning(ASMInstructionList* list, ASMInstruction* instruction) {
    if (!list->head) {
        list->head = instruction;
        list->tail = instruction;
    } else {
        instruction->next = list->head;
        list->head = instruction;
    }
}

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
            ASMInstruction* mov_inst = createMovInstruction(
                tackyValueToASMOperand(instruction->instValue.unaryOp.src), 
                tackyValueToASMOperand(instruction->instValue.unaryOp.dest)
            );
            addASMInstructionAtEnd(asmInstructionList, mov_inst);
            ASMInstruction* unary_inst = createASMUnaryInstruction(
                instruction->instValue.unaryOp.type, 
                instruction->instValue.unaryOp.dest);
            addASMInstructionAtEnd(asmInstructionList, unary_inst);
            break;
        }
        case TACKY_BINARY: {
            if (instruction->instValue.binaryOp.binaryOpType == BIN_DIVIDE 
                    || instruction->instValue.binaryOp.binaryOpType == BIN_MODULO) {
                handleDivideModuloCase(instruction, asmInstructionList);
                break;
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
        default: break; // Handle other instruction types as needed
    }
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
    ASMInstruction* idiv_inst = CreateIdivInstruction(
        tackyValueToASMOperand(instruction->instValue.binaryOp.src2));
    addASMInstructionAtEnd(asmInstructionList, idiv_inst);

    if (instruction->instValue.binaryOp.binaryOpType == BIN_MODULO) {
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


ASMInstruction* CreateIdivInstruction(ASMOperand* divisor) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_IDIV;
    inst->instValue.idiv.divisor = divisor;  // Use correct idiv union member
    return inst;
}



ASMOperand* tackyValueToASMOperand(TACKYValue* val) {
    ASMOperand* operand = malloc(sizeof(ASMOperand));
    if (!operand) return NULL;

    switch(val->type) {
        case TACKY_CONSTANT: {
            operand->type = ASM_OP_IMMEDIATE;
            operand->OperandValue.immediate = val->constant->value;
            return operand;
        }
        case TACKY_VAR: {
            operand->type = ASM_OP_PSEUDO;
            operand->OperandValue.identifier = strdup(val->identifier);
            return operand;
        }
        default: return NULL;
    }
}

ASMOperand* createRegisterOperand(Register reg) {
    ASMOperand* operand = malloc(sizeof(ASMOperand));
    if (!operand) return NULL;

    operand->type = ASM_OP_REGISTER;
    operand->OperandValue.reg = reg;
    return operand;
}


ASMInstruction* createASMUnaryInstruction(unaryType type, TACKYValue* dest) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_UNARY;
    switch (type) {
        case UNARY_NEGATE:
            inst->instValue.unary.type = ASM_UNARY_NEG;
            break;
        case UNARY_COMPLEMENT:
            inst->instValue.unary.type = ASM_UNARY_NOT;
            break;
         default: 
            free(inst);
            return NULL; // Unsupported unary type
    }
    inst->instValue.unary.op = tackyValueToASMOperand(dest);
    // Destination operand will be determined by the caller, as it may require a temporary register
    return inst;
}


ASMInstruction* createASMBinaryInstruction(binType type, ASMOperand* op1, ASMOperand* op2) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_BINARY;
    switch (type) {
        case BIN_ADD:
            inst->instValue.binary.type = ASM_BINARY_ADD;
            break;
        case BIN_SUBTRACT:
            inst->instValue.binary.type = ASM_BINARY_SUBTRACT;
            break;
        case BIN_MULTIPLY:
            inst->instValue.binary.type = ASM_BINARY_MULTIPLY;
            break;
        default: 
            free(inst);
            return NULL; // Unsupported binary type
    }
    inst->instValue.binary.op1 = op1;
    inst->instValue.binary.op2 = op2;
    return inst;
}


ASMInstruction* createMovInstruction(ASMOperand* src, ASMOperand* dest) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_MOV;
    inst->instValue.mov.operand1 = src;
    inst->instValue.mov.operand2 = dest;
    return inst;
}



ASMInstruction* createAllocStackInstruction(int size) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_ALLOCATESTACK;
    inst->instValue.allocatestack.size = size;
    return inst;
}

ASMOperand* createStackOperand(int offset) {
    ASMOperand* operand = malloc(sizeof(ASMOperand));
    if (!operand) return NULL;

    operand->type = ASM_OP_STACK;
    operand->OperandValue.immediate = offset; // Using immediate to store stack offset
    return operand;
}