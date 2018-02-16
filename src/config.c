#include <string.h>
#include <config.h>
#include <error.h>
#include <process.h>
#include <read.h>

#define try(read) if (!read) return false

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

    try(read_processcount(&n, cf));
    try(read_runfor(&config.runfor, cf));
    try(read_use(&config.use, cf));
    if (config.use == SCHEDULER_RR) {
        try(read_quantum(&config.quantum, cf));
    }
    if (n > 0) {
        try(read_processes(&config.processes, n, cf));
    }
    try(read_end(cf));

    *dest = amalloc(sizeof(Config));
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
