#include "reader.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "global.h"
#include "limit.h"
#include "sexp.h"
#include "token.h"
#include "vector.h"

// input -----------------------------------------------------------------------

// TODO: Handle strings, that is, treat characters delimited by double quotes as
// a single word.
static char *read_word(FILE *file, char **str, bool is_file) {
    static char word[WORD_MAX];

    size_t next = 0;
    for (int c;
         is_file
             ? (c = getc(file)) != EOF
             : (c = *(*str)++) != '\0';)
    {
        if (strchr(delim_drop, c)) {
            if (next == 0) continue; // skip leading whitespace
            else break;
        } else if (strchr(delim_keep, c)) {
            if (next == 0) word[next++] = c; // delimiter is word
            // delimiter is not word, put character back
            else if (is_file) ungetc(c, file);
            else --(*str);
            break;
        } else {
            word[next++] = c;
        }
    }
    word[next] = '\0';

    return next ? strdup(word) : NULL;
}

static size_t read_sexp(FILE *file, char **str, bool is_file, vec_t *words) {
    size_t stk = 0;
    for (char *word; (word = read_word(file, str, is_file)) != NULL;) {
        if (word[0] == '(') { ++stk; }
        else if (word[0] == ')') {
            if (stk == 0)
                error("unexpected list terminator");
            --stk;
        }
        vec_push(words, word);
        if (stk == 0 && word[0] != '\'')
            break;
    }
    return vec_size(words);
}

static size_t read_sexp_file(FILE *file, vec_t *words) {
    return read_sexp(file, NULL, true, words);
}

static size_t read_sexp_str(const char *str, vec_t *words) {
    char *copy, *start;
    copy = start = strdup(str);
    if (!copy) error("read_sexp_str: strdup failed");
    const size_t ret = read_sexp(NULL, &copy, false, words);
    free(start);
    return ret;
}

// lexer -----------------------------------------------------------------------

static tok_t *lex_word(char *word) {
    tok_type_t type;
    switch (word[0]) {
    case '\'': type = TOK_QUOTE; break;
    case '(':  type = TOK_OPEN; break;
    case ')':  type = TOK_CLOSE; break;
    default:   type = TOK_SYMBOL; // temporary
    }
    return make_tok(type, word);
}

// parser ----------------------------------------------------------------------

static tok_t _tok_quote = { .type = TOK_QUOTE, .word = "quote" };
static sexp_t *sexp_quote =
    &(sexp_t){ .atom = &_tok_quote, .list = NULL };

static vec_t *toks; // vector of tokens to parse
static size_t cnt;  // size of vector of tokens

static tok_t *tok; // current token being parsed
static size_t pos; // index of current token

static void parse_error(void) {
    error("parse error");
}

static void next_token(void) {
    tok = (pos < cnt) ? vec_get(toks, pos++) : NULL;
}

static void match_type(tok_type_t type) {
    if (tok->type == type) next_token();
    else parse_error();
}

static sexp_t *parse_atom(sexp_t *sexp) {
    if (!tok) parse_error();

    if (!sexp) sexp = make_sexp(tok, NULL);
    else sexp->atom = tok;

    next_token();
    return sexp;
}

static sexp_t *parse_sexp(sexp_t *sexp);

static sexp_t *parse_quote(sexp_t *sexp) {
    if (!tok) parse_error();

    if (!sexp) sexp = make_sexp(NULL, NULL);
    sexp_cons(parse_sexp(NULL), sexp);
    sexp_cons(sexp_quote, sexp);

    return sexp;
}

static sexp_t *parse_rest(sexp_t *sexp) {
    if (!tok) parse_error();

    const tok_type_t t = tok->type;
    if (tok_type_is_atom(t) || t == TOK_QUOTE || t == TOK_OPEN) {
        sexp_cons(parse_sexp(NULL), sexp);
        return parse_rest(sexp);
    } else if (t == TOK_CLOSE) {
        return sexp;
    } else {
        parse_error();
        return sexp;
    }
}

static sexp_t *parse_sexp(sexp_t *sexp) {
    if (!tok) parse_error();
    if (!sexp) sexp = make_sexp(NULL, NULL);

    if (tok_type_is_atom(tok->type)) {
        return parse_atom(sexp);
    } else {
        switch (tok->type) {
        case TOK_QUOTE:
            match_type(TOK_QUOTE);
            return parse_quote(sexp);
        case TOK_OPEN:
            match_type(TOK_OPEN);
            parse_rest(sexp);
            match_type(TOK_CLOSE);
            return sexp_reverse(sexp);
        default:
            parse_error();
            return sexp;
        }
    }
}

static sexp_t *parse(vec_t *sexp_toks) {
    toks = sexp_toks;
    cnt = vec_size(toks);

    if (cnt == 0) {
        return NULL;
    }

    pos = 0;
    next_token();

    return parse_sexp(NULL);
}

// reader ----------------------------------------------------------------------

sexp_t *crow_read(FILE *file) {
    vec_t *words = make_vec(1);
    read_sexp_file(file, words);

    vec_t *toks = make_vec(1);
    for (size_t i = 0; i < vec_size(words); ++i)
        vec_push(toks, lex_word(vec_get(words, i)));

    return parse(toks);
}
