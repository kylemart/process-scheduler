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
    // ...
}

void run_sjf(FILE *out, uint runfor, ProcessList *processes)
{
    // Im so rusty at C, this'll be fun
    // Im gonna borrow some of your code Kyle ;)
    size_t jobcount = processlist_size(processes);
    fprintf (out, "%zu processes\n", jobcount);
	
	// I modified this one just a little bit though
    fputs ("Using Shortest Job First (Pre)\n\n", out);
    
    // Starting the actual SJF code now
    // Make a for loop that goes one at a time through each tick till we run out of time
    Job *jobs = jobs_new(processes);
    uint min = 2147483646;
    uint prev = 2147483646;
    uint loop_hold = 2147483646;
    for (uint tick = 0; tick < runfor; tick++){
    	// Here I am looping through each process to check its burst time
    	// And printing it to the output file to test
        for (size_t loop = 0; loop < jobcount ; loop++){
    		// Read its info and print it
    		Job *job = &jobs[loop];
    		if (min > job->burst && job->start <= tick && job->burst != 0){
    			min = job->burst;
    			loop_hold = loop;
			}
			if (job->start == tick){
				fprintf(out, "Time %u: %s arrived\n",tick, job->name);
			}
			if (job->start <= tick && job->burst != 0){
				jobs[loop].turnaround ++;
				if (min != loop){
					jobs[loop].wait ++;
				}
			}
    		
		}
		//Make sure something has arrived
		if (loop_hold <= jobcount){
			if (loop_hold != prev){
				fprintf(out, "Time %u: %s selected (burst %u)\n", tick, jobs[loop_hold].name, jobs[loop_hold].burst);
			}
			jobs[loop_hold].burst = jobs[loop_hold].burst-1;
            if (jobs[loop_hold].burst == 0){
				fprintf(out, "Time %u: %s finished\n", tick+1, jobs[loop_hold].name);
			}
			prev = loop_hold;
			min = 2147483646;
			loop_hold = 2147483646;
			
		}	
		else {
		    fprintf (out, "Time %u: IDLE\n", tick);
			prev = loop_hold;
		}
	}
    fprintf (out,"Finished at time %u\n\n", runfor);
    for (size_t loop = 0; loop <jobcount ; loop++){
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
                selected = NULL;
                ++finished;
            }
            else if (timeleft == 0) {
                selected = NULL;
            }
        }

        if (started == finished) {
            fprintf(out, "Time %u: IDLE\n", tick);
        }
        else if (!selected) {
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
