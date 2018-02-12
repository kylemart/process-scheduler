#include <stdarg.h>
#include <stdio.h>
#include <config.h>
#include <error.h>
#include <scheduler.h>

static Config *config = NULL;

static void get_config(const char *filepath)
{
    FILE *cf = fopen(filepath, "r");
    if (!cf) {
        error_exit("couldn't open %s", filepath);
    }
    if (!config_load(&config, cf)) {
        error_exit("line %u in %s", lineno, filepath);
    }
    fclose(cf);
}

static void cleanup()
{
    config_destroy(config);
}

int main()
{
    get_config("processes.in");

    atexit(cleanup);

    FILE *out = fopen("processes.out", "w");
    if (!out) {
        error_exit("couldn't create output file");
    }

    uint runfor = config_runfor(config);
    uint quantum = config_quantum(config);
    ProcessList *processes = config_processes(config);
    switch (config_use(config)) {
        case SCHEDULER_RR:
            run_rr(out, runfor, quantum, processes);
            break;
        case SCHEDULER_FCFS:
            run_fcfs(out, runfor, processes);
            break;
        case SCHEDULER_SJF:
            run_sjf(out, runfor, processes);
            break;
        default:
            error_exit("unimplemented scheduler");
    }

    fclose(out);

    exit(EXIT_SUCCESS);
}
