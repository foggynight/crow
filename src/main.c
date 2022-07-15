#include <stdbool.h>
#include <stdio.h>

#include "env.h"
#include "reader.h"
#include "sexp.h"

#define GLOBAL_H_DEF
#include "global.h"

static bool flag_display_prompt = true;

static void display_banner(void) { fputs("CROW v0.0.0\n"
                                         "(C) 2022 Robert Coffey\n", stdout); }
static void display_prompt(void) { fputs("> ", stdout); }

static void crow_repl(env_t *env) {
recur:
    if (flag_display_prompt) display_prompt();

    sexp_t *sexp = crow_read(stdin);
    if (!sexp) { putchar('\n'); return; }

    sexp_t *datum = NULL;
    if (sexp_is_atom(sexp))
        datum = env_lookup(env, sexp);

    if (datum) print_sexp(datum);
    else print_sexp(sexp);

    putchar('\n');

    goto recur;
}

int main(int argc, char **argv) {
    if (argc == 1) display_banner();

    env_t *toplevel = make_env();

    // TEMP
    sexp_t *sexp_a = make_sexp_atom(make_tok(TOK_SYMBOL, "a"));
    sexp_t *sexp_b = make_sexp_atom(make_tok(TOK_SYMBOL, "b"));
    sexp_t *pair = sexp_cons(sexp_a, sexp_b);

    env_insert(toplevel, pair);

    crow_repl(toplevel);
}
