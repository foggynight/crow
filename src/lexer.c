#include "lexer.h"

#include "string.h"
#include "token.h"

tok_t *lex_word(char *word) {
    tok_type_t type;
    switch (word[0]) {
    case '\'': type = QUOTE; break;
    case '(': type = PAREN_OPEN; break;
    case ')': type = PAREN_CLOSE; break;
    default: type = SYMBOL; // temporary
    }
    return make_tok(type, word);
}
