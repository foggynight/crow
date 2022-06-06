#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "vector.h"

ast_t *parse_sexp(vec_t *sexp_toks);

#endif // PARSER_H
