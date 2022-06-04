// global.h - global variables

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>

extern FILE *input_port;
extern FILE *output_port;
extern FILE *error_port;

extern const char *delim_drop; // token delimiters to drop
extern const char *delim_keep; // token delimiters to tokenize
extern const char *delim_str;  // string token delimiters

#endif // GLOBAL_H

#ifdef GLOBAL_H_DEF
#undef GLOBAL_H_DEF

#include <unistd.h>

FILE *input_port  = (FILE *)STDIN_FILENO;
FILE *output_port = (FILE *)STDOUT_FILENO;
FILE *error_port  = (FILE *)STDERR_FILENO;

const char *delim_drop = " \f\n\r\t\v";
const char *delim_keep = "()";
const char *delim_str = "\"";

#endif // GLOBAL_H_DEF
