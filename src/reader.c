#include "reader.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "global.h"
#include "limits.h"
#include "sexp.h"
#include "string.h"
#include "token.h"
#include "types.h"
#include "vector.h"

// input -----------------------------------------------------------------------

static const char *delim_drop = " \f\n\r\t\v"; // token delimiters to drop
static const char *delim_keep = "()'.";        // token delimiters to tokenize

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
    bool improper_list = false;
    for (char *word; (word = read_word(file, str, is_file)) != NULL;) {
        if (improper_list) {
            improper_list = false;
            vec_push(words, word);
            word = read_word(file, str, is_file);
            if (word[0] != ')') error("read_sexp: missing list terminator");
            else                --stk;
        } else if (word[0] == '.') {
            if (stk == 0) error("read_sexp: invalid use of: .");
            else          improper_list = true;

        }
        else if (word[0] == '(') { ++stk; }
        else if (word[0] == ')') {
            if (stk == 0) error("read_sexp: unexpected list terminator");
            else          --stk;
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
    case '(':  type = TOK_OPEN; break;
    case ')':  type = TOK_CLOSE; break;
    case '\'': type = TOK_QUOTE; break;
    case '.':  type = TOK_PERIOD; break;
    default:
        if (str_is_numeric(word)) type = TOK_NUM;
        else type = TOK_SYMBOL;
    }
    return make_tok(type, word);
}

// parser ----------------------------------------------------------------------

static vec_t *toks; // vector of tokens to parse
static size_t cnt;  // size of vector of tokens

static tok_t *tok; // current token being parsed
static size_t pos; // index of current token

static void parse_error(void) {
    error("parse error");
}

static void next(void) {
    tok = (pos < cnt) ? vec_get(toks, pos++) : NULL;
}

static void match(tok_type_t type) {
    if (tok->type == type) next();
    else parse_error();
}

static sexp_t *parse_atom(void) {
    if (!tok) parse_error();

    sexp_t *sexp;
    switch (tok->type) {
    case TOK_SYMBOL: sexp = make_sexp_symbol(tok); break;
    case TOK_NUM: sexp = make_sexp_num(tok2num(tok)); break;
    default: parse_error();
    }

    next();
    return sexp;
}

static sexp_t *parse_sexp(void);

static sexp_t *parse_quote(void) {
    if (!tok) parse_error();
    return sexp_cons(sexp_quote, sexp_cons(parse_sexp(), sexp_null));
}

static sexp_t *parse_rest(void) {
    if (!tok) parse_error();

    const tok_type_t t = tok->type;
    if (tok_type_is_atom(t) || t == TOK_OPEN || t == TOK_QUOTE) {
        sexp_t *sexp = parse_sexp();
        return sexp_cons(sexp, parse_rest());
    } else if (t == TOK_CLOSE) {
        return sexp_null;
    } else if (t == TOK_PERIOD) {
        match(TOK_PERIOD);
        return parse_sexp();
    } else {
        parse_error();
        return NULL;
    }
}

static sexp_t *parse_sexp(void) {
    if (!tok) parse_error();

    if (tok_type_is_atom(tok->type)) {
        return parse_atom();
    } else {
        switch (tok->type) {
        case TOK_QUOTE:
            match(TOK_QUOTE);
            return parse_quote();
        case TOK_OPEN:
            match(TOK_OPEN);
            sexp_t *sexp = parse_rest();
            match(TOK_CLOSE);
            return sexp;
        default:
            parse_error();
            return NULL;
        }
    }
}

static sexp_t *parse(vec_t *sexp_toks) {
    toks = sexp_toks;
    cnt = vec_size(toks);

    if (cnt == 0) return NULL;

    pos = 0;
    next();

    return parse_sexp();
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
