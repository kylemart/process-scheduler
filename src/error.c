#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void error_print(const char *format, va_list args)
{
    fprintf(stderr, "error: ");
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
}

void error_exit(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    error_print(format, args);
    va_end(args);

    exit(EXIT_FAILURE);
}

void error_abort(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    error_print(format, args);
    va_end(args);

    abort();
}
