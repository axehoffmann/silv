#include <stdio.h>
#include <inttypes.h>
#include "lex.h"
#include "ast.h"

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
    print_expr(node->lhs);
    char* op;
    switch (node->opTk) {
        case ADD: op = "+"; break;
        case SUB: op = "-"; break;
        case MUL: op = "*"; break;
        case DIV: op = "/"; break;
        case MOD: op = "\%"; break;
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
}

void print_block(AstBlock* node) {
    printf("{\n");

}

void print_expr(ast_base* node) {

}
