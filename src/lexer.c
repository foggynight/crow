#include "lexer.h"

#include "global.h"
#include "string.h"
#include "token.h"
#include "vector.h"

tok_t *lex_word(char *word) {
    tok_type_t type;
    switch (word[0]) {
        case '(': type = PAREN_OPEN; break;
        case ')': type = PAREN_CLOSE; break;
        default:
            if (str_is_numeric(word)) type = NUMBER;
            else type = SYMBOL;
    }
    return make_tok(type, word);
}

void lex_str(char *str, vec_t *toks) {
    for (char *word = str_tok(str, tok_del_drop, tok_del_keep);
         word != NULL;
         word = str_tok(NULL, tok_del_drop, tok_del_keep))
    {
        tok_t *tok = lex_word(word);
        vec_push(toks, tok);
    }
}
