// global.h - global variables

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>

extern FILE *input_port;
extern FILE *output_port;
extern FILE *error_port;

#endif // GLOBAL_H

#ifdef GLOBAL_H_DEF
#undef GLOBAL_H_DEF

#include <unistd.h>

FILE *input_port  = (FILE *)STDIN_FILENO;
FILE *output_port = (FILE *)STDOUT_FILENO;
FILE *error_port  = (FILE *)STDERR_FILENO;

#endif // GLOBAL_H_DEF
