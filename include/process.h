#ifndef PROCESS_H
#define PROCESS_H

#include <stdlib.h>
#include <types.h>

/**
 * Creates a new process object and returns a pointer to it.
 *
 * @param  name    The name of the process.
 * @param  arrival The arrival time of the process.
 * @param  burst   The burst time of the process.
 * @return         A pointer to a new process.
 */
Process *process_new(char *name, uint arrival, uint burst);

/**
 * Destroys all memory associated with a process object. Will not complain
 * if the process pointer is NULL.
 *
 * @param process The process to drop kick into the void.
 */
void process_destroy(Process *process);

/**
 * @param  config The process object to query.
 * @return        The name of the process object.
 */
const char *process_name(Process *process);

/**
 * @param  process The process object to query.
 * @return         The burst time of the process.
 */
uint process_burst(Process *process);

/**
 * @param  process The process object to query.
 * @return         The arrival time of the process.
 */
uint process_arrival(Process *process);

/**
 * Creates a new process list object and returns a pointer to it.
 *
 * @param  capacity The maximum number of processes that can be stored.
 * @return          A pointer to a new process.
 */
ProcessList *processlist_new(size_t capacity);

/**
 * Destorys a process list object.
 *
 * @param list The process list to disintegrate with a space lazer.
 */
void processlist_destroy(ProcessList *list);

/**
 * Stores a pointer to a process in the list at the specified index.
 *
 * @param list    The process list to populate.
 * @param index   The index in the list to assign the process pointer to.
 * @param process The process pointer being stored in the list.
 */
void processlist_set(ProcessList *list, size_t index, Process *process);

/**
 * @param  list The process list object to query.
 * @return      The size of the process list.
 */
size_t processlist_size(ProcessList *list);

/**
 * Returns the process pointer stored at the specified index in the list.
 *
 * @param  list  The process list object to query.
 * @param  index The index of the process pointer to retrieve.
 * @return       A pointer to the retrieved process object.
 */
Process *processlist_get(ProcessList *list, size_t index);

#endif
