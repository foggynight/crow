#include "reader.h"

#include <stdio.h>
#include <string.h>

#include "error.h"
#include "global.h"
#include "limit.h"
#include "vector.h"

#define TMP_NAME "/tmp/crow_sexp"

// TODO: Handle strings, that is, treat characters delimited by double quotes as
// a single word.
char *read_word(FILE *file) {
    static char word[WORD_MAX];

    size_t next = 0;
    for (int c; (c = getc(file)) != EOF;) {
        if (strchr(delim_drop, c)) {
            if (next == 0) continue; // skip leading whitespace
            else break;
        } else if (strchr(delim_keep, c)) {
            if (next == 0) word[next++] = c; // delimiter is word
            else ungetc(c, file);
            break;
        } else {
            word[next++] = c;
        }
    }
    word[next] = '\0';

    return next ? strdup(word) : NULL;
}

size_t read_sexp(FILE *file, vec_t *words) {
    size_t stk = 0;
    do {
        char *word = read_word(file);
        if (word == NULL) { break; }
        else if (word[0] == '(') { ++stk; }
        else if (word[0] == ')') {
            if (stk == 0)
                error("read_sexp: unexpected list terminator");
            --stk;
        }
        vec_push(words, word);
    } while (stk > 0);
    return vec_size(words);
}

// TODO: Do this right.
size_t read_sexp_str(char *str, vec_t *words) {
    FILE *tmp_file = fopen(TMP_NAME, "w+");
    if (!tmp_file) error("failed to open temp file: %s", TMP_NAME);

    fputs(str, tmp_file);
    fseek(tmp_file, 0, SEEK_SET);
    size_t k = read_sexp(tmp_file, words);

    fclose(tmp_file);
    return k;
}
