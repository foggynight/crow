// This is a recursive descent parser which implements doc/grammar.md.

#include "parser.h"

#include "ast.h"
#include "error.h"
#include "token.h"
#include "vector.h"

static vec_t *toks; // tokens to parse
static size_t cnt;  // number of tokens to parse

static tok_t *tok; // current token being parsed
static size_t pos; // index of current token in toks

static void parse_error(void) {
    error("parse error");
}

static void next(void) {
    tok = (pos < cnt) ? vec_get(toks, pos) : NULL;
    ++pos;
}

static void match(tok_type_t type) {
    if (tok->type == type) next();
    else parse_error();
}

static ast_t *atom(ast_t *ast) {
    if (!tok || !tok_is_atom(tok))
        parse_error();
    ast_set_tok(ast, tok);
    match(tok->type);
    return ast;
}

static ast_t *sexp(ast_t *ast);

static size_t rest(ast_t *ast, size_t cnt) {
    if (!tok) parse_error();

    const tok_type_t t = tok->type;
    if (tok_type_is_atom(t) || t == QUOTE || t == PAREN_OPEN) {
        ast_add_child(ast, sexp(NULL));
        return rest(ast, cnt + 1);
    } else if (t == PAREN_CLOSE) {
        return cnt;
    } else {
        parse_error();
        return 0;
    }
}

static ast_t *sexp(ast_t *ast) {
    if (!tok) parse_error();

    if (!ast) ast = make_ast();
    if (tok_is_atom(tok)) {
        ast->type = AST_ATOM;
        return atom(ast);
    } else if (tok->type == QUOTE) {
        match(QUOTE);
        ast->eval = AST_QUOTE;
        return sexp(ast);
    } else if (tok->type == PAREN_OPEN) {
        match(PAREN_OPEN);
        size_t cnt = rest(ast, 0);
        match(PAREN_CLOSE);
        ast->type = (cnt > 0) ? AST_LIST : AST_NULL;
        return ast;
    } else {
        parse_error();
        return NULL;
    }
}

ast_t *parse_sexp(vec_t *sexp_toks) {
    toks = sexp_toks;
    cnt = vec_size(toks);

    pos = 0;
    next();

    return sexp(NULL);
}
