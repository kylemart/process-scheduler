#include <string.h>
#include <config.h>
#include <error.h>
#include <process.h>
#include <read.h>

ulong lineno;

struct Config
{
    uint runfor;
    uint quantum;
    SchedulerType use;
    ProcessList *processes;
};

SchedulerType config_use(Config *config)
{
    return config->use;
}

uint config_runfor(Config *config)
{
    return config->runfor;
}

uint config_quantum(Config *config)
{
    return config->quantum;
}

ProcessList *config_processes(Config *config)
{
    return config->processes;
}

bool config_load(Config **dest, FILE *cf)
{
    Config config = { 0, 0, SCHEDULER_UNDEF, NULL };
    size_t n = 0;
    lineno = 1;

    if (!read_processcount(&n, cf)) {
        return false;
    }
    if (!read_runfor(&config.runfor, cf)) {
        return false;
    }
    if (!read_use(&config.use, cf)) {
        return false;
    }
    if (config.use == SCHEDULER_RR && !read_quantum(&config.quantum, cf)) {
        return false;
    }
    if (n > 0 && !read_processes(&config.processes, n, cf)) {
        return false;
    }
    if (!read_end(cf)) {
        return false;
    }

    *dest = malloc(sizeof(Config));
    if (!*dest) {
        error_abort(ERROR_MSG_MALLOC);
    }
    memcpy(*dest, &config, sizeof(Config));

    return true;
}

void config_destroy(Config *config)
{
    if (!config) {
        return;
    }
    processlist_destroy(config->processes);
    free(config);
}
