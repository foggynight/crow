#include "ast.h"

#include <stdbool.h>
#include <stdlib.h>

#include "error.h"
#include "token.h"

ast_t *make_ast(void) {
    ast_t *ast = malloc(sizeof *ast);
    if (!ast) error("make_ast: malloc failed");

    ast->atom = true;
    ast->eval = true;

    ast->root = NULL;
    ast->children = NULL;

    return ast;
}

void dest_ast(ast_t *ast) {
    dest_vec(ast->children);
    free(ast);
}

// These two functions should never both be called on the same AST.

void ast_set_tok(ast_t *ast, tok_t *token) {
    ast->root = root;
}

void ast_add_child(ast_t *ast, ast_t *child) {
    if (ast->atom) {
        ast->atom = false;
        ast->children = make_vec(1);
    }
    vec_push(ast->children, child);
}
