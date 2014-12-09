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

extern int init_ok;

#ifdef I2C_MODULE
extern struct	dev_ops i2c;
#endif
#ifdef UART_MODULE
extern struct	dev_ops uart;
#endif
#ifdef PWM_MODULE
extern struct	dev_ops pwm;
#endif
#ifdef LCD_MODULE
extern struct	dev_ops lcd;
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
//        read = scanf("%127s", r);
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


static void i2C_task()
{
#ifdef I2C_MODULE
#define IIC_ADDRESS (0x40)
    struct vnode node;
    uint32_t c = 0;
    uint8_t val[2];
    uint8_t r = 0;
    uint8_t push = 0;
    uint8_t tab[3];
    tab[0] = 1;
    tab[1] = 2;
    
    val[0] = 0;
    val[1] = 3; 
    
    node.dev_type	= I2C;
    node.dev_id		= DEV_I2C;
    int add = IIC_ADDRESS;
    dev_ioctl(&node, I2C_IOCTL_SET_PEER, &add);
    dev_open(&node);    
#endif
    
    for(;;) {
#ifdef I2C_MODULE
	dev_read(&r, sizeof(r), 1, &node);
	if(r & 0x8) {
	  push ^= 0x4;
	}
	val[1] = tab[c] | push;
	dev_write(&val, sizeof(*val), 2, &node);
	c++;
	if (c == 2)
	  c = 0;
#else
        P1_OUT ^= 0x2;
#endif
	
        sys_sleep(500);
	//signal_wait(TASK_SYNCHONISATION(1));
    }
}

#if 0
static void PWM_task()
{
  int i = 0;
  int dir = 1;
#ifdef PWM_MODULE
    struct vnode pwm_node;
    pwm_node.dev_type	= PWM;
    pwm_node.dev_id		= DEV_PWM;
    open(&pwm_node);
#endif
  
    for(;;) {
#ifdef PWM_MODULE
	ioctl(&pwm_node, PWM_IOCTL_SET_DUTY, &i);
	ioctl(&pwm_node, PWM_IOCTL_SET_UPDATE, &i);
#endif
	i+=dir;
	if(i>=100 || i<=0) {
	  dir = -dir;
	}
	
        sleep(50);
    }

}
#endif

static void fatal()
{
    _disable_interrupts;
    P1_OUT |= 0x1;
    __WFI;
    for(;;) {
      _nop;
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
#ifdef I2C_MODULE
    driver_map(DEV_I2C, &i2c);
#endif    
#ifdef UART_MODULE
    driver_map(DEV_UART, &uart);
    struct vnode uart_node;
    uart_node.dev_type	= UART;
    uart_node.dev_id	= DEV_UART;
    dev_open(&uart_node);
#endif    
#ifdef PWM_MODULE
    driver_map(DEV_PWM, &pwm);
#endif    
#ifdef LCD_MODULE
    driver_map(DEV_LCD, &lcd);
    struct vnode lcd_node;
    lcd_node.dev_type	= LCD;
    lcd_node.dev_id	= DEV_LCD;
    dev_open(&lcd_node);
#endif

    if (scheduler_init() != 0)
        fatal();
    initPorts();
    
    if (task_register(uart_task) != 0)
        fatal();
    if (task_register(mess_task) != 0)
        fatal();
    if (task_register(i2C_task) != 0)
        fatal();
#if 0
    if (task_register(PWM_task) != 0)
        fatal();
#endif
   
    init_ok = 1;
    _enable_interrupts;

    for(;;) {
      //__WFI;
      _nop;
    }
}

void _exit()
{
}
