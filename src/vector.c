#include "vector.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

vec_t *make_vec(size_t size) {
    vec_t *vec = malloc(sizeof vec);
    if (!vec) error("make_vec: malloc failed");

    void **data = malloc(size * sizeof(void *));
    if (!data) error("make_vec: malloc failed");

    vec->size = size;
    vec->last = 0;
    vec->data = data;

    return vec;
}

void dest_vec(vec_t *vec) {
    assert(vec && vec->data);
    free(vec->data);
    free(vec);
}

bool vec_is_empty(vec_t *vec) { return vec->last == 0; }
bool vec_is_full(vec_t *vec) { return vec->last >= vec->size; }

void *vec_get(vec_t *vec, size_t pos) {
    assert(pos < vec->size);
    return vec->data[pos];
}

void vec_set(vec_t *vec, size_t pos, void *elem) {
    assert(pos < vec->size);
    vec->data[pos] = elem;
}

void vec_resize(vec_t *vec, size_t size) {
    assert(vec);

    void **data = realloc(vec->data, size * sizeof(void *));
    if (!data) error("vec_resize: realloc failed");

    vec->size = size;
    vec->data = data;
}

void vec_grow(vec_t *vec) {
    if (vec->size)
        vec_resize(vec, vec->size * 2);
    else
        vec_resize(vec, 1);
}

void vec_push(vec_t *vec, void *elem) {
    assert(vec);

    if (vec_is_full(vec))
        vec_grow(vec);

    vec_set(vec, vec->last, elem);
    ++(vec->last);
}

void *vec_pop(vec_t *vec) {
    assert(vec);

    if (vec_is_empty(vec))
        error("vec_pop: empty vector");

    --(vec->last);
    return vec_get(vec, vec->last);
}
