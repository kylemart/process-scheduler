#include <string.h>
#include <limits.h>
#include <error.h>
#include <scheduler.h>

#define min(x, y) ((x) < (y)) ? (x) : (y)

// #region Job(s) --------------------------------------------------------------

typedef struct Job
{
    const char *name;
    uint start;
    uint burst;
    uint wait;
    uint finished;
} Job;

static int cmp_start(const void *arg1, const void *arg2)
{
    const Job *job1 = arg1;
    const Job *job2 = arg2;
    if (job1->start < job2->start) {
        return -1;
    }
    else if (job1->start > job2->start) {
        return 1;
    }
    return 0;
}

static Job *jobs_new(ProcessList *processes)
{
    size_t jobcount = processlist_size(processes);
    Job *jobs = amalloc(jobcount * sizeof(Job));
    for (size_t i = 0; i < jobcount; ++i) {
        Process *p = processlist_get(processes, i);
        jobs[i] = (Job) {
            .name = process_name(p),
            .start = process_arrival(p),
            .burst = process_burst(p),
            .wait = 0,
            .finished = 0
        };
    }
    qsort(jobs, jobcount, sizeof(Job), cmp_start);
    return jobs;
}

static void jobs_destroy(Job *jobs)
{
    free(jobs);
}

// #endregion ------------------------------------------------------------------

// #region Scheduling Algorithms -----------------------------------------------

static void print_wait_turnaround(FILE *out, Job *jobs, size_t jobcount)
{
    for (size_t i = 0; i < jobcount; ++i) {
        fprintf(out, "%s wait %u turnaround %u\n", jobs[i].name, jobs[i].wait,
            jobs[i].finished - jobs[i].start);
    }
}

void run_fcfs(FILE *out, uint runfor, ProcessList *processes)
{
    size_t jobcount = processlist_size(processes);
    Job *jobs = jobs_new(processes);
    size_t finished = 0, arrived = 0;
    ssize_t select = -1;

    fprintf(out, "%zu processes\n", jobcount);
    fputs("Using First Come First Served\n\n", out);

    for (uint tick = 0; tick <= runfor; ++tick) {
        for (size_t i = 0; i < jobcount; ++i) {
            if (jobs[i].start > tick || jobs[i].burst == 0) {
                continue;
            }
            else if (jobs[i].start == tick) {
                fprintf(out, "Time %u: %s arrived\n", tick, jobs[i].name);
                ++arrived;
            }
            else if (i != select) {
                ++jobs[i].wait;
            }
        }

        if (select >= 0 && jobs[select].burst == 0) {
            fprintf(out, "Time %u: %s finished\n", tick, jobs[select].name);
            jobs[select].finished = tick;
            select = -1;
            ++finished;
        }

        if (finished < arrived) {
            if (select == -1) {
                select = finished;
                fprintf(out, "Time %u: %s selected (burst %u)\n", tick,
                    jobs[select].name, jobs[select].burst);
            }
            --jobs[select].burst;
        }

        if (tick < runfor && finished == arrived) {
            fprintf(out, "Time %u: IDLE\n", tick);
        }
    }
    fprintf(out, "Finished at time %u\n\n", runfor);

    print_wait_turnaround(out, jobs, jobcount);

    jobs_destroy(jobs);
}

void run_sjf(FILE *out, uint runfor, ProcessList *processes)
{
    size_t jobcount = processlist_size(processes);
    Job *jobs = jobs_new(processes);
    size_t finished = 0, arrived = 0;
    ssize_t select = -1;

    fprintf(out, "%zu processes\n", jobcount);
    fputs("Using Shortest Job First (Pre)\n\n", out);

    for (uint tick = 0; tick <= runfor; ++tick) {
        ssize_t shortest = -1;

        for (size_t i = 0; i < jobcount; ++i) {
            if (jobs[i].start > tick || jobs[i].burst == 0) {
                continue;
            }
            else if (jobs[i].start == tick) {
                fprintf(out, "Time %u: %s arrived\n", tick, jobs[i].name);
                ++arrived;
            }
            else if (i != select) {
                ++jobs[i].wait;
            }
            if (shortest == -1 || jobs[i].burst < jobs[shortest].burst) {
                shortest = i;
            }
        }

        if (select >= 0 && jobs[select].burst == 0) {
            fprintf(out, "Time %u: %s finished\n", tick, jobs[select].name);
            jobs[select].finished = tick;
            select = -1;
            ++finished;
        }

        if (finished < arrived) {
            if (select == -1 || select != shortest) {
                select = shortest;
                fprintf(out, "Time %u: %s selected (burst %u)\n", tick,
                    jobs[select].name, jobs[select].burst);
            }
            --jobs[select].burst;
        }

        if (tick < runfor && finished == arrived) {
            fprintf(out, "Time %u: IDLE\n", tick);
        }
    }
    fprintf(out, "Finished at time %u\n\n", runfor);

    print_wait_turnaround(out, jobs, jobcount);

    jobs_destroy(jobs);
}

void run_rr(FILE *out, uint runfor, uint quantum, ProcessList *processes)
{
    size_t jobcount = processlist_size(processes);
    Job *jobs = jobs_new(processes);
    size_t finished = 0, arrived = 0;
    ssize_t select = -1;
    uint timer = 0;

    fprintf(out, "%zu processes\n", jobcount);
    fputs("Using Round-Robin\n", out);
    fprintf(out, "Quantum %u\n\n", quantum);

    for (uint tick = 0; tick <= runfor; ++tick) {
        for (size_t i = 0; i < jobcount; ++i) {
            if (jobs[i].start > tick || jobs[i].burst == 0) {
                continue;
            }
            else if (jobs[i].start == tick) {
                fprintf(out, "Time %u: %s arrived\n", tick, jobs[i].name);
                ++arrived;
            }
            else if (i != select) {
                ++jobs[i].wait;
            }
        }

        if (select >= 0 && jobs[select].burst == 0) {
            fprintf(out, "Time %u: %s finished\n", tick, jobs[select].name);
            jobs[select].finished = tick;
            select = -1;
            ++finished;
        }

        if (finished < arrived) {
            if (select == -1 || timer == 0) {
                for (size_t offset = 1; offset <= jobcount; ++offset) {
                    size_t i = (select + offset) % jobcount;
                    if (jobs[i].start <= tick && jobs[i].burst > 0) {
                        select = i;
                        break;
                    }
                }
                timer = min(jobs[select].burst, quantum);
                fprintf(out, "Time %u: %s selected (burst %u)\n", tick,
                    jobs[select].name, jobs[select].burst);
            }
            --jobs[select].burst;
            --timer;
        }

        if (tick < runfor && finished == arrived) {
            fprintf(out, "Time %u: IDLE\n", tick);
        }
    }
    fprintf(out, "Finished at time %u\n\n", runfor);

    print_wait_turnaround(out, jobs, jobcount);

    jobs_destroy(jobs);
}

// #endregion ------------------------------------------------------------------
