#ifndef TYPES_H
#define TYPES_H

// num -------------------------------------------------------------------------

typedef enum num_type num_type_t;
typedef struct num num_t;
typedef long num_integer_t;
typedef double num_floating_t;

// sexp ------------------------------------------------------------------------

typedef enum sexp_type sexp_type_t;
typedef struct cons cons_t;
typedef struct sexp sexp_t;

// token -----------------------------------------------------------------------

typedef enum tok_type tok_type_t;
typedef struct tok tok_t;

#endif // TYPES_H
