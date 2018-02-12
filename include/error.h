#ifndef ERROR_H
#define ERROR_H

#define ERROR_MSG_MALLOC ("memory allocation failure")

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

#endif
