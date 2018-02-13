#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>
#include <stdlib.h>
#include <types.h>

// #region Process -------------------------------------------------------------

typedef struct Process Process;

/**
 * @param  name    The name of the process
 * @param  arrival The arrival time of the process
 * @param  burst   The burst time of the process
 * @return         A pointer to a new process
 */
Process *process_new(char *name, uint arrival, uint burst);

/**
 * Frees all memory associated with a process object.
 *
 * @param process A pointer to a process object to DROP KICK
 */
void process_destroy(Process *process);

/**
 * @param  config A pointer to a process object
 * @return        The name of the process object
 */
const char *process_name(Process *process);

/**
 * @param  process A pointer to a process object
 * @return         The burst time of the process
 */
uint process_burst(Process *process);

/**
 * @param  process A pointer to a process object
 * @return         The arrival time of the process
 */
uint process_arrival(Process *process);

// #endregion ------------------------------------------------------------------

// #region ProcessList ---------------------------------------------------------

typedef struct ProcessList ProcessList;

/**
 * @param  capacity The maximum capacity of the process list
 * @return          A pointer to a new process list
 */
ProcessList *processlist_new(size_t capacity);

/**
 * Frees all memory associated with a process list object.
 *
 * @param list A pointer to the process list object to DISINTEGRATE
 */
void processlist_destroy(ProcessList *list);

/**
 * If the capacity of the list has not been reached, append a process to the
 * end of the list.
 *
 * @param list    A pointer to the process list to append to
 * @param process A pointer to the process being appended
 */
bool processlist_add(ProcessList *list, Process *process);

/**
 * @param  list A pointer to a process list
 * @return      The logical size of the list
 */
size_t processlist_size(ProcessList *list);

/**
 * @param  list  A pointer to a process list
 * @return       A pointer to the i-th process in the list
 */
Process *processlist_get(ProcessList *list, size_t index);

// #endregion ------------------------------------------------------------------

#endif
