#include <stdio.h>

#include "lexer.h"

int main(void) {
    vec_t *toks = make_vec(1);
    lex_str("(foo 2 bar 3 (baz 42))", toks);
    for (size_t i = 0; i < toks->last; ++i) {
        tok_t *tok = vec_get(toks, i);
        printf("%d\t%s\n", tok->type, tok->word);
    }
    return 0;
}
