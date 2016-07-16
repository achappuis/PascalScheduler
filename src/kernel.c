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

#include "dev/mcu/mcu.h"
#include "dev/rtc/rtc.h"
#include "kernel.h"
#include "scheduler.h"

#include "xmc_gpio.h"
#include "xmc_uart.h"
#include "xmc_i2c.h"

#ifdef UART_MODULE
extern struct	dev_ops uart;
#endif
#ifdef I2C_MODULE
extern struct	dev_ops i2c;
#endif
#ifdef RTC_MODULE
extern struct	dev_ops rtc;
#endif
struct 		dev_ops* devices[DRIVER_NB];

enum errno_e errno;

struct vnode uart_node;
struct vnode i2c_node;
struct vnode rtc_node;

extern OS_TASK_LIST;

static void initPorts()
{
    XMC_GPIO_SetMode(P0_5, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    XMC_GPIO_SetOutputHigh(P0_5);
}

static void fatal()
{
    __disable_irq();
    XMC_GPIO_SetOutputLow(P0_5);
    __WFI();
    for(;;) {
      __NOP();
    }
}


/*
 * Function: _kernel_entry
 *
 * The <_kernel_entry> function is the *idle* task. It should never return.
 * It's the main entry point for the system after <init> and <c_init>.
 */
void _kernel_entry()
{
  /*
   * Some drivers rely on the value of SystemCoreClock
   * SystemCoreClockUpdate() update this value.
   */
  SystemCoreClockUpdate();
  NVIC_SetPriority(SVCall_IRQn, 0);
  NVIC_SetPriority(SysTick_IRQn, 2);
  initPorts();

#ifdef UART_MODULE
    driver_map(DEV_UART, &uart);
    uart_node.dev_type	= UART;
    uart_node.dev_id	= DEV_UART;
    XMC_UART_CH_CONFIG_t uart_config1;
    uart_node.data	= &uart_config1;
    dev_open(&uart_node);
#endif
#ifdef I2C_MODULE
    driver_map(DEV_I2C, &i2c);
    i2c_node.dev_type	= I2C;
    i2c_node.dev_id	= DEV_I2C;
    XMC_I2C_CH_CONFIG_t i2c_config1;
    i2c_node.data	= &i2c_config1;
    dev_open(&i2c_node);
#endif
#ifdef RTC_MODULE
    driver_map(DEV_RTC, &rtc);
    rtc_node.dev_type	= DEV_T_RTC;
    rtc_node.dev_id	= DEV_RTC;
    dev_open(&rtc_node);
#endif

    printf("PascalScheduler R0\n");
    printf(" -- %s\n", get_mcu_string());
    printf(" -- Flash : %dkB\n", get_flash_size());
    printf(" -- RAM   : %dB\n", get_ram_size());


    if (scheduler_init() != 0) {
      fatal();
    }

    int i = 0;
    while ((*OS_Tasks[i]) != 0) {
      if (k_task_register(*OS_Tasks[i]) != 0) {
          fatal();
      }
      i++;
    }

    for(;;) {
      __WFI();
      __NOP();
    }
}

void _exit()
{
}
