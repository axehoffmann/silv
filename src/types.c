#include "types.h"

#include "lex.h"
#include "ast.h"

#include "stb_ds.h"

bool converts_to(AstType* a, AstType* b) {
    return false;
}

bool types_equivalent(AstType* a, AstType* b) {
    // Equal type
    if (a == b) return true;

    // Both null/void

    // Convertable primitive type
    
    return false;
}

AstType* resolve_expr(ast* node, AstScope* scope);


// These functions validate the return type of the blocks they contain, as well as any contained expressions
void resolve_block(AstBlock* node, AstType* expected);
void resolve_if(AstIf* node, AstType* expected, AstScope* scope) {
    // The condition expression must be convertible to bool
    AstType* exprType = resolve_expr(node->condition, scope);
    // @TODO primitive typesystem
    if (!converts_to(exprType, exprType)) {
        printf("If statement condition must be convertible to a boolean\n");
        exit(1);
    }
    resolve_block(node->iftrue, expected);
    // @TODO else blocks
}

void resolve_decl(AstDecl* node, AstScope* scope) {
    if (node->rhs == NULL) return;

    AstType* declaredType = node->type;
    AstType* exprType = resolve_expr(node->rhs, scope);

    if (!types_equivalent(declaredType, exprType)) {
        printf("Declared type not equal to expression type\n");
        exit(1);
    }
}

void resolve_proc(AstProc* node) {
    resolve_block((AstBlock*) node->block, node->returnType);
}

void resolve_block(AstBlock* node, AstType* expected) {
    Array_for(stmt, node->statements) {
        switch ((*stmt)->nodeType) {

        case AST_BLOCK:
            resolve_block((AstBlock*) *stmt, expected);
            break;
        case AST_IF:
            resolve_if((AstIf*) *stmt, expected, node->scope);
            break;
        case AST_PROC:
            resolve_proc((AstProc*) *stmt);
            break;
        case AST_DECL:
            resolve_decl((AstDecl*) *stmt, node->scope);
            break;

        // Assignment + call expressions at top level
        case AST_CALL:
            resolve_expr(*stmt, node->scope);
        case AST_BINOP:
            if (((AstBinOp*) *stmt)->opTk != ASGN) {
                printf("Top level expressions must be assignments\n");
                exit(1);
            }
            resolve_expr(*stmt, node->scope);

        default:
            printf("Cannot be used as a top level statement!");
            exit(1);
        }
    }
}


// These functions return the type of the expression
AstType* resolve_constant(AstConstant* node) {}
AstType* resolve_memory(AstMemory* node, AstScope* scope) {}
AstType* resolve_binop(AstBinOp* node, AstScope* scope) {}
AstType* resolve_unop(AstUnOp* node, AstScope* scope) {}
AstType* resolve_call(AstCall* node, AstScope* scope) {
    if (node->flags & CALL_ARRAY_INDEX) {
        // @TODO arrays
    }

    // LHS must refer to a proc
    // @TODO function pointers and stuff here?
    if (node->lhs->nodeType != AST_MEMORY) {
        printf("LHS of a call expression must be an identifier");
        exit(1);
    }

    Symbol* symbol = find_symbol(scope, ((AstMemory*) node)->name);
    if (symbol == NULL || symbol->type != AST_PROC) {
        printf("LHS of a call expression must refer to a proc");
        exit(1);
    }

    node->resolvedProc = symbol->proc;
    
    // Parameters must match
    i32 argCount = arrlen(node->args);
    AstDecl** params = node->resolvedProc->parameters;
    if (argCount != arrlen(params)) {
        printf("Call must have correct number of arguments");
        exit(1);
    }

    for (i32 i = 0; i < argCount; i++) {
        AstType* argType = resolve_expr(node->args[i], scope);
        AstType* paramType = params[i]->type;
        if (!types_equivalent(argType, paramType)) {
            printf("Parameter %i types dont match\n", i);
            exit(1);
        }
    }

    return node->resolvedProc->returnType;
}

AstType* resolve_return(AstReturn* node, AstScope* scope) {
    return resolve_expr(node->value, scope);
}

AstType* resolve_expr(ast* node, AstScope* scope) {
    switch (node->nodeType) {
    case AST_MEMORY:    return resolve_memory((AstMemory*) node, scope);
    case AST_BINOP:     return resolve_binop((AstBinOp*) node, scope);
    case AST_UNOP:      return resolve_unop((AstUnOp*) node, scope);
    case AST_CALL:      return resolve_call((AstCall*) node, scope);
    case AST_RETURN:    return resolve_return((AstReturn*) node, scope);
    }
    printf("what the sigma\n");
    exit(1);
}

// Typecheck a top-level construct
void resolve_types(ast* node) {
    switch (node->nodeType) {
    // @TODO: file scope for decls
    case AST_DECL:      resolve_decl((AstDecl*) node, NULL);    return;
    case AST_PROC:      resolve_proc((AstProc*) node);          return;
    }
    printf("what the sigma\n");
    exit(1);
}
