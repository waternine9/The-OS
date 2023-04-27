#include "scheduler.h"

void KernelPanic(const char *Fmt, ...)
{
    // TODO: STUB
}

static void *PushRequest(void *Data, size_t Cap, size_t NewCap, size_t ElemSize)
{
    if (NewCap < Cap) {
        KernelPanic("new cap < cap");
        return NULL;
    }
    
    void *Elems = malloc(ElemSize * NewCap);
    memset(Elems, 0, ElemSize * NewCap);
    
    if (Data) {
        memcpy(Elems, Data, ElemSize * Cap);
        free(Data, Cap);
    }  

    return Elems;
}

static process_id ReallocProcesses(scheduler *Scheduler, size_t NewCap)
{
    Scheduler->Processes = PushRequest(Scheduler->Processes, Scheduler->ProcessesCapacity, NewCap, sizeof(scheduler_process));
    Scheduler->ProcessesCapacity = NewCap;
}

static process_id AllocProcessId(scheduler *Scheduler)
{
    if (Scheduler->Processes == NULL) {
        ReallocProcesses(Scheduler, 32);
    }

    if (Scheduler->ProcessesCount == Scheduler->ProcessesCapacity) {
        ReallocProcesses(Scheduler, Scheduler->ProcessesCapacity*2);
    }

    for (int I = 0; I < Scheduler->ProcessesCapacity; I++) {
        if (Scheduler->Processes[I].ID.ID == 0) {
            process_id ID = { 0 };
            ID.ID = I + 1;
            ID.Generation = ++Scheduler->Processes[I].ID.Generation;

            Scheduler->Processes[I].ID = ID;
            Scheduler->ProcessesCount += 1;
            return ID;
        }
    }

    KernelPanic("process cap not resized");
    return (process_id) { 0 };
}

static scheduler_process *GetProcessByID(scheduler *Scheduler, process_id ID)
{
    if ((ID.ID-1) >= Scheduler->ProcessesCapacity) {
        return NULL;
    }

    if (ID.Generation != Scheduler->Processes[ID.ID-1].ID.Generation) {
        return NULL;
    }

    return &Scheduler->Processes[ID.ID-1];
}

static process_id PushOntoRing(scheduler *Scheduler, process_id ID)
{
    if (Scheduler->ProcessRing == NULL) {
        PushRequest(Scheduler->ProcessRing, Scheduler->ProcessRingCapacity, 32, sizeof(process_id));
    }
    if (Scheduler->ProcessRingLength >= Scheduler->ProcessRingCapacity) {
        PushRequest(Scheduler->ProcessRing, Scheduler->ProcessRingCapacity, Scheduler->ProcessRingCapacity*2, sizeof(process_id));
        Scheduler->ProcessRingCapacity *= 2;
    }

    Scheduler->ProcessRing[Scheduler->ProcessRingLength++] = ID;
}

static bool RemoveFromRing(scheduler *Scheduler, process_id ID)
{
    int32_t Index = -1;
    for (int I = 0; I < Scheduler->ProcessRingLength; I++) {
        if (Scheduler->ProcessRing[I].ID == ID.ID) {
            Index = I;
        }
    }

    if (Index == -1) {
        return false;
    }

    memmove(Scheduler->ProcessRing + Index, Scheduler->ProcessRing + Index + 1, sizeof(process_id)*(Scheduler->ProcessRingLength-(Index + 1)));
    Scheduler->ProcessRingLength -= 1;
    
    return true;
}

process_id SchedulerPushProcess(scheduler *Scheduler, scheduler_process Process)
{
    MutexLock(&Scheduler->Mux);

    process_id ID = AllocProcessId(Scheduler);
    scheduler_process *ProcessPtr = GetProcessByID(Scheduler, ID);

    ProcessPtr->Win = Process.Win;
    ProcessPtr->ProcessRequest = Process.ProcessRequest;
    ProcessPtr->Mux.Taken = 0;

    PushOntoRing(Scheduler, ID);

    MutexRelease(&Scheduler->Mux);
    return ID;
}

bool SchedulerRemoveProcess(scheduler *Scheduler, process_id ID)
{
    MutexLock(&Scheduler->Mux);

    scheduler_process *Process = GetProcessByID(Scheduler, ID);
    if (Process) {
        size_t Generation = Process->ID.Generation;
        *Process = (scheduler_process) { 0 };
        Process->ID.Generation = Generation;
        Scheduler->ProcessesCount -= 1;
        RemoveFromRing(Scheduler, ID);
    }

    MutexRelease(&Scheduler->Mux);
    return Process != NULL;
}

void SchedulerExecuteNext(scheduler *Scheduler)
{
    MutexLock(&Scheduler->Mux);

    if (Scheduler->ProcessRingLength != 0) {
        
        if (Scheduler->CurrentProcess >= Scheduler->ProcessRingLength) {
            Scheduler->CurrentProcess = 0;
        }
    
        scheduler_process *Proc = GetProcessByID(Scheduler, Scheduler->ProcessRing[Scheduler->CurrentProcess]);
        if (MutexTryLock(&Proc->Mux)) 
        {
            (*Proc->ProcessRequest)(Proc->Win);
        }
        Scheduler->CurrentProcess++;
        if (Scheduler->CurrentProcess >= Scheduler->ProcessRingLength) {
            Scheduler->CurrentProcess = 0;
        }
    }

    MutexRelease(&Scheduler->Mux);
}

scheduler SchedulerInit()
{
    scheduler Result = { 0 };
    return Result;
}