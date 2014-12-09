/*
Copyright Aurélien Chappuis (2014)

aurelienchappuis@ymail.com

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#include "xmc1100.h"
#include "syscall.h"

#define MAX_TASKS  8

/*
 * Constant: TSB_REGISTERS
 * The number of registers to store in task descriptor <task_t>.
 *
 * See Also:
 *   - <scheduler>
 *   - <task_struct>
 */
#define TSB_REGISTERS  4

/*
 * Constants: Task status
 * These constants are used by the scheduler to determine the status of a task.
 *
 * TASK_CREATED   - The task as been created already.
 * TASK_READY     - The task can be scheduled.
 * TASK_RUNNING   - The task is currently running.
 * TASK_BLOCKED   - The task is waiting for a ressource.
 */
#define TASK_CREATED   0x1
#define TASK_READY     0x2
#define TASK_RUNNING   0x4
#define TASK_BLOCKED   0x8

extern const void* THREAD_STACK_TOP;

int init_ok;

/*
 * Structure: task_struct
 * A task descriptor.
 * This task desciptor does not contain PC, because it is stacked before entering the handler.
 * 
 * Attributes:
 *   state        - The current state of the task.
 *   sp           - Address of the top of the task stack.
 *   registers    - Array containing saved registers.
 *   sleep_until  - Time moment when the task can be switched from <TASK_BLOCKED>
 * 			to <TASK_READY>.
 *   wait_for     - If 0 the task has the <TASK_READY> state or sleep_until is set.
 * 			Else the task is in <TASK_BLOCKED> state and is waiting 
 * 			for a signal from an upper half handler or another task.
 */
struct task_struct {
    uint32_t        state;
    void            *sp;
    uint32_t        registers[TSB_REGISTERS];
    long int        sleep_until;
    uint32_t        signal_flag;
};


void scheduler() __attribute__((naked));

int scheduler_init();
int task_register(void(*)(void));

#endif
