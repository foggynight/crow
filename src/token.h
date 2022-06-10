// token.h - token type

#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

#define TOK_QUOTE &(tok_t){ .type = QUOTE, .word = "quote" }

typedef enum tok_type {
    SYMBOL, BOOL, NUMBER, CHARACTER, STRING,
    QUOTE, PAREN_OPEN, PAREN_CLOSE
} tok_type_t;

typedef struct tok {
    tok_type_t type;
    char *word;
} tok_t;

tok_t *make_tok(tok_type_t type, char *word);
void dest_tok(tok_t *tok);

tok_type_t tok_type(tok_t *tok);
char *tok_word(tok_t *tok);

bool tok_is_atom(tok_t *tok);
bool tok_type_is_atom(tok_type_t type);

#endif // TOKEN_H
