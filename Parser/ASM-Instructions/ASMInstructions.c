#include "ASMInstructions.h"
#include "ASMInstructionsUtilities/ASMInstructionsConstructors.h"
#include "ASMInstructionsUtilities/ASMInstructionsFree.h"
#include <stdlib.h>
#include <string.h>
#include "../../ASM-File-Generation/ASMInstructionFix.h"
#include "../TACKY/TACKYUtils/TACKYConstructors.h"
#include "../../DataStructures/HashTable/Wrappers/AsmSymbolTableWrapper.h"
#include "../AST/C-AST-Nodes/C-ASTNodeUtilities/TokenExpect/C-ASTNodeExpect.h"
#include "../Common/SharedTypeRank.h"


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

    addArgsAsInstructionsToFunc(tacky_func, asm_func->inst, symTable);
    while (InstructionArray_getCursor(tacky_func->instruction_list) < InstructionArray_size(tacky_func->instruction_list)) {
        parseASMInstruction(tacky_func->instruction_list, asm_func->inst, symTable);
    }
    addASMInstructionAtEnd(
        asm_func->inst,
        createMovInstruction(ASM_LONGWORD,createImmediateOperand(0), createRegisterOperand(AX))
    );
    addASMInstructionAtEnd(asm_func->inst, createASMReturnInstruction());
    ASMSymbolTable* asmSymTable = convertFrontEndSymTableToASMSymTable(symTable);
    pseudoToStackPositions(asm_func->inst, asm_func->pseudoTable, asmSymTable, symTable);
    
    return asm_func;
}

void parseASMReturn(TACKYValue* tacky_ret, ASMInstructionList* instruction_list, SymbolTable* symTable) {
    ASMInstruction* mov_inst = createMovInstruction
                        (convertTACKYTypeToASMType(tacky_ret, symTable), tackyValueToASMOperand(tacky_ret, symTable), createRegisterOperand(AX));
    addASMInstructionAtEnd(instruction_list, mov_inst);

    ASMInstruction* ret_inst = createASMReturnInstruction();
    addASMInstructionAtEnd(instruction_list, ret_inst);
}

void addArgsAsInstructionsToFunc(TACKYFunction* tacky_func, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    IdentifierTypeInfo* function = symbolTableLookup(symTable, tacky_func->function_name);
    CFuncType* funcType = function->funcInfo.funcType;
    IdentifierArray* params = tacky_func->parameters;
    int lenArgs = IdentifierArray_size(params);

    for (int i = 0; i < lenArgs && i < 6; i++) {
        char* arg = IdentifierArray_get(params, i);
        ASMType paramAsmType = ASM_LONGWORD;
        if (funcType && funcType->func.params[i]) {
            paramAsmType = convertSpecifierTypeToASMType(funcType->func.params[i]->type);
        }
        ASMInstruction* mov_arg_inst = createMovInstruction(
            paramAsmType,
            createRegisterOperand(argResigters[i]),
            tackyValueToASMOperand(createVarValue(arg), symTable));
        addASMInstructionAtEnd(asmInstructionList, mov_arg_inst);
    }
    int stackPos = 16;
    for (int i = 6; i < lenArgs; i++) {
        char* arg = IdentifierArray_get(params, i);
        ASMType paramAsmType = ASM_LONGWORD;
        if (funcType && funcType->func.params[i]) {
            paramAsmType = convertSpecifierTypeToASMType(funcType->func.params[i]->type);
        }
        ASMInstruction* movInst = createMovInstruction(
            paramAsmType,
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
                convertTACKYTypeToASMType(instruction->instValue.copy.src, symTable),
                tackyValueToASMOperand(instruction->instValue.copy.src, symTable), 
                tackyValueToASMOperand(instruction->instValue.copy.dest, symTable));
            addASMInstructionAtEnd(asmInstructionList, mov_inst);
            break;
        }
        case TACKY_SIGN_EXTEND: {
            parseASMSignExtendInstruction(instruction, asmInstructionList, symTable);
            break;
        }
        case TACKY_ZERO_EXTEND: {
            parseZeroExtendInstruction(instruction, asmInstructionList, symTable);
            break;
        }
        case TACKY_TRUNCATE: {
            parseASMTruncateInstruction(instruction, asmInstructionList, symTable);
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
        ASMType argType = convertTACKYTypeToASMType(arg, symTable);
        ASMInstruction* mov_arg_inst = createMovInstruction(argType,
            tackyValueToASMOperand(arg, symTable), 
            createRegisterOperand(argResigters[i]));
        addASMInstructionAtEnd(asmInstructionList, mov_arg_inst);
    }

    for (int i = lenArgs - 1; i >= 6; i--) {
        TACKYValue* arg = TACKYValueArray_get(instruction->instValue.funCall.args, i);
        ASMOperand* argOp = tackyValueToASMOperand(arg, symTable);
        ASMType argType = convertTACKYTypeToASMType(arg, symTable);
        if (argOp->type == ASM_OP_IMMEDIATE || argOp->type == ASM_OP_REGISTER || argType == ASM_QUADWORD) {
            ASMInstruction* push_inst = createASMPushInstruction(argOp);
            addASMInstructionAtEnd(asmInstructionList, push_inst);
        } 
        else {
            ASMInstruction* mov_arg_inst = createMovInstruction(ASM_LONGWORD, argOp, createRegisterOperand(AX));
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
    ASMType retAsmType = ASM_LONGWORD;
    IdentifierTypeInfo* calledFunction = symbolTableLookup(symTable, instruction->instValue.funCall.functionName);
    if (calledFunction && calledFunction->type == TYPE_FUNCTION && calledFunction->funcInfo.funcType && calledFunction->funcInfo.funcType->func.ret) {
        retAsmType = convertSpecifierTypeToASMType(calledFunction->funcInfo.funcType->func.ret->type);
    }
    ASMOperand* retDest = tackyValueToASMOperand(instruction->instValue.funCall.resultVar, symTable);
    addASMInstructionAtEnd(asmInstructionList, 
        createMovInstruction(retAsmType, createRegisterOperand(AX), retDest)); 
}


void parseCondJumpInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMCondCode cond = (instruction->type == TACKY_JUMP_IF_ZERO) ? ASM_COND_CODE_E : ASM_COND_CODE_NE;
    ASMInstruction* cmp_inst = createASMCmpInstruction(
        convertTACKYTypeToASMType(instruction->instValue.condJump.condition, symTable),
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
        convertTACKYTypeToASMType(instruction->instValue.unaryOp.src, symTable),
        createImmediateOperand(0),
        tackyValueToASMOperand(instruction->instValue.unaryOp.src, symTable)
    );
    addASMInstructionAtEnd(asmInstructionList, cmp_inst);

    ASMInstruction* mov_inst = createMovInstruction(
        convertTACKYTypeToASMType(instruction->instValue.unaryOp.dest, symTable),
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
            convertTACKYTypeToASMType(instruction->instValue.unaryOp.src, symTable),
            instruction->instValue.unaryOp.src,
            symTable);
        addASMInstructionAtEnd(asmInstructionList, unary_inst);

        ASMInstruction* mov_inst = createMovInstruction(
            convertTACKYTypeToASMType(instruction->instValue.unaryOp.src, symTable),
            tackyValueToASMOperand(instruction->instValue.unaryOp.src, symTable),
            tackyValueToASMOperand(instruction->instValue.unaryOp.dest, symTable));
        addASMInstructionAtEnd(asmInstructionList, mov_inst);
    } else {
        ASMInstruction* mov_inst = createMovInstruction(
            convertTACKYTypeToASMType(instruction->instValue.unaryOp.src, symTable),
            tackyValueToASMOperand(instruction->instValue.unaryOp.src, symTable), 
            tackyValueToASMOperand(instruction->instValue.unaryOp.dest, symTable));
        addASMInstructionAtEnd(asmInstructionList, mov_inst);

        ASMInstruction* unary_inst = createASMUnaryInstruction(
            instruction->instValue.unaryOp.type, 
            convertTACKYTypeToASMType(instruction->instValue.unaryOp.dest, symTable),
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
        convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable),
        tackyValueToASMOperand(instruction->instValue.binaryOp.src1, symTable), 
        tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable));
    addASMInstructionAtEnd(asmInstructionList, mov_inst);
    
    ASMInstruction* binary_inst = createASMBinaryInstruction(
        binTypeToASMBinaryOperator(instruction->instValue.binaryOp.binaryOpType),
        convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable),
        tackyValueToASMOperand(instruction->instValue.binaryOp.src2, symTable),
        tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable)
    );
    addASMInstructionAtEnd(asmInstructionList, binary_inst);
}


void handleBinaryRelationalOp(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMInstruction* cmp_inst = createASMCmpInstruction(
        convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable),
        tackyValueToASMOperand(instruction->instValue.binaryOp.src2, symTable),
        tackyValueToASMOperand(instruction->instValue.binaryOp.src1, symTable)
    );
    addASMInstructionAtEnd(asmInstructionList, cmp_inst);

    ASMInstruction* movInst = createMovInstruction(
        convertTACKYTypeToASMType(instruction->instValue.binaryOp.dest, symTable),
        createImmediateOperand(0),
        tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable)
    );
    addASMInstructionAtEnd(asmInstructionList, movInst);

    ASMCondCode cond = getCondCodeForRelationalOp(instruction->instValue.binaryOp.binaryOpType, signedOrUnsigned(instruction, symTable));
    ASMInstruction* setcc_inst = createASMSetCCInstruction(cond, 
                                tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable));
    addASMInstructionAtEnd(asmInstructionList, setcc_inst);
}

void parseZeroExtendInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    addASMInstructionAtEnd(asmInstructionList, 
        createASMMovZeroExtendInstruction(
            tackyValueToASMOperand(instruction->instValue.zeroExtend.src, symTable), 
            tackyValueToASMOperand(instruction->instValue.zeroExtend.dest, symTable)
        )
    );
}

void handleDivideModuloCase(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMInstruction* mov_inst = createMovInstruction(
                convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable),
                tackyValueToASMOperand(instruction->instValue.binaryOp.src1, symTable), 
                createRegisterOperand(AX));
    addASMInstructionAtEnd(asmInstructionList, mov_inst);


    divisionInst divInstFunc = getDivisionInst(instruction, asmInstructionList, symTable);
    // Create division instruction
    ASMInstruction* idiv_inst = divInstFunc(
        tackyValueToASMOperand(instruction->instValue.binaryOp.src2, symTable),
        convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable));
    addASMInstructionAtEnd(asmInstructionList, idiv_inst);

    if (instruction->instValue.binaryOp.binaryOpType == BIN_REMAINDER) {
        // Move remainder from RDX to destination for modulo
        ASMInstruction* mov_modulo_result = createMovInstruction(
            convertTACKYTypeToASMType(instruction->instValue.binaryOp.dest, symTable),
            createRegisterOperand(DX), 
            tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable));
        addASMInstructionAtEnd(asmInstructionList, mov_modulo_result);
    } else {
        // Move quotient from RAX to destination for division
        ASMInstruction* mov_div_result = createMovInstruction(
            convertTACKYTypeToASMType(instruction->instValue.binaryOp.dest, symTable),
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
    asmStaticVar->initVal.val = tackyStaticVar->initVal.val;
    asmStaticVar->initVal.staticInitType = tackyStaticVar->initVal.staticInitType;
    if (tackyStaticVar->initVal.staticInitType == STATIC_INIT_INT
        || tackyStaticVar->initVal.staticInitType == STATIC_INIT_UNSIGNED_INT) {
        asmStaticVar->alignment = 4;
    } else if (tackyStaticVar->initVal.staticInitType == STATIC_INIT_LONG
               || tackyStaticVar->initVal.staticInitType == STATIC_INIT_UNSIGNED_LONG) {
        asmStaticVar->alignment = 8;
    } else {
        asmStaticVar->alignment = 4;
    }

    return createTopLevel(ASM_TOP_LEVEL_STATIC_VAR, asmStaticVar);
}

void parseASMSignExtendInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    addASMInstructionAtEnd(asmInstructionList, 
        createASMMovsxInstruction(
            tackyValueToASMOperand(instruction->instValue.signExtend.src, symTable), 
            tackyValueToASMOperand(instruction->instValue.signExtend.dest, symTable)
        )
    );
}

void parseASMTruncateInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    addASMInstructionAtEnd(asmInstructionList, 
        createMovInstruction(
            ASM_LONGWORD,
            tackyValueToASMOperand(instruction->instValue.truncate.src, symTable), 
            tackyValueToASMOperand(instruction->instValue.truncate.dest, symTable)
        )
    );
}

divisionInst getDivisionInst(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    TACKYValue* src1 = instruction->instValue.binaryOp.src1;
    if(isSignedTACKYValue(src1, symTable)) {
        ASMInstruction* cdq_inst = createASMCDQInstruction(
                convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable));
        addASMInstructionAtEnd(asmInstructionList, cdq_inst);
        return createIdivInstruction;
    } else {
        ASMInstruction* movInst = createMovInstruction(
            convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable),
            createImmediateOperand(0),
            createRegisterOperand(DX)
        );
        addASMInstructionAtEnd(asmInstructionList, movInst);
        return createDivInstruction;
    }
}