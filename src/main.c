#include <stdbool.h>
#include <stdio.h>

#include "reader.h"
#include "sexp.h"

#define GLOBAL_H_DEF
#include "global.h"

void display_banner(void) { fputs("CROW v0.0.0\n"
                                  "(C) 2022 Robert Coffey\n", stdout); }
void display_prompt(void) { fputs("> ", stdout); }

void crow_repl(bool prompt) {
recur:
    if (prompt) display_prompt();

    sexp_t *sexp = crow_read(stdin);
    if (!sexp) { putchar('\n'); return; }

    print_sexp(sexp);
    putchar('\n');

    goto recur;
}

int main(int argc, char **argv) {
    if (argc == 1) display_banner();
    crow_repl(true);
}
