#include "../Parser/Parser.h"
#include <stdio.h>
#include "ASMGenerator.h"
#include <stdlib.h>
#include "ASMInstructionFix.h"
#include "../Parser/ASM-Instructions/ASMInstructionsUtilities/ASMInstructionsPrinter.h"
#include "../DataStructures/HashTable/Wrappers/SymbolTableWrapper.h"

static char* turnStaticInitTypeToAsm(initialValueStaticInitType type) {
    switch (type) {
        case STATIC_INIT_INT: return "long";
        case STATIC_INIT_LONG: return "quad";
        case STATIC_INIT_UNSIGNED_INT: return "long";
        case STATIC_INIT_UNSIGNED_LONG: return "quad";
        default: return "<unknown static init type>";
    }
}

static char* turnASMTypeToAsm(ASMType type) {
    switch (type) {
        case ASM_LONGWORD: return "l";
        case ASM_QUADWORD: return "q";
        default: return "<unknown asm type>";
    }
}


void generateASMFile(ASM* ast, char* asm_file_name, SymbolTable* symbolTable) {
    FILE* fp = fopen(asm_file_name, "w");
    if (!fp) {
        fprintf(stderr, "Couldn't create ASM file");
        exit(1);
    } 
    printAsmFileFromAst(ast, fp, symbolTable);
    printf("ASM file %s generated successfully.\n", asm_file_name);
    fclose(fp);
}

void printAsmFileFromAst(ASM *ast, FILE *fp, SymbolTable* symbolTable) {
    printProgramToASMFile(ast->prog, fp, symbolTable);
    fputs("   .section .note.GNU-stack,\"\",@progbits\n", fp);
}   

void printProgramToASMFile(ASMProgram* prog, FILE *fp, SymbolTable* symbolTable) {
    for (int i = 0; i < ASMTopLevelArray_size(prog->topLevels); i++) {
        ASMTopLevel* topLevel = ASMTopLevelArray_get(prog->topLevels, i);
        if (!topLevel) continue;

        if (topLevel->type == ASM_TOP_LEVEL_FUNCTION) {
            printFunctionToASMFile(topLevel->topLevel.function, fp, symbolTable);
        } else if (topLevel->type == ASM_TOP_LEVEL_STATIC_VAR) {
            printStaticVarToASMFile(topLevel->topLevel.staticVar, fp);
        }
    }
}

void printStaticVarToASMFile(ASMStaticVar* staticVar, FILE *fp) {
    if (!staticVar || !staticVar->identifier) return;

    if (staticVar->initVal.val == 0) {
        fprintf(fp, "   .bss\n   .align %d\n", staticVar->alignment);
        if (staticVar->global) {
            fprintf(fp, "   .globl %s\n", staticVar->identifier);
        }
        fprintf(fp, "%s:\n", staticVar->identifier);
        fprintf(fp, "\t.zero %d\n", staticVar->alignment);
        return;
    }

    fputs("   .data\n", fp);
    fprintf(fp, "   .align %d\n", staticVar->alignment);
    if (staticVar->global) {
        fprintf(fp, "   .globl %s\n", staticVar->identifier);
    }
    fprintf(fp, "%s:\n", staticVar->identifier);
    fprintf(fp, "\t.%s %ld\n", turnStaticInitTypeToAsm(staticVar->initVal.staticInitType), staticVar->initVal.val);
}


void printFunctionToASMFile(ASMFunction* func, FILE *fp, SymbolTable* symbolTable) {
    printFunctionPrologueToASMFile(func, fp);
    ASMInstructionList* p = func->inst;
    ASMInstruction* current;
    for (current = p->head; current; current = current->next) {
        printInstructionsToASMFile(current, fp, symbolTable);
    }
}

void printFunctionPrologueToASMFile(ASMFunction* func, FILE *fp) {
    if (func->global) {
        fprintf(fp, "   .globl %s\n", func->function_name);
    }
    fprintf(fp, "   .text\n%s:\n", func->function_name);
    fprintf(fp, "\tpushq %%rbp\n\tmovq %%rsp, %%rbp\n");
}

void printFunctionEpilogueToASMFile(FILE* fp) {
    fputs("\tmovq %rbp, %rsp\n\tpopq %rbp\n", fp);
}

void printInstructionsToASMFile(ASMInstruction* inst, FILE *fp, SymbolTable* symbolTable) {
    switch(inst->type) {
        
        case ASM_MOV:
        {
            REGISTER_SIZE size = inst->instValue.mov.asmType == ASM_LONGWORD ? REGISTER_32_BIT : REGISTER_64_BIT;
            fprintf(fp, "\tmov%s ", turnASMTypeToAsm(inst->instValue.mov.asmType));
            printOperandToASMFile(inst->instValue.mov.operand1, fp, size);
            fputs(", ", fp);
            printOperandToASMFile(inst->instValue.mov.operand2, fp, size);
            fputc('\n', fp);
            break;
        }
        case ASM_UNARY: {
            REGISTER_SIZE size = inst->instValue.unary.asmType == ASM_LONGWORD ? REGISTER_32_BIT : REGISTER_64_BIT;
            fprintf(fp, "\t%s%s ", asmUnaryOperatorToString(inst->instValue.unary.type),
                    turnASMTypeToAsm(inst->instValue.unary.asmType));
            printOperandToASMFile(inst->instValue.unary.op, fp, size);
            fputc('\n', fp);
            break;
        }
        case ASM_JUMP: {
            fprintf(fp, "\tjmp .L%s\n", inst->instValue.jmp.label);
            break;
        }
        case ASM_LABEL: {
            fprintf(fp, ".L%s:\n", inst->instValue.label.identifier);
            break;
        }
        case ASM_SETCC: {
            fprintf(fp, "\tset%s ", asmCondCodeToString(inst->instValue.setcc.cond));
            printOperandToASMFile(inst->instValue.setcc.op, fp, REGISTER_8_BIT);
            fputc('\n', fp);
            break;
        }
        case ASM_JUMPCC: {
            fprintf(fp, "\tj%s .L%s\n", 
                        asmCondCodeToString(inst->instValue.jumpcc.cond), inst->instValue.jumpcc.label);
            break;
        }

        case ASM_CMP: {
            REGISTER_SIZE size = inst->instValue.cmp.asmType == ASM_LONGWORD ? REGISTER_32_BIT : REGISTER_64_BIT;
            fprintf(fp, "\tcmp%s ", turnASMTypeToAsm(inst->instValue.cmp.asmType));
            printOperandToASMFile(inst->instValue.cmp.op1, fp, size);
            fputs(", ", fp);
            printOperandToASMFile(inst->instValue.cmp.op2, fp, size);
            fputc('\n', fp);
            break;
        }

        case ASM_BINARY: {
            REGISTER_SIZE size = inst->instValue.binary.asmType == ASM_LONGWORD ? REGISTER_32_BIT : REGISTER_64_BIT;
            fprintf(fp, "\t%s%s ", asmBinaryOperatorToString(inst->instValue.binary.type),
                    turnASMTypeToAsm(inst->instValue.binary.asmType));
            if (inst->instValue.binary.type == ASM_BINARY_SHIFT_LEFT || inst->instValue.binary.type == ASM_BINARY_SHIFT_RIGHT) 
                printOperandToASMFile(inst->instValue.binary.op1, fp, REGISTER_8_BIT);
            else 
                printOperandToASMFile(inst->instValue.binary.op1, fp, size);
            fputs(", ", fp);
            printOperandToASMFile(inst->instValue.binary.op2, fp, size);
            fputc('\n', fp);
            break;
        }

        case ASM_MOVSX: {
            fprintf(fp, "\tmovslq ");
            printOperandToASMFile(inst->instValue.movsx.operand1, fp, REGISTER_32_BIT);
            fputs(", ", fp);
            printOperandToASMFile(inst->instValue.movsx.operand2, fp, REGISTER_64_BIT);
            fputc('\n', fp);
            break;
        }
    
        case ASM_IDIV: {
            REGISTER_SIZE size = inst->instValue.idiv.asmType == ASM_LONGWORD ? REGISTER_32_BIT : REGISTER_64_BIT;
            fprintf(fp, "\tidiv%s ", turnASMTypeToAsm(inst->instValue.idiv.asmType));
            printOperandToASMFile(inst->instValue.idiv.divisor, fp, size);
            fputc('\n', fp);
            break;
        }
        case ASM_DIV: {
            REGISTER_SIZE size = inst->instValue.div.asmType == ASM_LONGWORD ? REGISTER_32_BIT : REGISTER_64_BIT;
            fprintf(fp, "\tdiv%s ", turnASMTypeToAsm(inst->instValue.div.asmType));
            printOperandToASMFile(inst->instValue.div.divisor, fp, size);
            fputc('\n', fp);
            break;
        }
        case ASM_RET: {
            printFunctionEpilogueToASMFile(fp);
            fputs("\tret\n\n", fp);
            break;
        }
        case ASM_CALL: {
            char* functionNameToCall = callingWithPLTOrNot((const char*)inst->instValue.call.functionName, symbolTable);
            fprintf(fp, "\tcall %s\n", functionNameToCall);
            if (functionNameToCall) free(functionNameToCall);
            break;
        }

        case ASM_PUSH: {
            fputs("\tpushq ", fp);
            printOperandToASMFile(inst->instValue.push.op, fp, REGISTER_64_BIT);
            fputc('\n', fp);
            break;
        }

        case ASM_POP: {
            fputs("\tpopq ", fp);
            printOperandToASMFile(inst->instValue.pop.op, fp, REGISTER_64_BIT);
            fputc('\n', fp);
            break;
        }
        
        case ASM_CDQ: {
            if (inst->instValue.cdq.asmType == ASM_LONGWORD) 
                fputs("\tcdq\n", fp);
            else
                fputs("\tcqo\n", fp);
            break;
        }
    }
}

void printOperandToASMFile(ASMOperand* op, FILE *fp, REGISTER_SIZE size) 
{
    if (!op) return;
    switch(op->type) {
        case ASM_OP_REGISTER: {
            fprintf(fp, "%%%s", getRegisterNameForCodeEmission(op->OperandValue.reg, size));
            break;
        }
            
        case ASM_OP_IMMEDIATE: {
            fprintf(fp, "$%ld", (long)op->OperandValue.immediate);
            break;
        }

        case ASM_OP_STACK: {
            fprintf(fp, "%ld(%%rbp)", (long)op->OperandValue.immediate);
            break;
        }
        case ASM_OP_DATA: {
            fprintf(fp, "%s(%%rip)", op->OperandValue.identifier);
            break;
        }
        default: break;
    }
}

const char* getRegisterNameForCodeEmission(Register reg, REGISTER_SIZE size) {
    switch (size) {
        case REGISTER_8_BIT:
            switch (reg) {
                case AX: return "al";
                case DX: return "dl";
                case CX: return "cl";
                case R8: return "r8b";
                case SI: return "sil";
                case SP: return "spl";
                case DI: return "dil";
                case R9: return "r9b";
                case R10: return "r10b";
                case R11: return "r11b";
                default: return "<unknown register>";
            }
        case REGISTER_16_BIT:
            switch (reg) {
                case AX: return "ax";
                case DX: return "dx";
                case CX: return "cx";
                case SP: return "sp";
                case R8: return "r8w";
                case R9: return "r9w";
                case SI: return "si";
                case DI: return "di";
                case R10: return "r10w";
                case R11: return "r11w";
                default: return "<unknown register>";
            }
        case REGISTER_32_BIT:
            switch (reg) {
                case AX: return "eax";
                case DX: return "edx";
                case CX: return "ecx";
                case R8: return "r8d";
                case SP: return "esp";
                case SI: return "esi";
                case DI: return "edi";
                case R9: return "r9d";
                case R10: return "r10d";
                case R11: return "r11d";
                default: return "<unknown register>";
            }
        case REGISTER_64_BIT:
            switch (reg) {
                case AX: return "rax";
                case DX: return "rdx";
                case CX: return "rcx";
                case R8: return "r8";
                case SP: return "rsp";
                case R9: return "r9";
                case SI: return "rsi";
                case DI: return "rdi";
                case R10: return "r10";
                case R11: return "r11";
                default: return "<unknown register>";
            }
        default:
            return "<unknown register size>";
    }
}

const char * asmUnaryOperatorToString(ASMUnaryOperator op) {
    switch (op) {
        case ASM_UNARY_NEG: return "neg";
        case ASM_UNARY_NOT: return "not";
        case ASM_UNARY_DEC: return "dec";
        case ASM_UNARY_INC: return "inc";
        default: return "<unknown unary operator>";
    }
}
const char * asmBinaryOperatorToString(ASMBinaryOperator op) {
    switch (op) {
        case ASM_BINARY_ADD: return "add";
        case ASM_BINARY_SUBTRACT: return "sub";
        case ASM_BINARY_MULTIPLY: return "imul";
        case ASM_BINARY_AND: return "and";
        case ASM_BINARY_OR: return "or";
        case ASM_BINARY_XOR: return "xor";
        case ASM_BINARY_SHIFT_LEFT: return "sal";
        case ASM_BINARY_SHIFT_RIGHT: return "sar";
        default: return "<unknown binary operator>";
    }
}

const char* asmCondCodeToString(ASMCondCode cond) {
    switch (cond) {
        case ASM_COND_CODE_E: return "e";
        case ASM_COND_CODE_NE: return "ne";
        case ASM_COND_CODE_L: return "l";
        case ASM_COND_CODE_LE: return "le";
        case ASM_COND_CODE_G: return "g";
        case ASM_COND_CODE_GE: return "ge";
        case ASM_COND_CODE_B: return "b";
        case ASM_COND_CODE_BE: return "be";
        case ASM_COND_CODE_A: return "a";
        case ASM_COND_CODE_AE: return "ae";
        default: return "<unknown condition code>";
    }
}

char* callingWithPLTOrNot(const char* functionName, SymbolTable* symbolTable) {
    if (symbolTableContains(symbolTable, functionName)) {
        IdentifierTypeInfo* info = symbolTableLookup(symbolTable, functionName);
        if (info->funcInfo.isDefined != 0) {
            return strdup(functionName);
        }
    }
    char* pltName = malloc(strlen(functionName) + strlen("@PLT") + 1); 
    sprintf(pltName,"%s@PLT", functionName);
    return pltName;
}