#include "lib.h"

#define LEX_BUFFER_SIZE 8

struct Token {
    enum Type {
        IDENT,
        INTEGER,
        FLOAT,

        FN,
        STRUCT,

        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACK,
        RBRACK,

        COLON,
        ARROW,
        COMMA,

        CONST_DECL,
        DECL,
        ASGN,

        DOT,
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

        // Comparisons
        EQ,
        NEQ,
        LT,
        LEQ,
        GT,
        GEQ,

        EOF_TOK,
    };
    enum Type type;

    u32 line;
    u32 column;
    u32 index; // byte index into the file this came from
    
    union 
    {
        Str str;
        u64 uint;
        f64 fpoint;
    };
};
typedef struct Token Token;

struct Lex {
    Str buffer;
    u64 index;

    Token tkbuf[LEX_BUFFER_SIZE];
    u8 tkWrite;
    u8 tkRead;

    // Cursor position
    u32 curLn;
    u32 curCol;
};
typedef struct Lex Lex;

Lex* lex_start(char* file);
void lex_end(Lex* l);

Token lex_eat(Lex* l);
Token lex_peek(Lex* l, u8 offset);
void lex_skip(Lex* l, u8 offset);
