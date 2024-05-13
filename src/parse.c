#include "ast.h"
#include "lex.h"
#include "compiler_util.h"
#include "alloc.h"

#include <assert.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define require(TK, message) {                                  \
    Token tk = lex_peek(p->l, 0);                               \
    if (tk.type != TK) {                                        \
        fprintf(stderr, "\nError on line %u (column %u): %s",   \
                tk.line, tk.column, message);                   \
        errloc(p->l->buffer.data, tk.index, tk.line);           \
    }}

#define require_s(TK, message)                  \
    require(TK, message)                        \
    lex_skip(p->l, 1)

typedef struct decl_entry {
    char* key;
    ast_base* node;
} decl_entry;

typedef struct parse_state {
    Lex* l;

    decl_entry* declarations;

    Arena* arena;
} Parse;

AstConstant* parse_constant(Parse* p) {
    Token tk = lex_eat(p->l);
    AstConstant* node = arena_alloc(p->arena, AstConstant);
    node->base = (ast_base){ AST_CONSTANT, tk.index };

    // #TODO: signed integers
    assert(node);
    switch (tk.type) {
    case INTEGER:
        node->valueType = VALUE_UINT;
        node->uint = tk.uint;
        break;
    case FLOAT:
        node->valueType = VALUE_FLOAT;
        node->fpoint = tk.fpoint;
        break;
    case BTRUE:
    case BFALSE:
        node->valueType = VALUE_BOOLEAN;
        node->boolean = tk.type == BTRUE;
        break;
    default:
        assert(false);
    }

    return node;
}

ast_base* parse_value(Parse* p) {
    if (lex_peek(p->l, 0).type == IDENT) {
        Token tk = lex_eat(p->l);
        AstMemory* node = arena_alloc(p->arena, AstMemory);
        node->base = (ast_base){ AST_MEMORY, tk.index };

        node->name = tk.str;
        return &node->base;
    }

    return &parse_constant(p)->base;
}

Parse* parse_begin(Lex* l) {
    Parse* p = new(Parse);
    p->l = l;
    p->declarations = NULL;
    p->arena = arena_new(1048576); // 1MB
    return p;
}

void parse_end(Parse* p) {
    arena_free(p->arena);
    free(p);
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


ast_base* parse_expr(Parse* p, u8 curPrecedence) {
    ast_base* node = parse_value(p);

    // Pratt expression parser
    while (true) {
        Token op = lex_peek(p->l, 0);

        if (op.type < ADD || op.type > GEQ)
            break; // end of expression chain, not an operator

        precedence prec = infix_precedence(op.type);
        if (prec.l < curPrecedence)
            break;

        lex_skip(p->l, 1);

        AstBinOp* opnode = arena_alloc(p->arena, AstBinOp);
        opnode->base = (ast_base){ AST_BINOP, op.index };
        opnode->opTk = op.type;
        opnode->lhs = node;
        opnode->rhs = parse_expr(p, prec.r);
        node = &opnode->base;
    }

    return node;
}

AstStructLiteral* parse_struct_literal(Parse* p) {
    return NULL;
}

AstType* parse_type(Parse* p) {
    // #TODO :)
    Token type = lex_eat(p->l);
    AstType* node = arena_alloc(p->arena, AstType);
    node->base = (ast_base){ AST_TYPE, type.index };

    node->typeID = type.type;

    return node;
}

AstCall* parse_call(Parse* p) {
    Token ident = lex_eat(p->l);
    // #TODO: call needs to count as an operator probably, so does indexing
    // otherwise mystruct.myfn() won't work
    AstCall* node = arena_alloc(p->arena, AstCall);
    node->base = (ast_base){ AST_CALL, ident.index };
    node->procName = ident.str;
    node->args = NULL;
    lex_eat(p->l); // l paren
    
    while (lex_peek(p->l, 0).type != RPAREN) {
        ast_base* expr = parse_expr(p, 0);
        arrput(node->args, expr);

        if (lex_peek(p->l, 0).type == COMMA)
            lex_skip(p->l, 1);
    }
    lex_eat(p->l); // r paren
    return NULL;
}

AstDecl* parse_single_decl(Parse* p) {
    Token ident = lex_peek(p->l, 0);
    require_s(IDENT, "A declaration must begin with an identifier");

    AstDecl* node = arena_alloc(p->arena, AstDecl);
    node->base = (ast_base){ AST_DECL, ident.index };
    require_s(COLON, "Parameter declarations must be of the form 'name: type'");
    node->name = ident.str;
    node->type = parse_type(p);
    node->rhs = NULL;
    node->next = NULL;

    return node;
}

AstDecl* parse_parameters(Parse* p) {
    AstDecl* first = parse_single_decl(p);
    AstDecl* current = first;

    while (lex_peek(p->l, 0).type == COMMA) {
        lex_skip(p->l, 1);

        AstDecl* next = parse_single_decl(p);
        current->next = next;
        current = next;
    }

    return first;
}

AstAssign* parse_asgn(Parse* p) {
    Token tk = lex_peek(p->l, 0);

    AstAssign* node = arena_alloc(p->arena, AstAssign);
    node->base = (ast_base){ AST_ASSIGN, tk.index };

    node->lhs = parse_expr(p, 0);
    require_s(ASGN, "An assignment must use the = sign.");
    node->rhs = parse_expr(p, 0);

    return NULL;
}

ast_base* parse_stmt(Parse* p) {
    ast_base* node = NULL;
    switch (lex_peek(p->l, 1).type) {
    case LPAREN:
        node = &parse_call(p)->base;
        break;
    case COLON:
        node = &parse_single_decl(p)->base;
        break;
    case ASGN:
        node = &parse_asgn(p)->base;
        break;
    }

    require_s(SEMI, "Expected a semicolon trailing a statement");
    return node;
}

AstIf* parse_if(Parse* p);

// Different path for parsing a code block with a single statement + no braces
AstBlock* parse_single_stmt_block(Parse* p) {
    return NULL;
}

AstBlock* parse_block(Parse* p) {
    if (lex_peek(p->l, 0).type != LBRACE)
        return parse_single_stmt_block(p);

    u32 loc = lex_eat(p->l).index;

    AstBlock* node = arena_alloc(p->arena, AstBlock);
    node->base = (ast_base){ AST_PROC, loc };

    while (true) {
        switch (lex_peek(p->l, 0).type) {
        case IDENT:
            arrput(node->statements, parse_stmt(p));
            break;
        case IF:
            arrput(node->statements, parse_if(p));
            break;
        default:
            require(IDENT, "Cannot start a statement with:");
        }

        if (lex_peek(p->l, 0).type == RBRACE) {
            lex_eat(p->l);
            break;
        }
    }
    
    return node;
}

AstIf* parse_if(Parse* p) {
    u32 loc = lex_eat(p->l).index;

    AstIf* node = arena_alloc(p->arena, AstIf);
    node->base = (ast_base){ AST_IF, loc };

    node->condition = parse_expr(p, 0);
    node->iftrue = parse_block(p);
    // if (lex_peek(p->l, 0).type == ELSE)
    node->iffalse = NULL;

    return node;
}

AstProc* parse_proc(Parse* p) {
    Token fntk = lex_eat(p->l);
    assert(fntk.type == FN);

    AstProc* node = arena_alloc(p->arena, AstProc);
    node->base = (ast_base){ AST_PROC, fntk.index };

    // Procedure name
    require(IDENT, "Procedure names must be a valid identifier.");
    Token ident = lex_eat(p->l);
    node->name = ident.str;

    // Parameters
    require_s(LPAREN, "Procedure parameters must be declared in parentheses ( )");
    Token param = lex_peek(p->l, 0);
    switch(param.type) {
    case RPAREN:
        // Procedure has no parameters
        node->parameters = NULL;
    case IDENT:
        // Procedure has some parameters
        node->parameters = parse_parameters(p);
    default:
        // Bad syntax
        require(RPAREN, "Parameters must be declared in the form (a: type, b: type, ...)");
    }

    require_s(RPAREN, "Parameter list must end with a closing parenthesis.");

    if (lex_peek(p->l, 0).type == ARROW) {
        // Explicit return type declaration
        lex_skip(p->l, 1);

        node->returnType = parse_type(p);
    }

    require(LBRACE, "A code block contained in curly braces { } must follow the function declaration");

    return node;
}

AstDecl* parse_decl(Parse* p) {
    Token tk = lex_eat(p->l);
    assert(tk.type == IDENT);

    AstDecl* node = arena_alloc(p->arena, AstDecl);
    node->base = (ast_base){ AST_DECL, tk.index };
    
    node->name = tk.str;

    require(CONST_ASGN, "Top-level declarations must use constant-syntax, i.e. x :: 10");
    lex_skip(p->l, 1);

    tk = lex_peek(p->l, 0);
    ast_base* def = NULL;
    switch (tk.type) {
    case DOT:
        def = &parse_struct_literal(p)->base; break;
    case INTEGER:
    case FLOAT:
        def = &parse_constant(p)->base; break;
    }

    return node;
}

ast_base* parse_one(Parse* p) {
    Token tk = lex_peek(p->l, 0);
    switch (tk.type) {
    case IDENT:
        return &parse_decl(p)->base;
    case FN:
        return &parse_proc(p)->base;
    case EOF_TOK:
        return NULL;
    default:
        errstart(tk.line, tk.column);
        printf("Expected a name to start a declaration\n");
        errloc(p->l->buffer.data, tk.index, tk.line);
        return NULL;
    }
}
