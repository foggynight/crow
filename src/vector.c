#include "vector.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

vec_t *make_vec(size_t size) {
    vec_t *vec = malloc(sizeof *vec);
    if (!vec) error("make_vec: malloc failed");

    void **data = malloc(size * sizeof(void *));
    if (!data) error("make_vec: malloc failed");

    vec->buf = data;
    vec->buf_size = size;
    vec->vec_size = 0;

    return vec;
}

void dest_vec(vec_t *vec) {
    assert(vec && vec->buf);
    free(vec->buf);
    free(vec);
}

bool vec_is_empty(vec_t *vec) { return vec_size(vec) == 0; }
bool vec_is_full(vec_t *vec) { return vec_size(vec) >= vec->buf_size; }

size_t vec_size(vec_t *vec) { return vec->vec_size; }

void vec_resize(vec_t *vec, size_t size) {
    assert(vec);

    void **data = realloc(vec->buf, size * sizeof(void *));
    if (!data) error("vec_resize: realloc failed");

    vec->buf = data;
    vec->buf_size = size;
}

void vec_grow(vec_t *vec) {
    if (vec->buf_size)
        vec_resize(vec, vec->buf_size * 2);
    else
        vec_resize(vec, 1);
}

void *vec_get(vec_t *vec, size_t pos) {
    assert(pos < vec->buf_size);
    return vec->buf[pos];
}

void vec_set(vec_t *vec, size_t pos, void *elem) {
    assert(pos < vec->buf_size);
    vec->buf[pos] = elem;
}

void vec_push(vec_t *vec, void *elem) {
    assert(vec);

    if (vec_is_full(vec))
        vec_grow(vec);

    vec_set(vec, vec->vec_size, elem);
    ++(vec->vec_size);
}

void *vec_pop(vec_t *vec) {
    assert(vec);

    if (vec_is_empty(vec))
        error("vec_pop: empty vector");

    --(vec->vec_size);
    return vec_get(vec, vec->vec_size);
}
