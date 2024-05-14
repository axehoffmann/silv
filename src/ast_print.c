#include <stdio.h>
#include <inttypes.h>
#include "stb_ds.h"
#include "lex.h"
#include "ast.h"
#include "lib.h"

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

void print_expr(ast_base* node);

void print_binop(AstBinOp* node) {
    printf("(");
    print_expr(node->lhs);
    char* op;
    switch (node->opTk) {
    case ADD: op = "+"; break;
    case SUB: op = "-"; break;
    case MUL: op = "*"; break;
    case DIV: op = "/"; break;
    case MOD: op = "%%"; break;
    case OR_BIT: op = "|"; break;
    case AND_BIT: op = "&"; break;
    case NOT_BIT: op = "~"; break;
    case OR: op = "||"; break;
    case AND: op = "&&"; break;
    case NOT: op = "!"; break;
    case EQ: op = "=="; break;
    case NEQ: op = "!="; break;
    case LT: op = "<"; break;
    case LEQ: op = "<="; break;
    case GT: op = ">"; break;
    case GEQ: op = ">="; break;

    default: op = "@ERROR@";
    }
    printf(" %s ", op);
    print_expr(node->rhs);
    printf(")");
}

void print_decl(AstDecl* decl) {
    printf("%.*s: %i", decl->name.size, decl->name.data, decl->type->typeID);
    if (decl->rhs) {
        printf(" = ");
        print_expr(decl->rhs);
    }
}

void print_stmt(ast_base* node) {
    switch (node->nodeType) {
    case AST_DECL:
        print_decl((AstDecl*) node); break;
    }

    printf(";\n");
}

void print_block(AstBlock* node) {
    printf("{\n");
    Array_for(stmt, node->statements) {
        print_stmt(*stmt);
    }
    printf("}\n");
}

void print_proc(AstProc* node) {
    printf("fn %.*s (", node->name.size, node->name.data);
    u32 bob = hmlen(node->parameters);
    Array_for(param, node->parameters) {
        print_decl(*param);
        printf(", ");
    }
    printf(") -> %i ", node->returnType->typeID);
    print_block(node->block);
}

void print_expr(ast_base* node) {
    switch (node->nodeType) {
    case AST_CONSTANT: print_constant((AstConstant*) node); break;
    case AST_BINOP: print_binop((AstBinOp*) node); break;
    case AST_MEMORY: print_memory((AstMemory*) node); break;
    case AST_PROC: print_proc((AstProc*) node); break;
    }
}
