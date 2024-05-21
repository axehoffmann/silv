#pragma once

typedef struct ast ast;
typedef struct ast_type AstType;
typedef struct ast_scope AstScope;

void resolve_types(ast* node);
