#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "reader.h"

#define GLOBAL_H_DEF
#include "global.h"

int main(void) {
    vec_t *words = make_vec(1);
    read_sexp(stdin, words);

    vec_t *toks = make_vec(1);
    for (size_t i = 0; i < vec_size(words); ++i)
        vec_push(toks, lex_word(vec_get(words, i)));

    for (size_t i = 0; i < vec_size(toks); ++i) {
        tok_t *tok = vec_get(toks, i);
        printf("TOK: [%d] %s\n", tok->type, tok->word);
    }

    parse_sexp(toks);

    return 0;
}
