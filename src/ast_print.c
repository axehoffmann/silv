#include <stdio.h>
#include <inttypes.h>
#include "stb_ds.h"
#include "lex.h"
#include "ast.h"
#include "lib.h"

#include <assert.h>

void print_constant(AstConstant* node) {
    switch (node->valueType) {
    case VALUE_UINT:
        printf("%"PRIu64, node->uint);
        break;
    case VALUE_SINT:
        printf("%"PRIi64, node->sint);
        break;
    case VALUE_FLOAT:
        printf("%f", node->fpoint);
        break;
    case VALUE_BOOLEAN:
        if (node->boolean)
            printf("true");
        else
            printf("false");
        break;
    }
}

void print_memory(AstMemory* node) {
    printf("%.*s", node->name.size, node->name.data);
}

void print_expr(ast* node);

void print_call(AstCall* node) {
    print_expr(node->lhs);

    bool useBracks = (node->flags & CALL_ARRAY_INDEX);
    printf("%s", useBracks ? "[" : "(");
    Array_for(arg, node->args) {
        print_expr(*arg);
        printf(", ");
    }
    printf("%s", useBracks ? "]" : ")");
}

const char* tk_to_str(i32 tk) {
    const char* s;
    switch (tk) {
    case ADD: s = "+"; break;
    case SUB: s = "-"; break;
    case MUL: s = "*"; break;
    case DIV: s = "/"; break;
    case MOD: s = "%%"; break;
    case OR_BIT: s = "|"; break;
    case AND_BIT: s = "&"; break;
    case NOT_BIT: s = "~"; break;
    case OR: s = "||"; break;
    case AND: s = "&&"; break;
    case NOT: s = "!"; break;
    case EQ: s = "=="; break;
    case NEQ: s = "!="; break;
    case LT: s = "<"; break;
    case LEQ: s = "<="; break;
    case GT: s = ">"; break;
    case GEQ: s = ">="; break;
    case DOT: s = "."; break;

    case ASGN: s = "="; break;

    default: s = "@ERROR@";
    }
    return s;
}

void print_return(AstReturn* node) {
    printf("return ");
    print_expr(node->value);
}

void print_binop(AstBinOp* node) {
    printf("(");
    print_expr(node->lhs);
    printf(" %s ", tk_to_str(node->opTk));
    print_expr(node->rhs);
    printf(")");
}

void print_unop(AstUnOp* node) {
    printf("%s", tk_to_str(node->opTk));
    print_expr(node->rest);
}

void print_decl(AstDecl* decl) {
    printf("%.*s: %i", decl->name.size, decl->name.data, decl->type->typeID);
    if (decl->rhs) {
        printf(" = ");
        print_expr(decl->rhs);
    }
}

void print_block(AstBlock* node) {
    printf("{\n");
    if (node->statements)
        Array_for(stmt, node->statements) {
            print_expr(*stmt);
            printf(";\n");
        }
    printf("}\n");
}

void print_proc(AstProc* node) {
    printf("fn %.*s (", node->name.size, node->name.data);
    Array_for(param, node->parameters) {
        print_decl(*param);
        printf(", ");
    }
    printf(") ");
    if (node->returnType) printf("-> %i ", node->returnType->typeID);
    print_block(node->block);
}

void print_expr(ast* node) {
    switch (node->nodeType) {
    case AST_DECL: print_decl((AstDecl*) node); break;
    case AST_CONSTANT: print_constant((AstConstant*) node); break;
    case AST_BINOP: print_binop((AstBinOp*) node); break;
    case AST_UNOP: print_unop((AstUnOp*) node); break;
    case AST_MEMORY: print_memory((AstMemory*) node); break;
    case AST_PROC: print_proc((AstProc*) node); break;
    case AST_CALL: print_call((AstCall*) node); break;
    case AST_RETURN: print_return((AstReturn*) node); break;

    default: assert(false);
    }
    
}
