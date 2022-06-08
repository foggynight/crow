#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "reader.h"

#define GLOBAL_H_DEF
#include "global.h"

int main(void) {
    vec_t *words = make_vec(1);
    // read_sexp(stdin, words);
    // read_sexp_str("'foo", words);
    // read_sexp_str("'()", words);
    // read_sexp_str("'(test)", words);

    // TODO: Need to implement quotes being expanded into (quote ...).
    read_sexp_str("(test 'foo '() (foo '(bar 'baz)))", words);

    vec_t *toks = make_vec(1);
    for (size_t i = 0; i < vec_size(words); ++i)
        vec_push(toks, lex_word(vec_get(words, i)));

    ast_t *ast = parse_sexp(toks);
    print_ast(ast);
    putchar('\n');

    return 0;
}
