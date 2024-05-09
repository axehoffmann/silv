#include "ast.h"
#include "lex.h"
#include "compiler_util.h"
#include "alloc.h"

#include <assert.h>

#define require(TK, message) {                                  \
    Token tk = lex_peek(p->l, 0);                                  \
    if (tk.type != TK) {                                        \
        fprintf(stderr, "\nError on line %u (column %u): %s",   \
                tk.line, tk.column, message);                   \
        errloc(p->l->buffer.data, tk.index, tk.line);              \
    }}

typedef struct decl_entry {
    char* key;
    ast_base* node;
} decl_entry;

typedef struct parse_state {
    Lex* l;

    decl_entry* declarations;

    Arena* arena;
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

AstStructLiteral* parse_struct_literal(Parse* p) {
    return NULL;
}

AstProc* parse_proc(Parse* p) {
    return NULL;
}

AstType* parse_type(Parse* p) {
    return NULL;
}

AstConstant* parse_constant(Parse* p) {
    Token tk = lex_eat(p->l);
    AstConstant* node = arena_alloc(p->arena, AstConstant);
    // #TODO: signed integers
    assert(node);
    switch (tk.type) {
    case INTEGER:
        node->valueType = VALUE_UINT;
        node->uint = tk.uint;
    case FLOAT:
        node->valueType = VALUE_FLOAT;
        node->fpoint = tk.fpoint;

    default:
        assert(false);
    }
}

AstDecl* parse_decl(Parse* p) {
    Token tk = lex_eat(p->l);
    assert(tk.type == IDENT);
    
    AstDecl* node = arena_alloc(p->arena, AstDecl);
    assert(node);
    
    node->name = tk.str;

    require(CONST_ASGN, "Top-level declarations must use constant-syntax, i.e. x :: 10");
    lex_skip(p->l, 1);

    tk = lex_peek(p->l, 0);
    ast_base* def = NULL;
    switch (tk.type) {
    case LPAREN:
        def = &parse_proc(p)->base; break;
    case DOT:
        def = &parse_struct_literal(p)->base; break;
    case INTEGER:
    case FLOAT:
        def = &parse_constant(p)->base; break;
    }
}

ast_base* parse_one(Parse* p) {
    Token tk = lex_peek(p->l, 0);
    switch (tk.type) {
    case IDENT:
        return &parse_decl(p)->base;
    case EOF_TOK:
        return NULL;
    default:
        errstart(tk.line, tk.column);
        printf("Expected a name to start a declaration\n");
        errloc(p->l->buffer.data, tk.index, tk.line);
        return NULL;
    }
}
