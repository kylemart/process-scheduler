#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <config.h>
#include <process.h>

/**
 * Runs a "first come, first served" scheduler simulation.
 *
 * @param out       The file to output simulation results to
 * @param runfor    The amount of time to run the simulation for
 * @param processes The processes to run the simulation with
 */
void run_fcfs(FILE *out, uint runfor, ProcessList *processes);

/**
 * Runs a "shortest job first" scheduler simulation.
 *
 * @param out       The file to output simulation results to
 * @param runfor    The amount of time to run the simulation for
 * @param processes The processes to run the simulation with
 */
void run_sjf(FILE *out, uint runfor, ProcessList *processes);

/**
 * Runs a "round-robin" scheduler simulation.
 *
 * @param out       The file to output simulation results to
 * @param runfor    The amount of time to run the simulation for
 * @param processes The processes to run the simulation with
 */
void run_rr(FILE *out, uint runfor, uint quantum, ProcessList *processes);

#endif
