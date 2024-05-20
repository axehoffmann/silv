#pragma once

typedef struct ast ast;
typedef struct ast_type AstType;

AstType* resolve_types(ast* node);
