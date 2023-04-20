#ifndef H_TOS_MUTEX
#define H_TOS_MUTEX

#include <stdbool.h>

typedef struct {
    _Atomic bool Taken;
} mutex;

mutex MutexCreate();
bool MutexTryLock(mutex *Mutex);
void MutexLock(mutex *Mutex);
void MutexRelease(mutex *Mutex);

#endif