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
typedef struct ast_unop AstUnOp;
typedef struct ast_decl AstDecl;
typedef struct ast_if AstIf;
typedef struct ast_array AstArray;
typedef struct ast_struct_literal AstStructLiteral;
typedef struct ast_call AstCall;
typedef struct ast_proc AstProc;
typedef struct ast_return AstReturn;

typedef struct ast_type AstType;

typedef struct ast_scope AstScope;

typedef enum {
    AST_BLOCK,
    AST_CONSTANT,
    AST_MEMORY,
    AST_BINOP,
    AST_UNOP,
    AST_DECL,
    AST_IF,
    AST_ARRAY,
    AST_STRUCT_LITERAL,
    AST_CALL,
    AST_RETURN,

    AST_PROC,
    AST_TYPE,
    AST_STRUCT,
} ast_node_type;

typedef struct symbol {
    ast_node_type type; // PROC/DECL
    // @TODO: make functions decls too so they don't need roundabout stuff
    union {
        AstDecl* decl;
        AstProc* proc;
    };
} Symbol;

typedef struct st_entry {
    char* key;
    bool declared; // Used when walking the tree during typechecking to
                   // ensure declaration before use
    Symbol symbol;
} SymbolEntry;


typedef struct ast_scope {
    SymbolEntry* symbols; // Hashmap
    AstScope* parent;
} AstScope;

typedef struct ast {
    ast_node_type nodeType;
    u32 sourceIndex; // The index at which this node appears in the source file
} ast;

typedef struct ast_block {
    ast base;

    ast** statements; // Note this is an stb_ds dynamic array

    AstScope* scope;
} AstBlock;

typedef struct ast_constant {
    ast base;

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
    ast base;

    char* name;
} AstMemory;

typedef struct ast_binop {
    ast base;    

    i32 opTk; // The operator token type
    ast* lhs;
    ast* rhs;
} AstBinOp;

typedef struct ast_unop {
    ast base;

    i32 opTk;
    ast* rest;
} AstUnOp;

typedef struct ast_decl {
    ast base;
    
    char* name;
    AstType* type;
    ast* rhs;
    
} AstDecl;

typedef struct ast_if {
    ast base;

    ast* condition;
    AstBlock* iftrue;
    ast* iffalse;
} AstIf;

typedef struct ast_value_list {
    ast base;

    ast* exprs;
} AstValueList;

typedef struct ast_array {
    ast base;

    AstType* type;
    AstValueList* values;
} AstArray;

typedef struct ast_struct_literal {
    ast base;

    AstType* type;
    AstValueList* values;
} AstStructLiteral;

typedef struct ast_call {
    ast base;

    enum {
        CALL_NOFLAGS        = 0,
        CALL_ARRAY_INDEX    = 0b1,
    } flags;
    
    ast* lhs;
    ast** args;

    AstProc* resolvedProc;
} AstCall;

typedef struct ast_proc {
    ast base;

    char* name;
    AstDecl** parameters;
    AstType* returnType;
    AstBlock* block;

    AstScope* scope;
} AstProc;

typedef struct ast_struct {
    ast base;

    char* name;
    AstDecl* members;
} AstStruct;

typedef struct ast_type {
    ast base;

    i32 typeID; // Currently just the token type.
} AstType;

typedef struct ast_return {
    ast base;

    ast* value;
} AstReturn;

typedef struct parse_state Parse;
typedef struct Lex Lex;

Parse* parse_begin(Lex* l);
void parse_end(Parse* p);
ast* parse_one(Parse* p);

Symbol* find_symbol(AstScope* scope, char* ident);

void print_expr(ast* node);
