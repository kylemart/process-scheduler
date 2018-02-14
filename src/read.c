#include <stdarg.h>
#include <string.h>
#include <read.h>
#include <process.h>

ulong lineno;

typedef struct
{
    const char * const string;
    const size_t arg_count;
} Format;

static void preprocess(char *line, size_t length)
{
    line[length - 1] = '\0';
    char *cmtpos = memchr(line, '#', length);
    if (cmtpos) {
        *cmtpos = '\0';
    }
}

static bool get_line_str(char **line, FILE *stream)
{
    size_t size;
    ssize_t length = getline(line, &size, stream);
    if (length >= 0) {
        preprocess(*line, length);
        return true;
    }
    return false;
}

static bool scanf_line(FILE *stream, Format *fmt, ...)
{
    char *line = NULL;
    if (!get_line_str(&line, stream)) {
        return false;
    }
    va_list arg;
    va_start(arg, fmt);
    int scan_count = vsscanf(line, fmt->string, arg);
    va_end(arg);
    return scan_count == fmt->arg_count;
}

bool read_processcount(size_t *result, FILE *stream)
{
    Format fmt = { "processcount %zu", 1 };
    if (scanf_line(stream, &fmt, result)) {
        ++lineno;
        return true;
    }
    return false;
}

bool read_runfor(uint *result, FILE *stream)
{
    Format fmt = { "runfor %u", 1 };
    if (scanf_line(stream, &fmt, result)) {
        ++lineno;
        return true;
    }
    return false;
}

static SchedulerType to_schedulertype(const char *str)
{
    if (strcmp(str, "fcfs") == 0) {
        return SCHEDULER_FCFS;
    }
    else if (strcmp(str, "rr") == 0) {
        return SCHEDULER_RR;
    }
    else if (strcmp(str, "sjf") == 0) {
        return SCHEDULER_SJF;
    }
    return SCHEDULER_UNDEF;
}

bool read_use(SchedulerType *result, FILE *stream)
{
    Format fmt = { "use %5s", 1 };
    char use[6];
    if (scanf_line(stream, &fmt, &use)) {
        *result = to_schedulertype(use);
        if (*result != SCHEDULER_UNDEF) {
            ++lineno;
            return true;
        }
    }
    return false;
}

bool read_quantum(uint *result, FILE *stream)
{
    Format fmt = { "quantum %u", 1 };
    if (scanf_line(stream, &fmt, result)) {
        ++lineno;
        return true;
    }
    return false;
}

static bool read_process(Process **result, FILE *stream)
{
    Format fmt = { "process name %20s arrival %u burst %u", 3 };
    char name[21];
    uint arrival = 0;
    uint burst = 0;
    if (!scanf_line(stream, &fmt, &name, &arrival, &burst)) {
        return false;
    }
    *result = process_new(name, arrival, burst);
    ++lineno;
    return true;
}

bool read_processes(ProcessList **result, size_t n, FILE *stream)
{
    ProcessList *list = processlist_new(n);
    for (size_t i = 0; i < n; ++i) {
        Process *process = NULL;
        if (!read_process(&process, stream)) {
            processlist_destroy(list);
            return false;
        }
        processlist_add(list, process);
    }
    *result = list;
    return true;
}

bool read_end(FILE *stream)
{
    Format fmt = { "%4s", 1 };
    char line[5];
    if (scanf_line(stream, &fmt, line)) {
        return strcmp(line, "end") == 0;
    }
    return false;
}
