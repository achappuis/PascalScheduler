#ifndef KERNEL_H
#define KERNEL_H

#define OS_TASK(_x) void _x()

#define OS_TASK_LIST void (*OS_Tasks[]) (void)

#endif
