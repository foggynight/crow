#include <stdbool.h>
#include <stdio.h>

#include "env.h"
#include "eval.h"
#include "reader.h"
#include "sexp.h"

#define GLOBAL_H_DEF
#include "global.h"

static struct {
    bool display_prompt;
} config = { true };

static void display_banner(void) { fputs("CROW v0.0.0\n"
                                         "(C) 2022 Robert Coffey\n", stdout); }
static void display_prompt(void) { fputs("> ", stdout); }

static void crow_repl(env_t *env) {
recur:
    if (config.display_prompt) display_prompt();

    sexp_t *sexp = crow_read(stdin);
    if (!sexp) { putchar('\n'); return; }

    print_sexp(crow_eval(sexp, env));
    putchar('\n');

    goto recur;
}

int main(int argc, char **argv) {
    if (argc == 1) display_banner();

    env_t *toplevel = make_env();

    sexp_t *sexp_a = make_sexp_symbol(make_tok(TOK_SYMBOL, "a"));
    sexp_t *sexp_b = make_sexp_symbol(make_tok(TOK_SYMBOL, "b"));
    env_insert(toplevel, sexp_cons(sexp_a, sexp_b));

    crow_repl(toplevel);
}
