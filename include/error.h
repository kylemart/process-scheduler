#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>

/**
 * Prints an error message to stderr and exits using the EXIT_FAILURE flag.
 * Preferably used where a normal, expected error has arisen (e.g. failed
 * to open a file).
 *
 * @param format  The formatted message to print.
 * @param VARARGS Arguments to inline in the formatted message.
 */
void error_exit(const char *format, ...);

/**
 * Prints an error message to stderr and aborts the program. Preferably used
 * in instances where an abnormal unexpected error has arisen (e.g. malloc
 * has failed) and there is no use trying to recover. A core dump is also
 * created.
 *
 * @param format  The formatted message to print.
 * @param VARARGS Arguments to inline in the formatted message.
 */
void error_abort(const char *format, ...);

/**
 * A wrapper for malloc that calls error_abort() should it fail.
 *
 * @param  size The number of bytes to allocate
 * @return      A pointer to the created memory
 */
void *amalloc(size_t size);

/**
 * A wrapper for calloc that calls error_abort() should it fail.
 *
 * @param  n    The number of items to allocate space for
 * @param  size The number of bytes each item consumes
 * @return      A pointer to the created memory
 */
void *acalloc(size_t n, size_t size);

#endif
