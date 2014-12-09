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

#ifndef SYSCALL_H
#define SYSCALL_H

#define SVC_NICE  1


#define __nice  asm volatile("MOV r6,%0\n"\
    "SVC 0x0\n":: "" (SVC_NICE):"r6")

/*
  Variable: sleep_flag
  A flag signaling that the scheduler was called after a sleep request.
  
  Design:
  This variable may later be replaced.
*/
volatile int sleep_flag;

/*
  Variable: sleep_time
  The amount of time the last scheduled task should sleep.
*/
volatile long int sleep_time;

void sys_sleep(long int);
void *sys_sbrk(int);
int  sys_open(const char*, int, int);
int  sys_close(int);
int  sys_read(int, char*, int);
int  sys_write(int, char*, int);
int  sys_getjiffy(void);
int  sys_isatty(int);

#endif
