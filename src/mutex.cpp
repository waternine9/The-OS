#include "mutex.hpp"
#include "io.hpp"

mutex MutexCreate() {
    return (mutex) { false };
}

bool MutexTryLock(mutex *Mutex) {
    if (Mutex->Taken) {
        return false;
    }
    Mutex->Taken = true;
    return true;
}

void MutexLock(mutex *Mutex) {
    while (Mutex->Taken) {
        // NOTE: Stalls the CPU for a while
        IO_Wait();
    }

    Mutex->Taken = true;
}

void MutexRelease(mutex *Mutex) {
    Mutex->Taken = false;
}