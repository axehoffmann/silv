#include "ast.h"
#include "lex.h"
#include "compiler_util.h"

#include <assert.h>

#define require(TK, message) {                                  \
    Token tk = lex_peek(l, 0);                                  \
    if (tk.type != TK) {                                        \
        fprintf(stderr, "\nError on line %u (column %u): %s",   \
                tk.line, tk.column, message);                   \
        errloc(l->buffer.data, tk.index, tk.line);              \
    }}

typedef struct decl_entry {
    char* key;
    ast_base* node;
} decl_entry;

typedef struct parse_state {
    Lex* l;

    decl_entry* declarations;

    usize arenaSize;
    usize arenaIt;
    void* arena;
} Parse;

Parse* parse_begin(Lex* l) {

}

void parse_end(Parse* p) {

}

typedef struct precedence {
    i8 l, r;
} precedence;

precedence infix_precedence(i32 op) {
    switch (op)
    {
    case AND:
    case OR:
        return (precedence){ 1, 2 };
    case EQ:
    case NEQ:
    case LT:
    case LEQ:
    case GT:
    case GEQ:
        return (precedence){ 3, 4 };
    case ADD:
    case SUB:
        return (precedence){ 5, 6 };
    case MUL:
    case DIV:
    case MOD:
        return (precedence){ 7, 8 };
    case OR_BIT:
    case AND_BIT:
        return (precedence){ 9, 10 };
    default:
        return (precedence){ -1, -1 };
    }
};

precedence prefix_precedence(i32 op) {
    switch (op) {
    case NOT:
        return (precedence){ -1, 11 };
    case NOT_BIT:
        return (precedence){ -1, 11};
    default:
        assert(false);
        exit(420);
    }
}

precedence postfix_precedence(i32 op) {
    switch (op) {
    case LBRACK:
        return (precedence){ 12, -1 };
    default:
        assert(false);
        exit(69);
    }
}

AstProc* parse_proc(Parse* p) {
    return NULL;
}

AstDecl* parse_decl(Parse* p) {

}

ast_base* parse_one(Parse* p) {
    switch (lex_peek(p->l, 0).type) {
    case IDENT:
        return &parse_decl(p)->base;
    case EOF_TOK:
        return NULL;
    }
}
