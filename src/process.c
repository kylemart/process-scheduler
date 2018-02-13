#include <string.h>
#include <error.h>
#include <process.h>

// #region Process -------------------------------------------------------------

struct Process
{
    char *name;
    uint arrival;
    uint burst;
};

Process *process_new(char *name, uint arrival, uint burst)
{
    Process *p = malloc(sizeof(Process));
    if (!p) {
        error_abort(ERROR_MSG_MALLOC);
    }
    p->name = malloc(strlen(name) + 1);
    if (!p->name) {
        free(p);
        error_abort(ERROR_MSG_MALLOC);
    }
    strcpy(p->name, name);
    p->arrival = arrival;
    p->burst = burst;
    return p;
}

void process_destroy(Process *process)
{
    if (!process) {
        return;
    }
    free(process->name);
    free(process);
}

const char *process_name(Process *process)
{
    return process->name;
}

uint process_burst(Process *process)
{
    return process->burst;
}

uint process_arrival(Process *process)
{
    return process->arrival;
}

// #endregion ------------------------------------------------------------------

// #region ProcessList ---------------------------------------------------------

struct ProcessList
{
    Process **items;
    size_t capacity;
    size_t size;
};

ProcessList *processlist_new(size_t capacity)
{
    ProcessList *list = malloc(sizeof(ProcessList));
    if (!list) {
        error_abort(ERROR_MSG_MALLOC);
    }
    list->items = calloc(capacity, sizeof(Process *));
    if (!list->items) {
        free(list);
        error_abort(ERROR_MSG_MALLOC);
    }
    list->capacity = capacity;
    list->size = 0;
    return list;
}

size_t processlist_size(ProcessList *list)
{
    if (!list) {
        return 0;
    }
    return list->size;
}

bool processlist_add(ProcessList *list, Process *process)
{
    if (!list || list->size == list->capacity) {
        return false;
    }
    list->items[list->size++] = process;
    return true;
}

Process *processlist_get(ProcessList *list, size_t index)
{
    return list->items[index];
}

void processlist_destroy(ProcessList *list)
{
    if (!list) {
        return;
    }
    if (list->items) {
        for (size_t i = 0; i < list->size; ++i) {
            process_destroy(list->items[i]);
        }
        free(list->items);
    }
    free(list);
}

// #endregion ------------------------------------------------------------------
