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

#include "scheduler.h"
#include "signals.h"
#include "syscall.h"

/*
  Variable: current_tid
  Index of the currently running task.
*/
static int current_tid;

/*
  Variable: registered_tasks
  Number of registered tasks.

  The <_main> function is the *idle* task, and rt should never be less than 1.
*/
static int registered_tasks;

/*
  Variable: tasks
  An array of <task_struct> containing all the system tasks.
*/
static struct task_struct tasks[MAX_TASKS];

/*
  Variable: jiffy
  Number of scheduler call since startup.
*/
volatile long int jiffy;

/*
  Macro: _save_context
  Save context of the interrupted task.

  Parameters:
  sp - uint32_t variable where stack address will be saved.
  registers - array where some registers will be saved.

  See Also:
    <_restore_context>
*/
#define _save_context(_x_,_y_) {\
        uint32_t tmp;\
        asm volatile(\
        "mov    %0, %2\n"\
        "stmia  %0!, {r4-r7}\n"\
        "mrs %1, psp\n"\
        : "=r" (tmp),  "=r" (_x_)\
        : "g" (_y_)\
    );\
    }

/*
  Macro: _restore_context
  Restore context of the interrupted task.

  Parameters:
  sp - uint32_t variable containing stack address.
  registers - array where some registers were saved.

  See Also:
    <_save_context>
*/

#define _restore_context(_x_,_y_) {\
        uint32_t tmp;\
        asm volatile(\
        "mov    %0, %1\n"\
        "ldmia  %0!, {r4-r7}\n"\
        : "=r" (tmp)\
        : "g" (_y_)\
        : "r4", "r5", "r6"\
    );\
    asm volatile(\
        "msr psp, %1\n"\
        "isb\n"\
        "ldr %0, =0xfffffffd\n"\
        "mov lr, %0\n"\
        "bx lr\n"\
        : "=r" (tmp)\
        : "r" (_x_)\
    );\
    }

/*
  Function: scheduler

  A handler for systick exception.

  This function as a *naked* attribute.

  Design:
  This scheduler is for now implemented as a Round Robin Scheduler.

  See Also:
    <task_register>
*/
void
scheduler()
{
    _save_context(tasks[current_tid].sp, tasks[current_tid].registers);
    tasks[current_tid].state = TASK_CREATED | TASK_READY;
    jiffy++;

    if (sleep_flag == 1) { // Scheduler called by a sleep
      sleep_flag = 0;
      tasks[current_tid].state = TASK_CREATED | TASK_BLOCKED;
      tasks[current_tid].sleep_until = jiffy + sleep_time;
      tasks[current_tid].signal_flag = 0;
    }
    else if (signal_flag != 0) { // Scheduler called by a signal_wait
      tasks[current_tid].state = TASK_CREATED | TASK_BLOCKED;
      tasks[current_tid].sleep_until = 0;
      tasks[current_tid].signal_flag = signal_flag;
      signal_flag = 0;
    }

    /*
     * Choose task
     */
    do {
      current_tid++;
      if(current_tid >= registered_tasks) {
	  current_tid = 0;
      }
    } while (!(
      (tasks[current_tid].state & TASK_READY) ||
      (
	(tasks[current_tid].state & TASK_BLOCKED) &&
	(
	  (jiffy > tasks[current_tid].sleep_until && tasks[current_tid].sleep_until !=0 ) ||
	  ((tasks[current_tid].signal_flag & signals_flags) != 0)
	)
      )
      ));

    if ((tasks[current_tid].signal_flag & signals_flags) != 0 && tasks[current_tid].sleep_until == 0) {
      signals_flags &= ~(tasks[current_tid].signal_flag);
    }

    tasks[current_tid].state = TASK_CREATED | TASK_RUNNING;
    _restore_context(tasks[current_tid].sp, tasks[current_tid].registers);

}
void SysTick_Handler() __attribute__((naked, alias ("scheduler")));

/*
  Function scheduler_init
*/
int
scheduler_init(void)
{
    registered_tasks = 1;
    current_tid = 0;
    tasks[current_tid].state = TASK_CREATED | TASK_RUNNING;
    return 0;
}

/*
  Function: task_register

  Register a new task.

  Parameters:
    task_func - A function pointer.

  Returns:
    0 if successful, an error code otherwise.

  See Also:
    <scheduler>
*/
int
task_register(void(*task_func)(void))
{
    void* sp;
    void* old_sp;
    uint32_t tmp1, tmp2;
    int tid = registered_tasks;

    if(tid >= MAX_TASKS) {
        return -1;
    }

#ifdef TARGET_XMC
    sp = (&THREAD_STACK_TOP - 512 * tid);
#endif //TARGET_XMC

    // Create stackframe
    asm volatile(
        "mov  %0, sp\n"             // save sp register into old_sp variable
        "ldr  %1,%3\n"              // load previously computed sp variable into
        "mov  sp,%1\n"              // sp register. From now we can push into
        "isb\n"                     // the task stack
#ifdef STACK_DEBUG
        "ldr  %2,=0xDEADBEEF\n"
        "push {%2}\n"
#endif
        "ldr  %2,=0x21000000\n"
        "push {%2}\n"               // Push xPSR  = 0x01000000 -> Thumb
        "ldr  %2,%4\n"
        "push {%2}\n"               // Push pc = function
        "push {%2}\n"               // Push lr = function --> auto reload of the function on return.
        "ldr  %2,=0x0\n"
        "push {%2}\n"               // Push r12 = 0
        "push {%2}\n"               // Push r3 = 0
        "push {%2}\n"               // Push r2 = 0
        "push {%2}\n"               // Push r1 = 0
        "push {%2}\n"               // Push r0 = 0
        "mov  sp, %0\n"             // Restore previous stack
        "isb\n"
        : "=r" (old_sp), "=r" (tmp1), "=r" (tmp2)
        : "g" (sp), "g" (task_func)
    );
#ifdef STACK_DEBUG
    tasks[tid].sp   = sp - 36;
#else
    tasks[tid].sp   = sp - 32;
#endif
    tasks[tid].state   = TASK_READY;

    registered_tasks++;
    return 0;
}
