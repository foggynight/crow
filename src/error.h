// error.h - error macro to print message and exit

#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define error(FMT, ...)                                     \
    do {                                                    \
        fprintf(stderr, "crow: " FMT "\n", ##__VA_ARGS__);  \
        exit(1);                                            \
    } while (0)

#endif // ERROR_H
