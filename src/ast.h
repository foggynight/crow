// ast.h - abstract syntax tree type

#ifndef AST_H
#define AST_H

#include <stdbool.h>

#include "token.h"
#include "vector.h"

typedef enum ast_type {
    AST_NULL, AST_ATOM, AST_LIST
} ast_type_t;

// Abstract syntax tree structure.
typedef struct ast {
    ast_type_t type; // Type of this AST, null, atom, or list.
    tok_t *tok;      // Token if this AST is an atom, otherwise NULL.
    vec_t *children; // Vector of ASTs if this AST is a list, otherwise NULL.
} ast_t;

ast_t *make_ast(void);
void dest_ast(ast_t *ast);

void ast_set_tok(ast_t *ast, tok_t *token);
void ast_add_child(ast_t *ast, ast_t *child);

size_t ast_child_count(ast_t *ast);
ast_t *ast_get_child(ast_t *ast, size_t i);

void print_ast(ast_t *ast);

#endif // AST_H
