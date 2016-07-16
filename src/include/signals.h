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

#ifndef SIGNALS_H
#define SIGNALS_H
/*
 * Topic: Signals
 * Signals are convenient objects in PascalScheduler.
 *
 * They can be used for synchronisation, or to implement upper-half/bottom-half
 * interrupt handler.
 *
 * Synchronisation example:
 * In the following code, the two LEDs are blinking at 1Hz.
 * (start code)
 * static void led1_task()
 * {
 *     for(;;) {
 *         P1_OUT ^= 0x1;
 *         sleep(1000);
 *         signal_raise(SIGNALS_TASK | (SIGNALS_TASK + 1));
 *     }
 *
 * }
 *
 * static void led2_task()
 * {
 *     for(;;) {
 *         P1_OUT ^= 0x2;
 *         sleep(500);
 *         signal_wait(SIGNALS_TASK | (SIGNALS_TASK + 1));
 *     }
 * }
 * (end)
 * Without, the synchronisation, the LED nb 2 would blink at 2Hz, but there the
 * second task have to slow down, and will be fired at the same rate than the
 * first task.
 */

#include <stdint.h>

/*
 * Constants: Signals IDs
 * These constants are used by the scheduler to determine if a task in <TASK_BLOCKED>
 * status can be switched to <TASK_READY>.
 */
#define SIGNALS_RTC	1      /**< Wait for an RTC alarm */
#define SIGNALS_ADC	2      /**< Wait for ADC to finish conversion */
#define SIGNALS_SERIAL	4  /**< Wait for serial interrupt */
#define SIGNALS_TASK	256  /**< Wait for synchronisation from another task */

/*
  Macro: TASK_SYNCHONISATION
  Evaluate as a task ID in SIGNAL_TASK domain.

  Parameters:
  x - uint16_t offset.

  Example:
  > signal_raise(TASK_SYNCHONISATION(1));

  See Also:
    <signal_raise>, <signal_wait>, <Signals IDs>
*/
#define TASK_SYNCHONISATION(x) (SIGNALS_TASK | (SIGNALS_TASK + x))

/*
  Variable: signal_flag
  A flag signaling that the scheduler was called after a <signal_raise>.

  Design:
  This variable may later be replaced.
*/
volatile int signal_flag;
volatile uint32_t signals_flags;

void signal_raise(uint32_t);
void signal_wait(uint32_t);

#endif
