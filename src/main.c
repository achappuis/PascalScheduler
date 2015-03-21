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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include "sys/dev.h"

#include "xmc1100.h"
#include "assembly.h"
#include "signals.h"
#include "syscall.h"
#include "scheduler.h"

#include "dev/i2c/i2c.h"
#include "dev/pwm/pwm.h"

#include "circular_buffer.h"

#include <xmc_gpio.h>
#include <xmc_uart.h>

extern int init_ok;

#ifdef UART_MODULE
extern struct	dev_ops uart;
#endif
struct 		dev_ops* devices[DRIVER_NB];

enum errno_e errno;

/*
 * Function: _delay
 * A simple delay function.
 *
 * The implementation of this function relies on a loop.
 *
 * Parameter:
 * len - The number of loop iteration to do before returning.
 */
static void _delay(unsigned len)
{
    while(len--);
}

static void initPorts()
{
    // LED's are on P1.0 and P1.1
    // So make these push-pull outputs
    P1_IOCR0 = BIT15 + BIT7;
    //P1_OUT = 0x02;

    P0_IOCR0 = BIT7;
    P0_OUT = 0x00;
}

static void uart_task()
{

#ifdef UART_MODULE
//    char r[128];
    int read = 0;
#endif
    for(;;) {
#ifdef UART_MODULE
      // read = scanf("%127s", r);
      printf("Alive %d!\n\r", read);
	    read++;
#endif
        P0_OUT ^= 0x1;
        sys_sleep(1000);
    }

}

static void mess_task()
{

#ifdef UART_MODULE
    char c;
#endif
    for(;;) {
#ifdef UART_MODULE
        c = getchar();
        printf("%c", c);
#endif
    }
}

static void fatal()
{
    __disable_irq();
    P1_OUT |= 0x1;
    __WFI();
    for(;;) {
      __NOP();
    }
}

/*
 * Function: _main
 *
 * The <_main> function is the *idle* task. It should never return.
 * It's the main entry point for the system after <init> and <c_init>.
 */
void _main()
{
  /*
   * Some drivers rely on the value of SystemCoreClock
   * SystemCoreClockUpdate() update this value.
   */
  SystemCoreClockUpdate();

#ifdef UART_MODULE
    driver_map(DEV_UART, &uart);
    struct vnode uart_node;
    uart_node.dev_type	= UART;
    uart_node.dev_id	= DEV_UART;
    XMC_UART_CH_CONFIG_t uart_config1;
    uart_node.data	= &uart_config1;
    dev_open(&uart_node);
#endif

    if (scheduler_init() != 0)
        fatal();
    initPorts();

    if (task_register(uart_task) != 0)
        fatal();
    if (task_register(mess_task) != 0)
        fatal();

    init_ok = 1;
    __enable_irq();

    for(;;) {
      __WFI();
      __NOP();
    }
}

void _exit()
{
}
