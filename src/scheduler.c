#include <string.h>
#include <limits.h>
#include <error.h>
#include <scheduler.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))

// #region Job(s) --------------------------------------------------------------

typedef struct Job
{
    const char *name;
    uint start;
    uint burst;
    uint wait;
    uint turnaround;
} Job;

static Job *jobs_new(ProcessList *processes)
{
    size_t n = processlist_size(processes);
    Job *jobs = amalloc(n * sizeof(Job));
    for (size_t i = 0; i < n; ++i) {
        Process *p = processlist_get(processes, i);
        jobs[i] = (Job) {
            .name = process_name(p),
            .start = process_arrival(p),
            .burst = process_burst(p),
            .wait = 0,
            .turnaround = 0
        };
    }
    return jobs;
}

static void jobs_destroy(Job *jobs)
{
    free(jobs);
}

// #endregion ------------------------------------------------------------------

// #region Scheduling Algorithms -----------------------------------------------

void run_fcfs(FILE *out, uint runfor, ProcessList *processes)
{
    size_t jobcount = processlist_size(processes);
    Job *jobs = jobs_new(processes);
    size_t started = 0;
    size_t finished = 0;
    Job *selected = NULL;
    ssize_t selectedindex = -1;

    fprintf(out, "%zu processes\n", jobcount);
    fputs("Using First Come First Served\n\n", out);

    for (uint tick = 0; tick <= runfor; ++tick) {
        for (size_t i = 0; i < jobcount; ++i) {
            Job *job = &jobs[i];
            if (job->start < tick && job->burst > 0) {
                if (job != selected) {
                    ++job->wait;
                }
                ++job->turnaround;
            }
            else if (job->start == tick) {
                fprintf(out, "Time %u: %s arrived\n", tick, job->name);
                ++started;
            }
        }

        if (selected) {
            --selected->burst;
            if (selected->burst == 0) {
                fprintf(out, "Time %u: %s finished\n", tick, selected->name);
                ++finished;
            }
            else continue;
        }

        if (tick == runfor) {
            break;
        }
        else if (started == finished) {
            selected = NULL;
            selectedindex = -1;
            fprintf(out, "Time %u: IDLE\n", tick);
        }
        else {
            for (size_t offset = 1; offset <= jobcount; ++offset) {
                size_t index = (selectedindex + offset) % jobcount;
                Job *job = &jobs[index];
                if (job->burst > 0 && job->start <= tick) {
                    selected = job;
                    selectedindex = index;
                    break;
                }
            }
            fprintf(out, "Time %u: %s selected (burst %u)\n", tick,
                selected->name, selected->burst);
        }
    }
    fprintf(out, "Finished at time %u\n\n", runfor);

    for (size_t i = 0; i < jobcount; ++i) {
        Job *job = &jobs[i];
        fprintf(out, "%s wait %u turnaround %u\n", job->name, job->wait,
                job->turnaround);
    }

    jobs_destroy(jobs);
}

void run_sjf(FILE *out, uint runfor, ProcessList *processes)
{
    size_t jobcount = processlist_size(processes);
    Job *jobs = jobs_new(processes);
    ssize_t previous = -1;
    ssize_t selectedindex = -1;

    fprintf (out, "%zu processes\n", jobcount);
    fputs ("Using Shortest Job First (Pre)\n\n", out);

    for (uint tick = 0; tick < runfor; ++tick) {
        uint min = UINT_MAX;

        for (size_t i = 0; i < jobcount; ++i) {
            Job *job = &jobs[i];
            if (job->start > tick || job->burst == 0) {
                continue;
            }
            if (min > job->burst) {
                min = job->burst;
                selectedindex = i;
            }
            if (job->start == tick) {
                fprintf(out, "Time %u: %s arrived\n", tick, job->name);
            }
        }

        for (size_t i = 0; i < jobcount; ++i) {
            Job *job = &jobs[i];
            if (job->start > tick || job->burst == 0) {
                continue;
            }
            ++job->turnaround;
            if (selectedindex != i) {
                ++job->wait;
            }
        }

        if (selectedindex >= 0) {
            Job *selected = &jobs[selectedindex];
            if (selectedindex != previous){
                fprintf(out, "Time %u: %s selected (burst %u)\n", tick,
                    selected->name, selected->burst);
            }
            --selected->burst;
            if (selected->burst == 0) {
                fprintf(out, "Time %u: %s finished\n", tick + 1,
                    selected->name);
            }
            previous = selectedindex;
            selectedindex = -1;
        }
        else {
            fprintf (out, "Time %u: IDLE\n", tick);
            previous = selectedindex;
        }
    }
    fprintf (out,"Finished at time %u\n\n", runfor);

    for (size_t i = 0; i < jobcount; ++i) {
        Job *job = &jobs[i];
        fprintf(out, "%s wait %u turnaround %u\n", job->name, job->wait,
            job->turnaround);
    }
}

void run_rr(FILE *out, uint runfor, uint quantum, ProcessList *processes)
{
    size_t jobcount = processlist_size(processes);
    Job *jobs = jobs_new(processes);
    size_t started = 0;
    size_t finished = 0;
    Job *selected = NULL;
    ssize_t selectedindex = -1;
    uint timeleft = 0;

    fprintf(out, "%zu processes\n", jobcount);
    fputs("Using Round-Robin\n", out);
    fprintf(out, "Quantum %u\n\n", quantum);

    for (uint tick = 0; tick <= runfor; ++tick) {
        for (size_t i = 0; i < jobcount; ++i) {
            Job *job = &jobs[i];
            if (job->start < tick && job->burst > 0) {
                if (job != selected) {
                    ++job->wait;
                }
                ++job->turnaround;
            }
            else if (job->start == tick) {
                fprintf(out, "Time %u: %s arrived\n", tick, job->name);
                ++started;
            }
        }

        if (selected) {
            --timeleft;
            --selected->burst;
            if (selected->burst == 0) {
                fprintf(out, "Time %u: %s finished\n", tick, selected->name);
                ++finished;
            }
            else if (timeleft > 0) {
                continue;
            }
        }

        if (tick == runfor) {
            break;
        }
        else if (started == finished) {
            selected = NULL;
            selectedindex = -1;
            fprintf(out, "Time %u: IDLE\n", tick);
        }
        else {
            for (size_t offset = 1; offset <= jobcount; ++offset) {
                size_t index = (selectedindex + offset) % jobcount;
                Job *job = &jobs[index];
                if (job->burst > 0 && job->start <= tick) {
                    selected = job;
                    selectedindex = index;
                    break;
                }
            }
            timeleft = min(selected->burst, quantum);
            fprintf(out, "Time %u: %s selected (burst %u)\n", tick,
                selected->name, selected->burst);
        }
    }
    fprintf(out, "Finished at time %u\n\n", runfor);

    for (size_t i = 0; i < jobcount; ++i) {
        Job *job = &jobs[i];
        fprintf(out, "%s wait %u turnaround %u\n", job->name, job->wait,
                job->turnaround);
    }

    jobs_destroy(jobs);
}

// #endregion ------------------------------------------------------------------
