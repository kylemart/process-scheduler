#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR_MSG_MEMORY ("memory allocation failure")

static void error_print(const char *format, va_list args)
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

void *amalloc(size_t size)
{
    void *ptr = malloc(size);
    if (!ptr) {
        error_abort(ERROR_MSG_MEMORY);
    }
    return ptr;
}

void *acalloc(size_t n, size_t size)
{
    void *ptr = calloc(n, size);
    if (!ptr) {
        error_abort(ERROR_MSG_MEMORY);
    }
    return ptr;
}
