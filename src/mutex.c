#include "mutex.h"
#include "syscall.h"

void mutex_lock()
{
    asm volatile("SVC %0\n":: "" (SVC_MUTEX_LOCK) :);
}

void mutex_unlock()
{
    asm volatile("SVC %0\n":: "" (SVC_MUTEX_UNLOCK) :);
}
