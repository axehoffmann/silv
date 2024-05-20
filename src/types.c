#include "types.h"

#include "ast.h"

bool types_equivalent(AstType* a, AstType* b) {
    // Equal type
    if (a == b) return true;

    // Both null/void

    // Convertable primitive type
    
    return false;
}

AstType* resolve_block(AstBlock* node) {}
AstType* resolve_constant(AstConstant* node) {}
AstType* resolve_memory(AstMemory* node) {}
AstType* resolve_binop(AstBinOp* node) {}
AstType* resolve_unop(AstUnOp* node) {}
AstType* resolve_decl(AstDecl* node) {}
AstType* resolve_if(AstIf* node) {}
AstType* resolve_array(AstArray* node) {}
AstType* resolve_struct_literal(AstStructLiteral* node) {}
AstType* resolve_call(AstCall* node) {}
AstType* resolve_return(AstReturn* node) {}

AstType* resolve_proc(AstProc* node) {
    AstType* foundReturnType = resolve_block((AstBlock*) node->block);
}

AstType* resolve_types(ast* node) {
    switch (node->nodeType) {
    case AST_PROC:      return resolve_proc((AstProc*) node);
    case AST_BLOCK:     return resolve_block((AstBlock*) node);
    case AST_CONSTANT:  return resolve_constant((AstConstant*) node);
    case AST_MEMORY:    return resolve_memory((AstMemory*) node);
    case AST_BINOP:     return resolve_binop((AstBinOp*) node);
    case AST_UNOP:      return resolve_unop((AstUnOp*) node);
    case AST_DECL:      return resolve_decl((AstDecl*) node);
    case AST_IF:        return resolve_if((AstIf*) node);
    case AST_ARRAY:     return resolve_array((AstArray*) node);
    case AST_CALL:      return resolve_call((AstCall*) node);
    case AST_RETURN:    return resolve_return((AstReturn*) node);

            /*
    default:
        printf("what the sigma\n");
        exit(1);
        */
    }
}
