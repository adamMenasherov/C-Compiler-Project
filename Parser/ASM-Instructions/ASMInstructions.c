#include "ASMInstructions.h"
#include "ASM-Parsers/ASMParserInclude.h"
#include "ASMInstructionsUtilities/ASMInstructionsFree.h"
#include <stdlib.h>
#include <string.h>
#include "../../ASM-File-Generation/ASMInstructionFix.h"
#include "../../DataStructures/HashTable/Wrappers/AsmSymbolTableWrapper.h"


void parseASMReturn(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    TACKYValue* tacky_ret = instruction->instValue.returnVal.retVal;
    ASMType ret_type  = convertTACKYTypeToASMType(tacky_ret, symTable);
    ASMOperand* ret_op = tackyValueToASMOperand(tacky_ret, symTable);

    if (ret_type.kind == ASM_DOUBLE) {
        addASMInstructionAtEnd(asmInstructionList, createMovInstruction(ret_type, ret_op, createRegisterOperand(XMM0)));
    } else {
        addASMInstructionAtEnd(asmInstructionList, createMovInstruction(ret_type, ret_op, createRegisterOperand(AX)));
    }
    addASMInstructionAtEnd(asmInstructionList, createASMReturnInstruction());
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
        createMovInstruction((ASMType){.kind = ASM_LONGWORD}, createImmediateOperand(0), createRegisterOperand(AX))
    );
    addASMInstructionAtEnd(asm_func->inst, createASMReturnInstruction());
    ASMSymbolTable* asmSymTable = convertFrontEndSymTableToASMSymTable(symTable);
    pseudoToStackPositions(asm_func->inst, asm_func->pseudoTable, asmSymTable, symTable);
    freeAsmSymbolTable(asmSymTable);

    return asm_func;
}

ASMProgram* parseASMprogram(TACKYProgram* tacky_prog, SymbolTable* symTable) {
    ASMProgram* asm_prog = malloc(sizeof(ASMProgram));
    if (!asm_prog) return NULL;
    asm_prog->topLevels = ASMTopLevelArray_create();
    if (!asm_prog->topLevels) {
        free(asm_prog);
        return NULL;
    }
    initConstantCache();
    for (int i = 0; i < TACKYTopLevelArray_size(tacky_prog->topLevels); i++) {
        TACKYTopLevel* topLevel = TACKYTopLevelArray_get(tacky_prog->topLevels, i);
        if (!topLevel) continue;

        switch (topLevel->type) {
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

    addConstantsAsTopLevels(asm_prog, getConstantCache());
    destroyConstantCache();
    return asm_prog;
}
