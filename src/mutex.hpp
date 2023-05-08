#ifndef H_MUTEX
#define H_MUTEX

#include <stdbool.h>
#include <atomic>

typedef struct {
    std::atomic<bool> Taken;
} mutex;

mutex MutexCreate();
bool MutexTryLock(mutex *Mutex);
void MutexLock(mutex *Mutex);
void MutexRelease(mutex *Mutex);

#endif // H_MUTEX