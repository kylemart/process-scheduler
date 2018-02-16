#include <string.h>
#include <limits.h>
#include <error.h>
#include <scheduler.h>

#define min(x, y) ((x < y) ? x : y)

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

// #region JobQueue ------------------------------------------------------------

typedef struct JobQueue
{
    size_t capacity;
    size_t size;
    size_t start;
    Job **jobs;
} JobQueue;

static JobQueue *jobq_new(size_t capacity)
{
    JobQueue *q = amalloc(sizeof(JobQueue));
    q->capacity = capacity;
    q->size = 0;
    q->start = 0;
    q->jobs = calloc(capacity, sizeof(Job *));
    return q;
}

static void jobq_destroy(JobQueue *q)
{
    free(q);
}

static bool jobq_empty(JobQueue *q)
{
    return q->size == 0;
}

static Job *jobq_peek(JobQueue *q)
{
    if (jobq_empty(q)) {
        return NULL;
    }
    return q->jobs[q->start];
}

static Job *jobq_remove(JobQueue *q)
{
    Job *removed = q->jobs[q->start];
    q->jobs[q->start] = NULL;
    q->start = (q->start + 1) % q->capacity;
    --q->size;
    return removed;
}

static bool jobq_add(JobQueue *q, Job *job)
{
    if (q->size == q->capacity) {
        return false;
    }
    size_t index = (q->start + q->size) % q->capacity;
    q->jobs[index] = job;
    ++q->size;
    return true;
}

static void jobq_lshift(JobQueue *q)
{
    Job *removed = jobq_remove(q);
    jobq_add(q, removed);
}

// #endregion ------------------------------------------------------------------

// #region Scheduling Algorithms -----------------------------------------------

void run_fcfs(FILE *out, uint runfor, ProcessList *processes)
{
    size_t jobcount = processlist_size(processes);
    Job *jobs = jobs_new(processes);
    JobQueue *ready = jobq_new(jobcount);
    Job *selected = NULL;

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
                jobq_add(ready, job);
                fprintf(out, "Time %u: %s arrived\n", tick, job->name);
            }
        }

        if (selected) {
            --selected->burst;
            if (selected->burst == 0) {
                Job *done = jobq_remove(ready);
                fprintf(out, "Time %u: %s finished\n", tick, done->name);
            }
            else continue;
        }

        if (tick == runfor) {
            break;
        }
        else if (jobq_empty(ready)) {
            selected = NULL;
            fprintf(out, "Time %u: IDLE\n", tick);
        }
        else {
            selected = jobq_peek(ready);
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

    jobq_destroy(ready);
    jobs_destroy(jobs);
}

void run_sjf(FILE *out, uint runfor, ProcessList *processes)
{
    size_t jobcount = processlist_size(processes);
    Job *jobs = jobs_new(processes);
    uint min = UINT_MAX;
    ssize_t previous = -1;
    ssize_t selectedindex = -1;

    fprintf (out, "%zu processes\n", jobcount);
    fputs ("Using Shortest Job First (Pre)\n\n", out);

    for (uint tick = 0; tick < runfor; ++tick) {
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
            min = UINT_MAX;
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

    fprintf(out, "%zu processes\n", jobcount);
    fputs("Using Round-Robin\n", out);
    fprintf(out, "Quantum %u\n\n", quantum);

    Job *jobs = jobs_new(processes);

    size_t started = 0;
    size_t finished = 0;
    Job *selected = NULL;
    ssize_t selectedindex = -1;
    uint timeleft = 0;

    for (uint tick = 0; tick < runfor; ++tick) {
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
        if (job->burst > 0) {
            fprintf(out, "%s wait %u turnaround ?\n", job->name, job->wait);
        }
        else {
            fprintf(out, "%s wait %u turnaround %u\n", job->name, job->wait,
                job->turnaround);
        }
    }

    jobs_destroy(jobs);
}

// #endregion ------------------------------------------------------------------
