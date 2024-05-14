#pragma once

#include "lib.h"

#define VALUE_IDENT 0
#define VALUE_STRING 1
#define VALUE_UINT 2
#define VALUE_SINT 3
#define VALUE_FLOAT 4
#define VALUE_BOOLEAN 5

typedef struct ast_block AstBlock;
typedef struct ast_constant AstConstant;
typedef struct ast_memory AstMemory;
typedef struct ast_binop AstBinOp;
typedef struct ast_decl AstDecl;
typedef struct ast_assign AstAssign;
typedef struct ast_if AstIf;
typedef struct ast_array AstArray;
typedef struct ast_struct_literal AstStructLiteral;
typedef struct ast_call AstCall;
typedef struct ast_proc AstProc;

typedef struct ast_type AstType;

enum ast_node_type {
    AST_BLOCK,
    AST_CONSTANT,
    AST_MEMORY,
    AST_BINOP,
    AST_DECL,
    AST_ASSIGN,
    AST_IF,
    AST_ARRAY,
    AST_STRUCT_LITERAL,
    AST_CALL,
    AST_PROC,
    AST_STRUCT,
    AST_TYPE,
};

typedef struct ast_base {
    i32 nodeType;
    u32 sourceIndex; // The index at which this node appears in the source file
} ast_base;

typedef struct ast_block {
    ast_base base;

    ast_base** statements; // Note this is an stb_ds dynamic array
} AstBlock;

typedef struct ast_constant {
    ast_base base;

    u8 valueType;
    union {
        char* str;
        u64 uint;
        i64 sint;
        f64 fpoint;
        bool boolean;
    };
} AstConstant;

// @TODO: rename to identifer, its not necesarrily memory. operator overloads and such
typedef struct ast_memory {
    ast_base base;

    Str name;
} AstMemory;

typedef struct ast_binop {
    ast_base base;    

    i32 opTk; // The operator token type
    ast_base* lhs;
    ast_base* rhs;
} AstBinOp;

typedef struct ast_decl {
    ast_base base;
    
    Str name;
    AstType* type;
    ast_base* rhs;
    
} AstDecl;

typedef struct ast_assign {
    ast_base base;

    ast_base* lhs;
    ast_base* rhs;
} AstAssign;

typedef struct ast_if {
    ast_base base;

    ast_base* condition;
    AstBlock* iftrue;
    ast_base* iffalse;
} AstIf;

typedef struct ast_value_list {
    ast_base base;

    ast_base* exprs;
} AstValueList;

typedef struct ast_array {
    ast_base base;

    AstType* type;
    AstValueList* values;
} AstArray;

typedef struct ast_struct_literal {
    ast_base base;

    AstType* type;
    AstValueList* values;
} AstStructLiteral;

typedef struct ast_call {
    ast_base base;

    Str procName;
    ast_base** args;
} AstCall;

typedef struct ast_proc {
    ast_base base;

    Str name;
    AstDecl** parameters;
    AstType* returnType;
    AstBlock* block;
} AstProc;

typedef struct ast_struct {
    ast_base base;

    Str name;
    AstDecl* members;
} AstStruct;

typedef struct ast_type {
    ast_base base;

    i32 typeID; // Currently just the token type.
} AstType;

typedef struct parse_state Parse;
typedef struct Lex Lex;

Parse* parse_begin(Lex* l);
void parse_end(Parse* p);
ast_base* parse_one(Parse* p);

void print_expr(ast_base* node);
