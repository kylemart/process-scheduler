#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <types.h>

/**
 * The line number where loading a configuration file failed.
 */
extern ulong lineno;

/**
 * Loads configuration data from a configuration file into a configuration
 * object. A pointer to this object will be stored in the dest argument.
 *
 * @param  dest Where the pointer to the configuration object will be stored
 * @param  cf   The configuration file to be read from
 * @return      True if loading succeeded
 */
bool config_load(Config **dest, FILE *cf);

/**
 * Destroys all memory associated with the configuration object.
 *
 * @param config The configuration object to hulk smash.
 */
void config_destroy(Config *config);

/**
 * @param  config The configuration object to query.
 * @return        The (enum) type of scheduler to use.
 */
SchedulerType config_use(Config *config);

/**
 * @param  config The configuration object to query.
 * @return        The amount of time to run the scheduler for.
 */
uint config_runfor(Config *config);

/**
 * @param  config The configuration object to query.
 * @return        The quantum alloted to each job.
 */
uint config_quantum(Config *config);

/**
 * @param  config The configuration object to query.
 * @return        A list of processes to be run.
 */
ProcessList *config_processes(Config *config);

#endif
