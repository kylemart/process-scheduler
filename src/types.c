#include <string.h>
#include <types.h>

SchedulerType to_schedulertype(const char *str)
{
    if (strcmp(str, "fcfs") == 0) {
        return SCHEDULER_FCFS;
    }
    else if (strcmp(str, "rr") == 0) {
        return SCHEDULER_RR;
    }
    else if (strcmp(str, "sjf") == 0) {
        return SCHEDULER_SJF;
    }
    return SCHEDULER_UNDEF;
}
