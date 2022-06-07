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

static void atom(void) {
    if (!tok || !tok_is_atom(tok)) parse_error();
    else match(tok->type);
}

static void sexp(void);

static void rest(void) {
    if (!tok) parse_error();

    const tok_type_t t = tok->type;
    if (tok_type_is_atom(t) || t == QUOTE || t == PAREN_OPEN) {
        sexp();
        rest();
    } else if (t == PAREN_CLOSE) {
        return;
    } else {
        parse_error();
    }
}

static void sexp(void) {
    if (!tok) parse_error();

    const tok_type_t type = tok->type;
    if (tok_is_atom(tok)) {
        atom();
    } else if (type == QUOTE) {
        match(QUOTE);
        sexp();
    } else if (type == PAREN_OPEN) {
        match(PAREN_OPEN);
        rest();
        match(PAREN_CLOSE);
    } else {
        parse_error();
    }
}

ast_t *parse_sexp(vec_t *sexp_toks) {
    toks = sexp_toks;
    cnt = vec_size(toks);

    pos = 0;
    next();

    sexp();

    return NULL;
}
