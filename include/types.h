#ifndef TYPES_H
#define TYPES_H

typedef unsigned int uint;

typedef unsigned long ulong;

typedef struct Config Config;

typedef struct Process Process;

typedef struct ProcessList ProcessList;

typedef enum SchedulerType {
    SCHEDULER_FCFS, // first in first out
    SCHEDULER_SJF,  // shortest job first
    SCHEDULER_RR,   // round robin
    SCHEDULER_UNDEF // undefined
} SchedulerType;

SchedulerType to_schedulertype(const char *str);

#endif
