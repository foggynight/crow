#include <stdio.h>

#include "lexer.h"
#include "reader.h"

#define GLOBAL_H_DEF
#include "global.h"

int main(void) {
    // vec_t *toks = make_vec(1);
    // lex_str("(foo 2 bar 3 (baz 42))", toks);
    // for (size_t i = 0; i < vec_size(toks); ++i) {
    //     tok_t *tok = vec_get(toks, i);
    //     printf("%d\t%s\n", tok->type, tok->word);
    // }
    vec_t *words = read_sexp(stdin);
    for (size_t i = 0; i < vec_size(words); ++i)
        printf("WORD: %s\n", (char *)vec_get(words, i));
    return 0;
}
