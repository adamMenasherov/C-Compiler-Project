#include "ASMInstructionFix.h"
#include "../Parser/ASM-Instructions/ASMInstructionsUtilities/ASMInstructionsFree.h"
#include "../DataStructures/HashTable/Wrappers/AsmSymbolTableWrapper.h"
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    ASMInstruction* head;
    ASMInstruction* tail;
} Emitter;

static void emit(Emitter* e, ASMInstruction* inst) {
    if (!inst) return;
    inst->next = NULL;
    if (!e->head) {
        e->head = e->tail = inst;
    } else {
        e->tail->next = inst;
        e->tail = inst;
    }
}

static ASMOperand* reg(int r) { return createRegisterOperand(r); }

static int isMemoryOp(ASMOperand* op) {
    return op && (op->type == ASM_OP_MEMORY || op->type == ASM_OP_DATA);
}

static int isBothMemoryOps(ASMOperand* a, ASMOperand* b) {
    return isMemoryOp(a) && isMemoryOp(b);
}

static int isLargeImmediate(ASMOperand* op) {
    if (!op || op->type != ASM_OP_IMMEDIATE) return 0;
    int64_t val = (int64_t)op->OperandValue.immediate;
    return val < INT32_MIN || val > INT32_MAX;
}

static int isRegister(ASMOperand* op) {
    return op && op->type == ASM_OP_REGISTER;
}

static int isImmediate(ASMOperand* op) {
    return op && op->type == ASM_OP_IMMEDIATE;
}

static int isShiftBinary(ASMBinaryOperator t) {
    return t == ASM_BINARY_SHIFT_LEFT || t == ASM_BINARY_SHIFT_RIGHT;
}

static Register scratchReg1(ASMType t) {
    return t == ASM_DOUBLE ? XMM15 : R10;
}

static Register scratchReg2(ASMType t) {
    return t == ASM_DOUBLE ? XMM14 : R11;
}

static int getPseudoSlotSize(const char* identifier, ASMSymbolTable* asmSymTable) {
    ASMSymTabEntry* entry;
    if (!identifier || !asmSymTable) return 4;

    entry = asmSymbolTableLookup(asmSymTable, identifier);
    if (!entry || entry->entryType != ASM_SYMTAB_OBJ_ENTRY) return 4;
    return entry->objEntry.assemblyType == ASM_LONGWORD ? 4 : 8;
}

static void insertPseudoToTable(CharIntMap* table, char* identifier,
                                int* offset, ASMSymbolTable* asmSymTable) {
    int val, size, alignment, used;
    if (charIntMapGet(table, identifier, &val)) return;

    size = getPseudoSlotSize(identifier, asmSymTable);
    alignment = size;
    used = *offset + size;
    if (used % alignment != 0) {          // align slot to its own size
        used += alignment - (used % alignment);
    }
    charIntMapPut(table, identifier, -used);
    *offset = used;
}

static ASMOperand* resolvePseudo(ASMOperand* operand, CharIntMap* table,
                                 int* offset, ASMSymbolTable* asmSymTable) {
    if (!operand || operand->type != ASM_OP_PSEUDO) return NULL;

    insertPseudoToTable(table, operand->OperandValue.identifier, offset, asmSymTable);
    int stackOffset;
    charIntMapGet(table, operand->OperandValue.identifier, &stackOffset);
    ASMOperand* stackOp = createStackOperand(stackOffset);
    freeASMOperand(operand);
    return stackOp;
}

#define RESOLVE(field) do {                                                  \
    ASMOperand* _r = resolvePseudo((field), table, offset, asmSymTable);     \
    if (_r) (field) = _r;                                                    \
} while (0)

static void fixMov(Emitter* e, ASMInstruction* in) {
    ASMType t = in->instValue.mov.asmType;
    ASMOperand* src = in->instValue.mov.operand1;
    ASMOperand* dst = in->instValue.mov.operand2;

    int largeImmToMem = (t == ASM_QUADWORD) && isLargeImmediate(src) && !isRegister(dst);

    if (isBothMemoryOps(src, dst) || largeImmToMem) {
        emit(e, createMovInstruction(t, src, reg(scratchReg1(t))));
        src = reg(scratchReg1(t));
    }
    emit(e, createMovInstruction(t, src, dst));
}

static void fixMovsx(Emitter* e, ASMInstruction* in) {
    ASMOperand* src = in->instValue.movsx.operand1;
    ASMOperand* dst = in->instValue.movsx.operand2;
    int memDst = isMemoryOp(dst);

    if (isImmediate(src)) {                 // src can't be a constant
        emit(e, createMovInstruction(ASM_LONGWORD, src, reg(R10)));
        src = reg(R10);
    }
    ASMOperand* realDst = memDst ? reg(R11) : dst;
    emit(e, createASMMovsxInstruction(src, realDst));
    if (memDst) emit(e, createMovInstruction(ASM_QUADWORD, reg(R11), dst));  // write back
}

static void fixCVTTSD2SI(Emitter* e, ASMInstruction* in) {
    ASMType dstType = in->instValue.cvttsd2si.dst_type;
    ASMOperand* src = in->instValue.cvttsd2si.src;
    ASMOperand* dst = in->instValue.cvttsd2si.dest;
    if (!isRegister(dst)) {
        emit(e, createCvttsd2siInstruction(dstType, src, reg(R11)));
        emit(e, createMovInstruction(dstType, reg(R11), dst));
    } else {
        emit(e, createCvttsd2siInstruction(dstType, src, dst));
    }
}

static void fixCVTSI2SD(Emitter* e, ASMInstruction* in) {
    ASMType srcType = in->instValue.cvtsi2sd.src_type;
    ASMOperand* src = in->instValue.cvtsi2sd.src;
    ASMOperand* dst = in->instValue.cvtsi2sd.dest;
    if (isImmediate(src)) {
        emit(e, createMovInstruction(srcType, src, reg(R10)));
        src = reg(R10);
    }
    if (!isRegister(dst)) {
        emit(e, createCvtsi2sdInstruction(srcType, src, reg(XMM15)));
        emit(e, createMovInstruction(ASM_DOUBLE, reg(XMM15), dst));
    } else {
        emit(e, createCvtsi2sdInstruction(srcType, src, dst));
    }
}

static void fixLea(Emitter* e, ASMInstruction* in) {
    ASMOperand* src = in->instValue.lea.src;
    ASMOperand* dst = in->instValue.lea.dest;
    if (!isRegister(dst)) {
        emit(e, createLeaInstruction(src, reg(R11)));
        emit(e, createMovInstruction(ASM_QUADWORD, reg(R11), dst));
    } else {
        emit(e, createLeaInstruction(src, dst));
    }
}

static void fixMovZeroExtend(Emitter* e, ASMInstruction* in) {
    ASMOperand* src = in->instValue.movZeroExtend.operand1;
    ASMOperand* dst = in->instValue.movZeroExtend.operand2;
    if (isRegister(dst)) {
        emit(e, createMovInstruction(ASM_LONGWORD, src, dst));
    } else {
        emit(e, createMovInstruction(ASM_LONGWORD, src, reg(R11)));
        emit(e, createMovInstruction(ASM_QUADWORD, reg(R11), dst));
    }
}

static void fixIdiv(Emitter* e, ASMInstruction* in) {
    ASMType t = in->instValue.idiv.asmType;
    ASMOperand* divisor = in->instValue.idiv.divisor;

    if (!isRegister(divisor)) {             // divisor can't be a constant
        emit(e, createMovInstruction(t, divisor, reg(R10)));
        divisor = reg(R10);
    }
    emit(e, createIdivInstruction(divisor, t));
}

static void fixDiv(Emitter* e, ASMInstruction* in) {
    ASMType t = in->instValue.div.asmType;
    ASMOperand* divisor = in->instValue.div.divisor;

    if (!isRegister(divisor)) {             // divisor can't be a constant
        emit(e, createMovInstruction(t, divisor, reg(R10)));
        divisor = reg(R10);
    }
    emit(e, createDivInstruction(divisor, t));
}

static void fixUnary(Emitter* e, ASMInstruction* in) {
    ASMInstruction* copy = calloc(1, sizeof(ASMInstruction));
    *copy = *in;
    emit(e, copy);
}

static void fixShift(Emitter* e, ASMInstruction* in) {
    ASMBinaryOperator op = in->instValue.binary.type;
    ASMType t = in->instValue.binary.asmType;
    ASMOperand* count = in->instValue.binary.op1;
    ASMOperand* dst = in->instValue.binary.op2;

    // shift count must live in CL; preserve CX around the borrow
    int countNeedsCx = !isImmediate(count) &&
                       !(isRegister(count) && count->OperandValue.reg == CX);

    if (countNeedsCx) {
        emit(e, createASMPushInstruction(reg(CX)));
        emit(e, createMovInstruction(ASM_LONGWORD, count, reg(CX)));
        count = reg(CX);
    }
    if (isImmediate(dst)) {                 // dst can't be a constant: use R11, write back
        emit(e, createMovInstruction(t, dst, reg(R11)));
        emit(e, createASMBinaryInstruction(op, t, count, reg(R11)));
        emit(e, createMovInstruction(t, reg(R11), in->instValue.binary.op2));
    } else {
        emit(e, createASMBinaryInstruction(op, t, count, dst));
    }
    if (countNeedsCx) emit(e, createASMPopInstruction(reg(CX)));
}

static void fixBinary(Emitter* e, ASMInstruction* in) {
    ASMBinaryOperator op = in->instValue.binary.type;
    ASMType t = in->instValue.binary.asmType;

    if (isShiftBinary(op)) { fixShift(e, in); return; }

    ASMOperand* op1 = in->instValue.binary.op1;
    ASMOperand* op2 = in->instValue.binary.op2;

    if (t == ASM_QUADWORD && isLargeImmediate(op1)) {   // 64-bit imm must go via reg
        emit(e, createMovInstruction(ASM_QUADWORD, op1, reg(R10)));
        op1 = reg(R10);
    }
    if (isBothMemoryOps(op1, op2)) {        // no mem,mem form
        emit(e, createMovInstruction(t, op1, reg(scratchReg1(t))));
        op1 = reg(scratchReg1(t));
    }
    if ((op == ASM_BINARY_MULTIPLY || t == ASM_DOUBLE) && isMemoryOp(op2)) {
        emit(e, createMovInstruction(t, op2, reg(scratchReg2(t))));
        emit(e, createASMBinaryInstruction(op, t, op1, reg(scratchReg2(t))));
        emit(e, createMovInstruction(t, reg(scratchReg2(t)), op2));
        return;
    }
    emit(e, createASMBinaryInstruction(op, t, op1, op2));
}

static void fixCmp(Emitter* e, ASMInstruction* in) {
    ASMType t = in->instValue.cmp.asmType;
    ASMOperand* op1 = in->instValue.cmp.op1;
    ASMOperand* op2 = in->instValue.cmp.op2;

    if (t == ASM_QUADWORD && isLargeImmediate(op1)) {   // 64-bit imm must go via reg
        emit(e, createMovInstruction(ASM_QUADWORD, op1, reg(R10)));
        op1 = reg(R10);
    }
    if (isBothMemoryOps(op1, op2)) {        // no mem,mem form
        emit(e, createMovInstruction(t, op1, reg(scratchReg1(t))));
        op1 = reg(scratchReg1(t));
    }
    if (!isRegister(op2)) {                 // second operand must be a register
        emit(e, createMovInstruction(t, op2, reg(scratchReg2(t))));
        op2 = reg(scratchReg2(t));
    }
    emit(e, createASMCmpInstruction(t, op1, op2));
}

static void fixSetcc(Emitter* e, ASMInstruction* in) {
    emit(e, createASMSetCCInstruction(in->instValue.setcc.cond,
                                   in->instValue.setcc.op));
}

static void fixPush(Emitter* e, ASMInstruction* in) {
    emit(e, createASMPushInstruction(in->instValue.push.op));
}

static void fixPop(Emitter* e, ASMInstruction* in) {
    emit(e, createASMPopInstruction(in->instValue.pop.op));
}

static void fixPassthrough(Emitter* e, ASMInstruction* in) {
    ASMInstruction* copy = calloc(1, sizeof(ASMInstruction));
    *copy = *in;                            // labels/jumps/ret/cdq need no rewrite
    emit(e, copy);
}

void pseudoToStackPositions(ASMInstructionList* instList, CharIntMap* table,
                            ASMSymbolTable* asmSymTable, SymbolTable* symTable) {
    (void)symTable;
    int usedStackBytes = 0;
    int* offset = &usedStackBytes;          // alias for the RESOLVE macro
    Emitter e = { NULL, NULL };
    ASMInstruction* in;

    for (in = instList->head; in != NULL; in = in->next) {
        switch (in->type) {
            case ASM_MOV:
                RESOLVE(in->instValue.mov.operand1);
                RESOLVE(in->instValue.mov.operand2);
                fixMov(&e, in);
                break;
            case ASM_MOVSX:
                RESOLVE(in->instValue.movsx.operand1);
                RESOLVE(in->instValue.movsx.operand2);
                fixMovsx(&e, in);
                break;
            case ASM_MOVZEROEXTEND:
                RESOLVE(in->instValue.movZeroExtend.operand1);
                RESOLVE(in->instValue.movZeroExtend.operand2);
                fixMovZeroExtend(&e, in);
                break;
            case ASM_IDIV:
                RESOLVE(in->instValue.idiv.divisor);
                fixIdiv(&e, in);
                break;
            case ASM_DIV:
                RESOLVE(in->instValue.div.divisor);
                fixDiv(&e, in);
                break;
            case ASM_UNARY:
                RESOLVE(in->instValue.unary.op);
                fixUnary(&e, in);
                break;
            case ASM_BINARY:
                RESOLVE(in->instValue.binary.op1);
                RESOLVE(in->instValue.binary.op2);
                fixBinary(&e, in);
                break;
            case ASM_CVTTSD2SI:
                RESOLVE(in->instValue.cvttsd2si.src);
                RESOLVE(in->instValue.cvttsd2si.dest);
                fixCVTTSD2SI(&e, in);
                break;
            case ASM_CVTSI2SD:
                RESOLVE(in->instValue.cvtsi2sd.src);
                RESOLVE(in->instValue.cvtsi2sd.dest);
                fixCVTSI2SD(&e, in);
                break;
            case ASM_CMP:
                RESOLVE(in->instValue.cmp.op1);
                RESOLVE(in->instValue.cmp.op2);
                fixCmp(&e, in);
                break;
            case ASM_SETCC:
                RESOLVE(in->instValue.setcc.op);
                fixSetcc(&e, in);
                break;
            case ASM_LEA:
                RESOLVE(in->instValue.lea.src);
                RESOLVE(in->instValue.lea.dest);
                fixLea(&e, in);  
                break;
            case ASM_PUSH:
                RESOLVE(in->instValue.push.op);
                fixPush(&e, in);
                break;
            case ASM_POP:
                RESOLVE(in->instValue.pop.op);
                fixPop(&e, in);
                break;
            default:
                fixPassthrough(&e, in);
                break;
        }
    }

    charIntMapPrint(table);

    instList->head = e.head;                // swap in the rebuilt list
    addASMInstructionAtBeginning(
        instList,
        createASMAllocateStackInstruction(fixStackSizeForFunction(usedStackBytes)));
}

int fixStackSizeForFunction(int stackSize) {
    if (stackSize <= 0) return 0;
    int rem = stackSize % 16;
    return rem == 0 ? stackSize : stackSize + (16 - rem);   // round up to 16
}