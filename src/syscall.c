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


#include <sys/unistd.h>
#include <errno.h>

#include "assembly.h"
#include "syscall.h"
#include "kern.h"
#include "sys/dev.h"
#include "platform.h"

/*
  Function: sleep

  A sleep function.
  If a task call this function, it may sleep for *at least* ms milli-seconds.

  Design:
  <sleep_time> is set to ms and <sleep_flag> to 1. Then the remaining quantum
of time for the current task is set to 0, and the task wait for sleep_flag to
be set to 0. The scheduler *should* clear <sleep_flag>.

*/
void
sys_sleep(long int ms)
{
  sleep_time = ms;
  sleep_flag = 1;
  SysTick->VAL = 0;
  while (sleep_flag == 1) {
    __NOP();
  }
}

/*
  Function: _sbrk

  A simple implementation of sbrk.

  Design:
  This sbrk function return ENOMEM if the __heap_end symbol from linker script
is lower than the next heap_top address.
*/
void *
sys_sbrk(int nbytes)
{
#ifdef TARGET_XMC
  extern const int __heap;
  extern const int __heap_end;
  static void *heap_top = (void *)&__heap;
  static void *heap_end = (void *)&__heap_end;
#endif //TARGET_XMC

  static int c=0;

  void *base;

  if ((heap_top  + nbytes) > heap_end) {
    errno = ENOMEM;
    return (void *)-1;
  }

  c++;
  base = heap_top;
  heap_top += nbytes;

  return base;
}

int
sys_open(const char *name, int flags, int mode)
{
    __UNUSED(name);
    __UNUSED(flags);
    __UNUSED(mode);

    return -1;
}

int
sys_close(int fd)
{
    __UNUSED(fd);
    return -1;
}

int
sys_read(int file, char *ptr, int len)
{
    struct vnode vnode;

    switch (file) {
    case STDIN_FILENO:
      vnode.dev_type	= UART;
      vnode.dev_id	= DEV_UART;
      break;
    default:
      errno = EBADF;
      return -1;
      break;
    }

    return dev_read(ptr, len, &vnode);
}

int
sys_write(int file, char *ptr, int len)
{
    struct vnode vnode;

    switch (file) {
    case STDOUT_FILENO:
    case STDERR_FILENO:
      vnode.dev_type	= UART;
      vnode.dev_id	= DEV_UART;
      break;
    default:
      errno = EBADF;
      return -1;
      break;
    }


    return dev_write(ptr, len, &vnode);
}

int
sys_getjiffy(void)
{
    extern volatile long int jiffy;
    return jiffy;
}
