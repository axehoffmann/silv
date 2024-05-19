#include "lib.h"


typedef enum {
    OP_CPY_REG,

    OP_LOADCONST,

    // Memory
    OP_LOAD,
    OP_STORE,

    // Arithmetic - has signed, unsigned, fpoint variants.
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_LT,
    OP_LEQ,
    OP_GT,
    OP_GEQ,
    OP_EQ,
    OP_NEQ,
    
    OP_NEG,

    // Logical
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_NOT,

    OP_SHL,
    OP_SHR,

    OP_CALL,
    OP_RET,
    OP_JMP,
    OP_CJMP,

    // TODO?
    // XCHG
} Opcode;

typedef struct bc_instruction {
    Opcode op;
    u32 flags;

    u32 regOut;
    
    union {
        struct {
            u32 regA;
            u32 regB;
        };

        void* ptr;
        f64 fpoint;
        u64 uint;
        i64 sint;
    };
} Inst;

