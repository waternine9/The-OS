///
/// BRIEF: Thread-safe scheduler
///

#ifndef H_TOS_SCHEDULER
#define H_TOS_SCHEDULER

#include <stddef.h>
#include "mutex.h"
#include "mem.h"
#include "OS.h"

typedef struct {
    size_t ID;
    size_t Generation;
} process_id;
typedef void (*scheduler_process_request)(window* Win);


typedef struct {
    process_id ID;
    window *Win;
    scheduler_process_request ProcessRequest;
    mutex Mux;
    uint8_t Priority;
} scheduler_process;

typedef struct {
    mutex Mux;

    scheduler_process *Processes;
    size_t ProcessesCapacity;
    size_t ProcessesCount;
    
    process_id *ProcessRing;
    _Atomic size_t ProcessRingCapacity;
    _Atomic size_t ProcessRingLength;
    _Atomic size_t CurrentProcess;
    _Atomic size_t CurrentProcessPriority;
} scheduler;

process_id SchedulerPushProcess(scheduler *Scheduler, scheduler_process Process);
bool SchedulerRemoveProcess(scheduler *Scheduler, process_id ID);
void SchedulerExecuteNext(scheduler *Scheduler);
void SchedulerUnlockProcesses(scheduler *Scheduler);

scheduler_process *SchedulerGetProcessByID(scheduler *Scheduler, process_id ID);
scheduler SchedulerInit();
#endif // H_TOS_SCHEDULER