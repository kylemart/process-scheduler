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

size_t config_processcount(Config *config)
{
    return processlist_size(config->processes);
}

bool config_load(Config **dest, FILE *cf)
{
    Config cfg = { 0, 0, SCHEDULER_UNDEF, NULL };
    size_t cnt = 0;
    lineno = 0;

    if (!read_processcount(&cnt, cf)) {
        return false;
    }
    if (!read_runfor(&cfg.runfor, cf)) {
        return false;
    }
    if (!read_use(&cfg.use, cf)) {
        return false;
    }
    if (!read_quantum(&cfg.quantum, cf) && cfg.use == SCHEDULER_RR) {
        return false;
    }
    if (cnt > 0 && !read_processes(&cfg.processes, cnt, cf)) {
        return false;
    }
    if (!read_end(cf)) {
        return false;
    }

    *dest = malloc(sizeof(Config));
    if (!*dest) {
        error_abort(ERROR_MSG_MALLOC);
    }
    memcpy(*dest, &cfg, sizeof(Config));

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
