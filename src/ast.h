// ast.h - abstract syntax tree type

#ifndef AST_H
#define AST_H

#include <stdbool.h>

#include "token.h"
#include "vector.h"

// Abstract syntax tree structure.
typedef struct ast {
    bool atom;       // Does this AST represent an atom, instead of a list?
    bool eval;       // Should this AST be evaluated, instead of quoted?
    tok_t *tok;      // Token if this AST is an atom, otherwise NULL.
    vec_t *children; // Vector of ASTs if this AST is a list, otherwise NULL.
} ast_t;

ast_t *make_ast(void);
void dest_ast(ast_t *ast);

void ast_set_tok(ast_t *ast, tok_t *token);
void ast_add_child(ast_t *ast, ast_t *child);

#endif // AST_H
