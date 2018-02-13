#include <stdarg.h>
#include <string.h>
#include <read.h>
#include <process.h>

#define FMT_PROCESSCOUNT (0)
#define FMT_RUNFOR       (1)
#define FMT_USE          (2)
#define FMT_QUANTUM      (3)
#define FMT_PROCESS      (4)
#define FMT_END_LINE     (5)
#define END              ("end")
#define COMMENT          ('#')

ulong lineno;

typedef struct
{
    const char * const string;
    const size_t arg_count;
}
ScanFormat;

ScanFormat formats[] = {
    { "processcount %zu", 1 },
    { "runfor %u", 1 },
    { "use %5s", 1 },
    { "quantum %u", 1 },
    { "process name %20s arrival %u burst %u", 3 },
    { "%4s", 1 }
};

static void preprocess(char *line, size_t length)
{
    line[length - 1] = '\0';
    char *cmtpos = memchr(line, COMMENT, length);
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

static bool scanf_line(FILE *stream, int fmt, ...)
{
    char *line = NULL;
    if (!get_line_str(&line, stream)) {
        return false;
    }

    ScanFormat *format = &formats[fmt];

    va_list arg;
    va_start(arg, fmt);
    int scan_count = vsscanf(line, format->string, arg);
    va_end(arg);

    return scan_count == format->arg_count;
}

bool read_processcount(size_t *result, FILE *stream)
{
    if (scanf_line(stream, FMT_PROCESSCOUNT, result)) {
        ++lineno;
        return true;
    }
    return false;
}

bool read_runfor(uint *result, FILE *stream)
{
    if (scanf_line(stream, FMT_RUNFOR, result)) {
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
    char use[6];
    if (scanf_line(stream, FMT_USE, &use)) {
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
    if (scanf_line(stream, FMT_QUANTUM, result)) {
        ++lineno;
        return true;
    }
    return false;
}

static bool read_process(Process **result, FILE *stream)
{
    char name[21];
    uint arrival = 0;
    uint burst = 0;
    if (!scanf_line(stream, FMT_PROCESS, &name, &arrival, &burst)) {
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
    char line[5];
    if (scanf_line(stream, FMT_END_LINE, line)) {
        return strcmp(line, END) == 0;
    }
    return false;
}
