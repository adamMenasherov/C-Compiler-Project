#include "ASMInstructionsConstructors.h"
#include "../../generateUtils.h"
#include <stdlib.h>
#include <string.h>

/* Module-static cache: double bit-pattern → label string.
 * Lifetime is one ASM program pass (init/destroy called by parseASMprogram). */
DoubleStringMap* g_constCache = NULL;

void initConstantCache() {
    g_constCache = createDoubleStringMap();
}

void destroyConstantCache() {
    freeDoubleStringMap(g_constCache);
    g_constCache = NULL;
}

DoubleStringMap* getConstantCache() {
    return g_constCache;
}

/* ============================================================
 * Instruction List Management Implementation
 * ============================================================ */

ASMInstructionList* createASMInstructionList() {
    ASMInstructionList* list = malloc(sizeof(ASMInstructionList));
    if (!list) return NULL;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void addASMInstructionAtEnd(ASMInstructionList* list, ASMInstruction* instruction) {
    if (!list || !instruction) return;

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

ASMStaticConst* createAsmStaticConst(char* identifier, int alignment, double initVal) {
    ASMStaticConst* asmStaticConst = malloc(sizeof(ASMStaticConst));
    if (!asmStaticConst) return NULL;

    asmStaticConst->identifier = strdup(identifier);
    if (!asmStaticConst->identifier) {
        free(asmStaticConst);
        return NULL;
    }
    asmStaticConst->alignment = initVal == -0.0 ? 16 : alignment;
    asmStaticConst->initVal.val.doubleVal = initVal;
    asmStaticConst->initVal.staticInitType = STATIC_INIT_DOUBLE;

    return asmStaticConst;
}


/* ============================================================
 * Operand Creation Implementation
 * ============================================================ */

ASMOperand* createRegisterOperand(Register reg) {
    ASMOperand* operand = malloc(sizeof(ASMOperand));
    if (!operand) return NULL;

    operand->type = ASM_OP_REGISTER;
    operand->OperandValue.reg = reg;
    return operand;
}

ASMOperand* createImmediateOperand(uint64_t value) {
    ASMOperand* operand = malloc(sizeof(ASMOperand));
    if (!operand) return NULL;

    operand->type = ASM_OP_IMMEDIATE;
    operand->OperandValue.immediate = value;
    return operand;
}

ASMOperand* createStackOperand(int offset) {
    ASMOperand* operand = malloc(sizeof(ASMOperand));
    if (!operand) return NULL;

    operand->type = ASM_OP_STACK;
    operand->OperandValue.immediate = offset; // Using immediate to store stack offset
    return operand;
}

ASMInstruction* createASMMovZeroExtendInstruction(ASMOperand* src, ASMOperand* dest) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_MOVZEROEXTEND; // Using MOVZX for zero extension
    inst->instValue.movZeroExtend.operand1 = src;
    inst->instValue.movZeroExtend.operand2 = dest;
    return inst;
}

ASMInstruction* createDivInstruction(ASMOperand* divisor, ASMType asmType) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_DIV; // Using a separate type for unsigned division
    inst->instValue.div.asmType = asmType; // Reusing idiv union member for divisor
    inst->instValue.div.divisor = divisor;
    return inst;
}

ASMOperand* createAsmConstantOperand(double val, SymbolTable* symTable) {
    char* label = createStaticConstant(val, symTable);
    ASMOperand* operand = malloc(sizeof(ASMOperand));
    if (!operand) return NULL;
    operand->type = ASM_OP_DATA;
    operand->OperandValue.identifier = label;
    return operand;
}

ASMOperand* tackyValueToASMOperand(TACKYValue* val, SymbolTable* symTable) {
    IdentifierTypeInfo* info;
    ASMOperand* operand = malloc(sizeof(ASMOperand));
    if (!operand) return NULL;

    switch(val->type) {
        case TACKY_CONSTANT: {
            if (val->constant->type == CONST_FLOATING_POINT) {
                char* label = createStaticConstantFromTACKYValue(val, symTable);
                operand->OperandValue.identifier = label;
                operand->type = ASM_OP_DATA;
                return operand;
            }
            else {
                operand->type = ASM_OP_IMMEDIATE;
                operand->OperandValue.immediate = val->constant->val.intValue;
                return operand;
            }
            
        }
        case TACKY_VAR: {
            if ((info = symbolTableLookup(symTable, val->identifier)) &&
                info->attrs &&
                info->attrs->attrType == IDENTIFIER_STATIC_ATTR) {
                operand->type = ASM_OP_DATA;
                operand->OperandValue.identifier = strdup(val->identifier);
                return operand;
            }
            operand->type = ASM_OP_PSEUDO;
            operand->OperandValue.identifier = strdup(val->identifier);
            return operand;
        }
        case TACKY_STATIC: {
            operand->type = ASM_OP_DATA;
            operand->OperandValue.identifier = strdup(val->identifier);
            return operand;
        }
        default: return NULL;
    }
}

char* createStaticConstantFromTACKYValue(TACKYValue* val, SymbolTable* symTable) {
    if (val->type != TACKY_CONSTANT || val->constant->type != CONST_FLOATING_POINT) {
        return NULL; // Not a floating-point constant
    }
    return createStaticConstant(val->constant->val.doubleValue, symTable);
}

char* createStaticConstant(double dval, SymbolTable* symTable) {
    if (g_constCache) {
        const char* existing = doubleStringMapGet(g_constCache, dval);
        if (existing) return strdup(existing);
    }

    char* temp = generateDoubleTempName();
    initialValue* initVal = createInitialValue(STATIC_INIT_DOUBLE, INITIAL_WITH_VALUE, 0, dval);
    identifierAttrs* attrs = createIdentifierAttrs(IDENTIFIER_STATIC_ATTR, 0, initVal, 1);
    symbolTableInsert(symTable, temp, TYPE_DOUBLE, NULL, 1, attrs);

    if (g_constCache) doubleStringMapPut(g_constCache, dval, temp);

    return strdup(temp);
}

/* ============================================================
 * Instruction Creation Implementation
 * ============================================================ */

ASMInstruction* createASMUnaryInstruction(unaryType type, ASMType asmType, TACKYValue* dest, SymbolTable* symTable) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_UNARY;
    inst->instValue.unary.asmType = asmType;
    switch (type) {
        case UNARY_NEGATE:
            inst->instValue.unary.type = ASM_UNARY_NEG;
            break;
        case UNARY_COMPLEMENT:
            inst->instValue.unary.type = ASM_UNARY_NOT;
            break;
        case UNARY_NOT:
            inst->instValue.unary.type = ASM_UNARY_NOT;
            break;
        case UNARY_DECREMENT:
            inst->instValue.unary.type = ASM_UNARY_DEC;
            break;
        case UNARY_INCREMENT:
            inst->instValue.unary.type = ASM_UNARY_INC;
            break;
         default: 
            free(inst);
            return NULL; // Unsupported unary type
    }
    inst->instValue.unary.op = tackyValueToASMOperand(dest, symTable);
    // Destination operand will be determined by the caller, as it may require a temporary register
    return inst;
}

ASMBinaryOperator binTypeToASMBinaryOperator(binType type) {
    switch (type) {
        case BIN_ADD:          return ASM_BINARY_ADD;
        case BIN_SUBTRACT:     return ASM_BINARY_SUBTRACT;
        case BIN_MULTIPLY:     return ASM_BINARY_MULTIPLY;
        case BIN_BITWISE_AND:  return ASM_BINARY_AND;
        case BIN_BITWISE_OR:   return ASM_BINARY_OR;
        case BIN_BITWISE_XOR:  return ASM_BINARY_XOR;
        case BIN_LEFT_SHIFT:   return ASM_BINARY_SHIFT_LEFT;
        case BIN_RIGHT_SHIFT:  return ASM_BINARY_SHIFT_RIGHT;
        default:               return -1;
    }
}

ASMInstruction* createASMBinaryInstruction(ASMBinaryOperator op, ASMType asmType, ASMOperand* op1, ASMOperand* op2) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_BINARY;
    inst->instValue.binary.asmType = asmType;
    inst->instValue.binary.type = op;
    inst->instValue.binary.op1 = op1;
    inst->instValue.binary.op2 = op2;
    return inst;
}

ASMInstruction* createMovInstruction(ASMType asmType, ASMOperand* src, ASMOperand* dest) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_MOV;
    inst->instValue.mov.asmType = asmType;
    inst->instValue.mov.operand1 = src;
    inst->instValue.mov.operand2 = dest;
    return inst;
}

ASMInstruction* createIdivInstruction(ASMOperand* divisor, ASMType asmType) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_IDIV;
    inst->instValue.idiv.asmType = asmType;
    inst->instValue.idiv.divisor = divisor;  // Use correct idiv union member
    return inst;
}

ASMInstruction* createASMCmpInstruction(ASMType asmType, ASMOperand* op1, ASMOperand* op2) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_CMP;
    inst->instValue.cmp.asmType = asmType;
    inst->instValue.cmp.op1 = op1;
    inst->instValue.cmp.op2 = op2;
    return inst;
}

ASMInstruction* createASMCDQInstruction(ASMType asmType) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_CDQ;
    inst->instValue.cdq.asmType = asmType;
    return inst;
}

ASMInstruction* createASMMovsxInstruction(ASMOperand* src, ASMOperand* dest) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_MOVSX;
    inst->instValue.movsx.operand1 = src;
    inst->instValue.movsx.operand2 = dest;
    return inst;
}

ASMInstruction* createCvtsi2sdInstruction(ASMType src_type, ASMOperand* src, ASMOperand* dest) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_CVTSI2SD;
    inst->instValue.cvtsi2sd.src_type = src_type;
    inst->instValue.cvtsi2sd.src = src;
    inst->instValue.cvtsi2sd.dest = dest;
    return inst;
}

ASMInstruction* createCvttsd2siInstruction(ASMType dst_type, ASMOperand* src, ASMOperand* dest) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_CVTTSD2SI;
    inst->instValue.cvttsd2si.dst_type = dst_type;
    inst->instValue.cvttsd2si.src = src;
    inst->instValue.cvttsd2si.dest = dest;
    return inst;
}

ASMInstruction* createASMJumpInstruction(char* label) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_JUMP;
    inst->instValue.jmp.label = strdup(label);
    if (!inst->instValue.jmp.label) {
        free(inst);
        return NULL;
    }
    return inst;
}

ASMInstruction* createASMPushInstruction(ASMOperand* src) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_PUSH;
    inst->instValue.push.op = src;
    return inst;
}

ASMInstruction* createASMPopInstruction(ASMOperand* dest) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_POP;
    inst->instValue.pop.op = dest;
    return inst;
}

ASMInstruction* createASMJumpCCInstruction(ASMCondCode cond, char* label) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_JUMPCC;
    inst->instValue.jumpcc.cond = cond;
    inst->instValue.jumpcc.label = strdup(label);
    if (!inst->instValue.jumpcc.label) {
        free(inst);
        return NULL;
    }
    return inst;
}

ASMInstruction* createASMSetCCInstruction(ASMCondCode cond, ASMOperand* op) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_SETCC;
    inst->instValue.setcc.cond = cond;
    inst->instValue.setcc.op = op;
    return inst;
}

ASMInstruction* createASMLabelInstruction(char* label) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_LABEL;
    inst->instValue.label.identifier = strdup(label);
    if (!inst->instValue.label.identifier) {
        free(inst);
        return NULL;
    }
    return inst;
}

ASMInstruction* createASMReturnInstruction() {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_RET;
    return inst;
}

ASMInstruction* createASMAllocateStackInstruction(int size) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_BINARY;
    inst->instValue.binary.type = ASM_BINARY_SUBTRACT; // Use SUBTRACT to represent stack allocation (e.g., SUB RSP, size)
    inst->instValue.binary.asmType = ASM_QUADWORD; 
    inst->instValue.binary.op2 = createRegisterOperand(SP); 
    inst->instValue.binary.op1 = createImmediateOperand(size);
    return inst;
}

ASMInstruction* createASMCallInstruction(char* functionName) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_CALL;
    inst->instValue.call.functionName = strdup(functionName);
    if (!inst->instValue.call.functionName) {
        free(inst);
        return NULL;
    }
    return inst;
}

ASMInstruction* createASMDeallocateStackInstruction(int size) {
    ASMInstruction* inst = calloc(1, sizeof(ASMInstruction));
    if (!inst) return NULL;
    inst->type = ASM_BINARY;
    inst->instValue.binary.type = ASM_BINARY_ADD; // Use ADD to represent stack deallocation (e.g., ADD RSP, size)
    inst->instValue.binary.asmType = ASM_QUADWORD; 
    inst->instValue.binary.op2 = createRegisterOperand(SP); 
    inst->instValue.binary.op1 = createImmediateOperand(size);
    return inst;
}

ASMTopLevel* createTopLevel(ASMTopLevelType type, void* value) {
    ASMTopLevel* topLevel = malloc(sizeof(ASMTopLevel));
    if (!topLevel) return NULL;
    topLevel->type = type;
    switch (type) {
        case ASM_TOP_LEVEL_FUNCTION:
            topLevel->topLevel.function = (ASMFunction*)value;
            break;
        case ASM_TOP_LEVEL_STATIC_VAR:
            topLevel->topLevel.staticVar = (ASMStaticVar*)value;
            break;
        case ASM_TOP_LEVEL_STATIC_CONST:
            topLevel->topLevel.staticConst = (ASMStaticConst*)value;
            break;
        default:
            free(topLevel);
            return NULL; 
    }
    return topLevel;
}


ASMTopLevel* createASMTopLevelFromASMFunction(ASMFunction* func) {
    return createTopLevel(ASM_TOP_LEVEL_FUNCTION, func);
}