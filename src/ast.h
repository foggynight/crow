// ast.h - abstract syntax tree type

#ifndef AST_H
#define AST_H

#include <stdbool.h>

#include "token.h"
#include "vector.h"

#define AST_NULL 0
#define AST_ATOM 1
#define AST_LIST 2

#define AST_QUOTE 0
#define AST_EVAL  1

// Abstract syntax tree structure.
typedef struct ast {
    char type;       // Type of this AST, null, atom, or list.
    bool eval;       // Should this AST be eval'd, instead of quote'd?
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
