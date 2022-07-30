#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

#include "types.h"

enum tok_type {
    TOK_SYMBOL, TOK_NUM, TOK_BOOL, TOK_CHARACTER, TOK_STRING,
    TOK_NULL, TOK_OPEN, TOK_CLOSE, TOK_QUOTE, TOK_PERIOD
};

struct tok {
    tok_type_t type;
    char *word;
};

tok_t *make_tok(tok_type_t type, char *word);
void dest_tok(tok_t *tok);

char *tok_word(tok_t *tok);

bool tok_type_is_atom(tok_type_t type);
bool tok_is_atom(tok_t *tok);

#endif // TOKEN_H
