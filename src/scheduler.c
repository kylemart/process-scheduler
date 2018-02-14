#include <string.h>
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
    Job *jobs = malloc(n * sizeof(Job));
    if (!jobs) {
        error_abort(ERROR_MSG_MALLOC);
    }
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
    JobQueue *q = malloc(sizeof(JobQueue));
    if (!q) {
        error_abort(ERROR_MSG_MALLOC);
    }

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
    // ...
}

void run_sjf(FILE *out, uint runfor, ProcessList *processes)
{
    // Im so rusty at C, this'll be fun
    // Im gonna borrow some of your code Kyle ;)
    size_t jobcount = processlist_size(processes);
	fprintf(out, "%zu processes\n", jobcount);
	
	// I modified this one just a little bit though
    fputs("Using Shortest Job First(Preemptive)\n", out);
    
    // Starting the actual SJF code now
    // Make a for loop that goes one at a time through each tick till we run out of time
    Job *jobs = jobs_new(processes);
    uint min = 10000;
    uint prev = 10000;
   for(uint tick = 0; tick < runfor; tick++){
    	// Here I am looping through each process to check its burst time
    	// And printing it to the output file to test
    	for(size_t loop = 0; loop <jobcount ; loop++){
    		// Read its info and print it
    		Job *job = &jobs[loop];
    		if(min > job->burst && job->start <= tick && job->burst != 0){
    			min = loop;
			}
			if(job->start == tick){
				fprintf(out, "Time %u: %s Has Arrived\n",tick, job->name);
			}
			if(job->start<=tick && job->burst!=0){
				jobs[loop].turnaround ++;
				if(min != loop){
					jobs[loop].wait ++;
				}
			}
    		
		}
		//Make sure something has arrived
		if(min <= jobcount){
			if(min != prev){
				fprintf(out, "Time %u: %s selected(Burst: %u)\n", tick, jobs[min].name, jobs[min].burst);
			}
			//fprintf(out, "Time %u: %s is running(Burst: %u)\n", tick, jobs[min].name, jobs[min].burst);
			jobs[min].burst = jobs[min].burst-1;
			if(jobs[min].burst == 0){
				fprintf(out, "Time %u: %s Finished\n", tick, jobs[min].name);
			}
			prev = min;
			min = 10000;
	}	
	else{
			fprintf(out, "Time %u: IDLE\n", tick);
			prev = min;
	}
	}
    fprintf(out,"Finished at time %u\n\n", runfor);
    for(size_t loop = 0; loop <jobcount ; loop++){
    	fprintf(out, "%s wait %u turnaround %u\n", jobs[loop].name, jobs[loop].wait,jobs[loop].turnaround);
	}
    
    
}

void run_rr(FILE *out, uint runfor, uint quantum, ProcessList *processes)
{
    size_t jobcount = processlist_size(processes);

    fprintf(out, "%zu processes\n", jobcount);
    fputs("Using Round-Robin\n", out);
    fprintf(out, "Quantum %u\n\n", quantum);

    Job *jobs = jobs_new(processes);

    JobQueue *ready = jobq_new(jobcount);
    Job *selected = NULL;
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
                jobq_add(ready, job);
                fprintf(out, "Time %u: %s arrived\n", tick, job->name);
            }
        }

        if (selected) {
            --timeleft;
            --selected->burst;
            if (selected->burst == 0) {
                Job *done = jobq_remove(ready);
                fprintf(out, "Time %u: %s finished\n", tick, done->name);
                selected = NULL;
            }
            else if (timeleft == 0) {
                jobq_lshift(ready);
            }
        }

        if (jobq_empty(ready)) {
            fprintf(out, "Time %u: IDLE\n", tick);
        }
        else {
            selected = jobq_peek(ready);
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
    jobq_destroy(ready);
}

// #endregion ------------------------------------------------------------------
