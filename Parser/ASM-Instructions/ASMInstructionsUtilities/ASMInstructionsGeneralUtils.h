#pragma once
#include "../../Common/SharedNodeEnums.h"
#include "../../AST/C-AST-Nodes/C-ASTNodes.h"
#include "../../TACKY/TACKYProgram.h"
#include "../../../DataStructures/Map/Wrappers/CharIntMap.h"
#include "../../../DataStructures/Map/Wrappers/DoubleStringMap.h"
#include "../../../DataStructures/DynamicArray/Wrappers/ASMTopLevelArrayWrapper.h"

/* ============================================================
 * Enum Definitions
 * ============================================================ */

typedef enum {
    ASM_UNARY,
    ASM_ALLOCATESTACK,
    ASM_DEALLOCATESTACK,
    ASM_MOV,
    ASM_MOVSX,
    ASM_MOVZEROEXTEND,
    ASM_BINARY,
    ASM_CDQ,
    ASM_IDIV,
    ASM_DIV,
    ASM_LABEL,
    ASM_SETCC,
    ASM_CVTTSD2SI,
    ASM_CVTSI2SD,
    ASM_JUMP,
    ASM_CMP,
    ASM_PUSH,
    ASM_POP,
    ASM_JUMPCC,
    ASM_CALL,
    ASM_RET
} ASMInstructionType;

typedef enum {
    ASM_UNARY_NEG,
    ASM_UNARY_NOT,
    ASM_UNARY_DEC,
    ASM_UNARY_INC,
    ASM_UNARY_SHR
} ASMUnaryOperator;

typedef enum {
    ASM_BINARY_ADD,
    ASM_BINARY_SUBTRACT,
    ASM_BINARY_MULTIPLY,
    ASM_BINARY_XOR,
    ASM_BINARY_AND,
    ASM_BINARY_OR,
    ASM_BINARY_SHIFT_LEFT,
    ASM_BINARY_SHIFT_RIGHT,
    ASM_BINARY_DIVDOUBLE,
} ASMBinaryOperator;

typedef enum {
    ASM_COND_CODE_E,
    ASM_COND_CODE_NE,
    ASM_COND_CODE_L,
    ASM_COND_CODE_LE,
    ASM_COND_CODE_G,
    ASM_COND_CODE_GE,
    ASM_COND_CODE_A,
    ASM_COND_CODE_AE,
    ASM_COND_CODE_B,
    ASM_COND_CODE_BE
} ASMCondCode;

typedef enum {
    ASM_OP_REGISTER,
    ASM_OP_IMMEDIATE,
    ASM_OP_PSEUDO,
    ASM_OP_STACK,
    ASM_OP_DATA
} OperandType;

typedef enum {
    ASM_LONGWORD,
    ASM_QUADWORD,
    ASM_DOUBLE
} ASMType;

typedef enum {
    ASM_TOP_LEVEL_FUNCTION,
    ASM_TOP_LEVEL_STATIC_VAR,
    ASM_TOP_LEVEL_STATIC_CONST
} ASMTopLevelType;

typedef enum {
    AX,
    CX,
    DX,
    DI,
    SI,
    SP,
    R8,
    R9,
    R10,
    R11,
    XMM0,
    XMM1,
    XMM2,
    XMM3,
    XMM4,
    XMM5,
    XMM6,
    XMM7,
    XMM14,
    XMM15
} Register;

extern const int argResigters[];
extern const int doubleArgRegisters[];

typedef struct {
    OperandType type;
    union {
        Register reg;
        uint64_t immediate;
        char* identifier;
    } OperandValue;
} ASMOperand;


typedef struct ASMInstruction {
    ASMInstructionType type;
    union {
        struct {
            ASMType asmType;
            ASMOperand* operand1;
            ASMOperand* operand2;
        } mov;
        struct {
            ASMOperand* operand1;
            ASMOperand* operand2;
        } movsx;
        struct {
            ASMOperand* operand1;
            ASMOperand* operand2;
        } movZeroExtend;
        struct {
            ASMType asmType;
            ASMUnaryOperator type;
            ASMOperand* op;
        } unary;
        struct {
            int size;
        } allocatestack;
        struct {
            int size;
        } deallocatestack;
        struct {
            ASMType asmType;
            ASMBinaryOperator type;
            ASMOperand* op1;
            ASMOperand* op2;
        } binary;
        struct {
            ASMType asmType;
            ASMOperand* divisor;
        } idiv;
        struct {
            ASMType asmType;
            ASMOperand* divisor;
        } div;
        struct {
            char* identifier;
        } label;
        struct {
            ASMType asmType;
            ASMOperand* op1;
            ASMOperand* op2;
        } cmp;
        struct {
            char * label;
        } jmp;
        struct {
            ASMType asmType;
        } cdq;
        struct {
            ASMCondCode cond;
            char* label;
        } jumpcc;
        struct {
            ASMCondCode cond;
            ASMOperand* op;
        } setcc;
        struct {
            ASMOperand* op;
        } push;
        struct {
            ASMOperand* op;
        } pop;
        struct {
            char* functionName;
        } call;
        struct {
            ASMType dst_type;
            ASMOperand* src;
            ASMOperand* dest;
        } cvttsd2si;
        struct {
            ASMType src_type;
            ASMOperand* src;
            ASMOperand* dest;
        } cvtsi2sd;
    } instValue;
    struct ASMInstruction* next;
} ASMInstruction;

typedef struct {
    ASMInstruction* head;
    ASMInstruction* tail;
} ASMInstructionList;

typedef struct ASMFunction {
    char* function_name;
    ASMInstructionList* inst;
    CharIntMap* pseudoTable; // Maps pseudo-register names to stack offsets
    int global;
} ASMFunction;

typedef struct {
    char* identifier;
    int global; 
    int alignment;
    staticInitialVal initVal;   
} ASMStaticVar;

typedef struct {
    char* identifier;
    int alignment;
    staticInitialVal initVal;
} ASMStaticConst;

typedef struct ASMTopLevel {
    ASMTopLevelType type;
    union {
        ASMStaticVar* staticVar;
        ASMFunction* function;
        ASMStaticConst* staticConst;
    } topLevel;
} ASMTopLevel;


typedef struct {
    ASMTopLevelArray* topLevels;
} ASMProgram;


/**
 * @brief Get the Cond Code For Relational Op object
 * 
 * @param type The binary operation type for which to get the corresponding condition code
 * @return ASMCondCode The condition code corresponding to the given relational operator type
 */
ASMCondCode getCondCodeForRelationalOp(binType type, int isSigned);
ASMType convertTACKYTypeToASMType(TACKYValue* val, SymbolTable* symTable);
ASMType convertSpecifierTypeToASMType(specifierType type);
ASMType convertIdentifierTypeToASMType(IdentifierTypeInfo* info);
void addConstantsAsTopLevels(ASMProgram* asmProgram, DoubleStringMap* cache);
int signedOrUnsigned(TACKYInstruction* instruction, SymbolTable* symTable);
int isSignedTACKYValue(TACKYValue* val, SymbolTable* symTable);
int isDoubleOperand(TACKYValue* val, SymbolTable* symTable);