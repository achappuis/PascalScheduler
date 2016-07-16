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

#include <stdio.h>

#include "dev/rtc/rtc.h"
#include "mutex.h"
#include "syscall.h"
#include "time.h"
#include "kernel.h"

extern struct vnode rtc_node;

struct tm myRTC;

void RTC_update()
{
    dev_ioctl(&rtc_node, RTC_IOCTL_GET_TIME, &myRTC);
}

void RTC_print()
{
  printf("RTC reports : %d/%d/%d %d:%d:%d!\n",
    myRTC.tm_mday, myRTC.tm_mon, myRTC.tm_year,
    myRTC.tm_hour, myRTC.tm_min, myRTC.tm_sec);
}

OS_TASK(uart_task)
{
    myRTC.tm_mday = 17;
    myRTC.tm_mon  = 6;
    myRTC.tm_year = 16;
    myRTC.tm_hour = 14;
    myRTC.tm_min  = 53;
    myRTC.tm_sec  = 0;
    dev_ioctl(&rtc_node, RTC_IOCTL_SET_TIME, &myRTC);
    sys_sleep(100);
    for(;;) {
        mutex_lock();
        RTC_update();
        RTC_print();
        mutex_unlock();
        sys_yield();
    }
}

OS_TASK(mutex_task)
{
    for(;;) {
        mutex_lock();
        sys_sleep(1000 * 2);
        mutex_unlock();
        sys_yield();
    }
}

OS_TASK_LIST = {
  uart_task,
  mutex_task,
  0
};
