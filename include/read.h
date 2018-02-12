#ifndef READ_H
#define READ_H

#include <stdbool.h>
#include <stdio.h>
#include <types.h>

extern ulong lineno;

bool read_use(SchedulerType *result, FILE *stream);

bool read_process(Process **result, FILE *stream);

bool read_processes(ProcessList **result, size_t n, FILE *stream);

bool read_runfor(uint *result, FILE *stream);

bool read_processcount(size_t *result, FILE *stream);

bool read_quantum(uint *result, FILE *stream);

bool read_end(FILE *stream);

#endif
