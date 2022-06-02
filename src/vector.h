// vector.h - vector type, resizable array

#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct vec {
    size_t size;
    size_t last;
    void **data;
} vec_t;

vec_t *make_vec(size_t size);
void dest_vec(vec_t *vec);

bool vec_is_empty(vec_t *vec);
bool vec_is_full(vec_t *vec);

void *vec_get(vec_t *vec, size_t pos);
void vec_set(vec_t *vec, size_t pos, void *elem);

void vec_resize(vec_t *vec, size_t size);

void vec_push(vec_t *vec, void *elem);
void *vec_pop(vec_t *vec);

void print_vec(vec_t *vec);

#endif // VECTOR_H
