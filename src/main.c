#include <stdio.h>

#include "vector.h"

int main(void) {
    vec_t *vec = make_vec(4);
    print_vec(vec);
    vec_push(vec, 1);
    vec_push(vec, 2);
    vec_push(vec, 3);
    print_vec(vec);
    printf("%d\n", vec_pop(vec));
    print_vec(vec);
    vec_push(vec, 4);
    vec_push(vec, 5);
    vec_push(vec, 6);
    print_vec(vec);
    dest_vec(vec);
    return 0;
}
