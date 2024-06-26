#pragma once

#include "lib.h"

#define LEX_BUFFER_SIZE 8

typedef enum {
    IDENT,
    INTEGER,
    FLOAT,

    FN,
    STRUCT,
    F32, F64,
    U8, U16, U32, U64,
    I8, I16, I32, I64,
    VEC2, VEC3, VEC4,
    MAT2, MAT3, MAT4,
    VOID,

    BOOL, BTRUE, BFALSE,

    IF, ELSE,

    FOR, IN,

    RETURN, CONTINUE, BREAK,

    RPAREN,
    RBRACE,
    LBRACK,
    RBRACK,

    COLON,
    ARROW,
    COMMA,

    CONST_ASGN,
    ASGN,

    SEMI,

    // Operators
 // +           +=             &&
    ADD,        ADD_ASGN,
    SUB,        SUB_ASGN,
    MUL,        MUL_ASGN,
    DIV,        DIV_ASGN,
    MOD,        MOD_ASGN,
    OR_BIT,     OR_ASGN,       OR,
    AND_BIT,    AND_ASGN,      AND,
    // BIT_XOR,    BIT_XOR_ASGN,
    NOT_BIT, NOT,

    DOT,    // Dereference operator
    LPAREN, // Call operator
    LBRACE, // Index operator

    // Comparisons
    EQ,
    NEQ,
    LT,
    LEQ,
    GT,
    GEQ,

    EOF_TOK,
} TokenType;

struct Token {
    TokenType type;

    u32 line;
    u32 column;
    u32 index; // byte index into the file this came from
    
    union 
    {
        char* str;
        u64 uint;
        f64 fpoint;
    };
};
typedef struct Token Token;

typedef struct ident {
    char* key;
    u32 value;
} Ident;

struct Lex {
    Str buffer;
    u32 index;

    Token tkbuf[LEX_BUFFER_SIZE];
    u8 tkWrite;
    u8 tkRead;

    // Cursor position
    u32 curLn;
    u32 curCol;

    Ident* strings;
};
typedef struct Lex Lex;

Lex* lex_start(char* file);
void lex_end(Lex* l);

Token lex_eat(Lex* l);
Token lex_peek(Lex* l, u8 offset);
void lex_skip(Lex* l, u8 offset);
