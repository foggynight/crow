#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

typedef enum tok_type {
    TOK_SYMBOL, TOK_NUMBER, TOK_BOOL, TOK_CHARACTER, TOK_STRING,
    TOK_QUOTE, TOK_OPEN, TOK_CLOSE
} tok_type_t;

typedef struct tok {
    tok_type_t type;
    char *word;
} tok_t;

tok_t *make_tok(tok_type_t type, char *word);
void dest_tok(tok_t *tok);
bool tok_type_is_atom(tok_type_t type);
bool tok_is_atom(tok_t *tok);

#endif // TOKEN_H
