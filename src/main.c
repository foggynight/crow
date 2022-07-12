#include <stdio.h>

#include "reader.h"
#include "sexp.h"

#define GLOBAL_H_DEF
#include "global.h"

int main(void) {
    for (;;) {
        sexp_t *sexp = crow_read(stdin);
        if (!sexp) break;

        print_sexp(sexp);
        putchar('\n');
    }
}
