// token.h - token type

#ifndef TOKEN_H
#define TOKEN_H

typedef enum tok_type {
    SYMBOL,
    NUMBER,
    PAREN_OPEN,
    PAREN_CLOSE
} tok_type_t ;

typedef struct tok {
    tok_type_t type;
    char *word;
} tok_t;

tok_t *make_tok(tok_type_t type, char *word);
void dest_tok(tok_t *tok);

#endif // TOKEN_H
