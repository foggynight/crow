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
