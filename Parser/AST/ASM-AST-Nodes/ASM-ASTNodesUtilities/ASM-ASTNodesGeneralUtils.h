#pragma once
#include "../../C-AST-Nodes/C-ASTNodes.h"
#include "../../../TACKY/TACKY_AST.h"
#include "../../../../DataStructures/Map/Wrappers/CharIntMap.h"

/* ============================================================
 * Enum Definitions
 * ============================================================ */

typedef enum {
    ASM_UNARY,
    ASM_ALLOCATESTACK,
    ASM_MOV,
    ASM_BINARY,
    ASM_CDQ,
    ASM_IDIV,
    ASM_LABEL,
    ASM_SETCC,
    ASM_JUMP,
    ASM_CMP,
    ASM_PUSH,
    ASM_POP,
    ASM_JUMPCC,
    ASM_RET
} ASMInstructionType;

typedef enum {
    ASM_UNARY_NEG,
    ASM_UNARY_NOT,
    ASM_UNARY_DEC,
    ASM_UNARY_INC
} ASMUnaryOperator;

typedef enum {
    ASM_BINARY_ADD,
    ASM_BINARY_SUBTRACT,
    ASM_BINARY_MULTIPLY,
    ASM_BINARY_XOR,
    ASM_BINARY_AND,
    ASM_BINARY_OR,
    ASM_BINARY_SHIFT_LEFT,
    ASM_BINARY_SHIFT_RIGHT
} ASMBinaryOperator;

typedef enum {
    ASM_COND_CODE_E,
    ASM_COND_CODE_NE,
    ASM_COND_CODE_L,
    ASM_COND_CODE_LE,
    ASM_COND_CODE_G,
    ASM_COND_CODE_GE
} ASMCondCode;

typedef enum {
    ASM_OP_REGISTER,
    ASM_OP_IMMEDIATE,
    ASM_OP_PSEUDO,
    ASM_OP_STACK
} OperandType;

typedef enum {
    AX,
    CX,
    DX,
    R10,
    R11
} Register;

/* ============================================================
 * Type Definitions / Structs
 * ============================================================ */

typedef struct {
    OperandType type;
    union {
        Register reg;
        int immediate;
        char* identifier;
    } OperandValue;
} ASMOperand;


typedef struct ASMInstruction {
    ASMInstructionType type;
    union {
        struct {
            ASMOperand* operand1;
            ASMOperand* operand2;
        } mov;
        struct {
            ASMUnaryOperator type;
            ASMOperand* op;
        } unary;
        struct {
            int size;
        } allocatestack;
        struct {
            ASMBinaryOperator type;
            ASMOperand* op1;
            ASMOperand* op2;
        } binary;
        struct {
            ASMOperand* divisor;
        } idiv;
        struct {
            char* identifier;
        } label;
        struct {
            ASMOperand* op1;
            ASMOperand* op2;
        } cmp;
        struct {
            char * label;
        } jmp;
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
} ASMFunction;


typedef struct {
    ASMFunction* function_def;
} ASMProgram;


/* ============================================================
 * Utility Function Declarations
 * ============================================================ */

/**
 * @brief Checks if the given binary operation type is a relational operator (e.g. <, <=, >, >=, ==, !=).
 * 
 * @param type The binary operation type to check 
 * @return int Returns 1 if the type is a relational operator, 0 otherwise
 */
int isRelationalOp(binType type);

/**
 * @brief Get the Cond Code For Relational Op object
 * 
 * @param type The binary operation type for which to get the corresponding condition code
 * @return ASMCondCode The condition code corresponding to the given relational operator type
 */
ASMCondCode getCondCodeForRelationalOp(binType type);
