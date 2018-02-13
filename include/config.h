#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <types.h>

/**
 * Each line read while loading a configuration file will increment this value.
 * This is especially useful for determining the line number in a configuration
 * file that is improperly formatted.
 */
extern ulong lineno;

/**
 * Reads a configuration file into a new configuration object and updates a
 * destination pointer to point to the new object. Should loading fail, the
 * function will return False. Furthermore, the externalized lineno variable
 * will contain the line within the configuration file the load operation
 * read up to.
 *
 * @param  dest Will be updated to point at the new configuration object
 * @param  cf   The configuration file to deserialize
 * @return      True if loading succeeded
 */
bool config_load(Config **dest, FILE *cf);

/**
 * Frees all memory associated with a configuration object.
 *
 * @param config A pointer to the configuration object to HULK SMASH
 */
void config_destroy(Config *config);

/**
 * @param  config A pointer to a configuration object
 * @return        The scheduling algorithm that is to be simulated
 */
SchedulerType config_use(Config *config);

/**
 * @param  config A pointer to a configuration object
 * @return        The amount of time to run the scheduling algorithm for
 */
uint config_runfor(Config *config);

/**
 * This is only relevant in the context of the Round-Robin scheduling
 * algorithm. This value should be disregarded in all other contexts.
 *
 * @param  config A pointer to a configuration object
 * @return        The maximum amount of time alloted to each running process
 */
uint config_quantum(Config *config);

/**
 * @param  config A pointer to a configuration object
 * @return        A pointer to a list of processes
 */
ProcessList *config_processes(Config *config);

#endif
