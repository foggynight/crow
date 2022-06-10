#include "ast.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "token.h"

ast_t *make_ast(void) {
    ast_t *ast = malloc(sizeof *ast);
    if (!ast) error("make_ast: malloc failed");

    ast->type = AST_NULL;
    ast->tok = NULL;
    ast->children = NULL;

    return ast;
}

void dest_ast(ast_t *ast) {
    dest_vec(ast->children);
    free(ast);
}

void ast_set_tok(ast_t *ast, tok_t *tok) {
    ast->tok = tok;
}

void ast_add_child(ast_t *ast, ast_t *child) {
    if (!ast->children)
        ast->children = make_vec(1);
    vec_push(ast->children, child);
}

size_t ast_child_count(ast_t *ast) {
    return ast->type == AST_LIST ? vec_size(ast->children) : 0;
}

ast_t *ast_get_child(ast_t *ast, size_t i) {
    if (!ast || i >= ast_child_count(ast))
        return NULL;
    return vec_get(ast->children, i);
}

void print_ast(ast_t *ast) {
    if (ast->type == AST_ATOM) {
        fputs(ast->tok->word, stdout);
    } else if (ast->type == AST_NULL) {
        fputs("()", stdout);
    } else { // this AST is a non-empty list
        putchar('(');
        const size_t cnt = ast_child_count(ast);
        for (size_t i = 0; i < cnt; ++i) {
            ast_t *child = ast_get_child(ast, i);
            print_ast(child);
            if (i + 1 < cnt) putchar(' ');
        }
        putchar(')');
    }
}
