#ifndef FIXEDPRIORITY_H
#define FIXEDPRIORITY_H

#include "os.h"
#include "sleep.h"

extern OS_Scheduler_t const fixedPriorityScheduler;

#define PRIORITY_QUEUE_MAX_TASKS 8

#endif /* FIXEDPRIORITY_H */
