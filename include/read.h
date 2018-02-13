#ifndef READ_H
#define READ_H

#include <stdbool.h>
#include <stdio.h>
#include <types.h>

/**
 * Unless an end of file has been reached, or a read failure occurs, each call
 * to a read function will increment this value.
 */
extern ulong lineno;

/**
 * Reads a line and parses out a scheduler type.
 *
 * @param  result A pointer to where the scheduler type will be stored
 * @param  stream The stream to read from
 * @return        True if successful
 */
bool read_use(SchedulerType *result, FILE *stream);

/**
 * Reads a specified number of lines, and parses a process from each of them.
 *
 * @param  result A pointer to where the new process list will be stored
 * @param  n      The number of processes to read
 * @param  stream The stream to read from
 * @return        True if successful
 */
bool read_processes(ProcessList **result, size_t n, FILE *stream);

/**
 * Reads a line and parses out the runfor quantity.
 *
 * @param  result A pointer to where the runfor quantity will be stored
 * @param  stream The stream to read from
 * @return        True if successful
 */
bool read_runfor(uint *result, FILE *stream);

/**
 * Reads a line and parses out the number of processes.
 *
 * @param  result A pointer to where the number of processes will be stored
 * @param  stream The stream to read from
 * @return        True if successful
 */
bool read_processcount(size_t *result, FILE *stream);

/**
 * Reads a line and parses out the quantum.
 *
 * @param  result A pointer to where the quantum will be stored
 * @param  stream The stream to read from
 * @return        True if successful
 */
bool read_quantum(uint *result, FILE *stream);

/**
 * Reads a line and checks if it contains a proper file ending indicator.
 *
 * @param  stream The stream to read from
 * @return        True if successful
 */
bool read_end(FILE *stream);

#endif
