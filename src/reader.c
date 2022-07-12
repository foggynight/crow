#include "reader.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "global.h"
#include "limit.h"
#include "vector.h"

// input -----------------------------------------------------------------------

#define TMP_NAME "/tmp/crow_sexp"

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

// token -----------------------------------------------------------------------

typedef enum tok_type {
    TOK_SYMBOL, TOK_NUMBER, TOK_BOOL, TOK_CHARACTER, TOK_STRING,
    TOK_QUOTE, TOK_OPEN, TOK_CLOSE
} tok_type_t;

typedef struct tok {
    tok_type_t type;
    char *word;
} tok_t;

static tok_t *make_tok(tok_type_t type, char *word) {
    tok_t *tok = malloc(sizeof *tok);
    if (!tok) error("make_tok: malloc failed");
    tok->type = type;
    tok->word = word;
    return tok;
}

static void dest_tok(tok_t *tok) {
    free(tok->word);
    free(tok);
}

static bool tok_type_is_atom(tok_type_t type) {
    return type == TOK_SYMBOL
        || type == TOK_NUMBER
        || type == TOK_BOOL
        || type == TOK_CHARACTER
        || type == TOK_STRING;
}

static bool tok_is_atom(tok_t *tok) {
    return tok ? tok_type_is_atom(tok->type) : false;
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

// ast -------------------------------------------------------------------------

typedef enum ast_type {
    AST_NULL, AST_ATOM, AST_LIST
} ast_type_t;

// Abstract syntax tree structure.
typedef struct ast {
    ast_type_t type; // Type of this AST, null, atom, or list.
    tok_t *tok;      // Token if this AST is an atom, otherwise NULL.
    vec_t *children; // Vector of ASTs if this AST is a list, otherwise NULL.
} ast_t;

static tok_t _tok_quote = { .type = TOK_QUOTE, .word = "quote" };
static ast_t *ast_quote =
    &(ast_t){ .type = AST_ATOM, .tok = &_tok_quote, .children = NULL };

static ast_t *make_ast(void) {
    ast_t *ast = malloc(sizeof *ast);
    if (!ast) error("make_ast: malloc failed");
    ast->type = AST_NULL;
    ast->tok = NULL;
    ast->children = NULL;
    return ast;
}

static void dest_ast(ast_t *ast) {
    dest_vec(ast->children);
    free(ast);
}

static void ast_set_tok(ast_t *ast, tok_t *tok) {
    ast->tok = tok;
}

static void ast_add_child(ast_t *ast, ast_t *child) {
    if (!ast->children)
        ast->children = make_vec(1);
    vec_push(ast->children, (void *)child);
}

static size_t ast_child_count(ast_t *ast) {
    return ast->type == AST_LIST ? vec_size(ast->children) : 0;
}

static ast_t *ast_get_child(ast_t *ast, size_t i) {
    if (!ast || i >= ast_child_count(ast))
        return NULL;
    return vec_get(ast->children, i);
}

static void print_ast(ast_t *ast) {
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

// parser ----------------------------------------------------------------------

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

static ast_t *sexp(ast_t *ast);

static ast_t *atom(ast_t *ast) {
    if (!tok || !tok_is_atom(tok))
        parse_error();

    ast->type = AST_ATOM;
    ast_set_tok(ast, tok);

    match(tok->type);
    return ast;
}

static ast_t *quote(ast_t *ast) {
    if (!tok || tok->type != TOK_QUOTE)
        parse_error();
    match(TOK_QUOTE);

    if (!ast) ast = make_ast();
    ast->type = AST_LIST;
    ast_add_child(ast, ast_quote);
    ast_add_child(ast, sexp(NULL));

    return ast;
}

static size_t rest(ast_t *ast, size_t cnt) {
    if (!tok) parse_error();

    const tok_type_t t = tok->type;
    if (tok_type_is_atom(t) || t == TOK_QUOTE || t == TOK_OPEN) {
        ast_add_child(ast, sexp(NULL));
        return rest(ast, cnt + 1);
    } else if (t == TOK_CLOSE) {
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
        return atom(ast);
    } else if (tok->type == TOK_QUOTE) {
        return quote(ast);
    } else if (tok->type == TOK_OPEN) {
        match(TOK_OPEN);
        size_t cnt = rest(ast, 0);
        match(TOK_CLOSE);
        ast->type = (cnt > 0) ? AST_LIST : AST_NULL;
        return ast;
    } else {
        parse_error();
        return NULL;
    }
}

static ast_t *parse_sexp(vec_t *sexp_toks) {
    toks = sexp_toks;
    cnt = vec_size(toks);

    pos = 0;
    next();

    return sexp(NULL);
}

// reader ----------------------------------------------------------------------

// TODO: Add sexp_t and return a (sexp_t *) from this function.
void crow_read(FILE *file) {
    vec_t *words = make_vec(1);
    //read_sexp(stdin, words);
    read_sexp_str("(test 'foo '() (foo '(bar 'baz)))", words);

    vec_t *toks = make_vec(1);
    for (size_t i = 0; i < vec_size(words); ++i)
        vec_push(toks, lex_word(vec_get(words, i)));

    ast_t *ast = parse_sexp(toks);
    print_ast(ast);
    putchar('\n');
}
