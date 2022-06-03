#include "lexer.h"

#include "string.h"
#include "token.h"
#include "vector.h"

tok_t *lex_word(char *word) {
    tok_type_t type;
    switch (word[0]) {
        case '(': type = PAREN_OPEN; break;
        case ')': type = PAREN_CLOSE; break;
        default: type = SYMBOL;
    }
    return make_tok(type, word);
}

void lex_str(char *str, vec_t *toks) {
    const char *del_drop = " \f\n\r\t\v"; // drop all whitespace
    const char *del_keep = "()";          // tokenize parentheses

    for (char *word = str_tok(str, del_drop, del_keep);
         word != NULL;
         word = str_tok(NULL, del_drop, del_keep))
    {
        tok_t *tok = lex_word(word);
        vec_push(toks, tok);
    }
}
