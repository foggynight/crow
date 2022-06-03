// test/string.c - string utility function tests

#include "../src/string.h"

#include <stdio.h>

char *del_drop = " ";
char *del_keep = "()";

void print_tokens(const char *str) {
    for (char *tok = str_tok(str, del_drop, del_keep);
         tok != NULL;
         tok = str_tok(NULL, del_drop, del_keep))
    {
        printf("\"%s\"\n", tok);
    }
}

int main(void) {
    print_tokens("");
    puts("---");
    print_tokens(" ");
    puts("---");
    print_tokens("()");
    puts("---");
    print_tokens("( foo )");
    puts("---");
    print_tokens("(foo)");
    puts("---");
    print_tokens("((foo))");
    puts("---");
    print_tokens("((foo bar))");
    puts("---");
    print_tokens("((foo bar)) () baz");
}
