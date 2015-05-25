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

#include <stdint.h>

#include "syscall.h"
#include "assembly.h"
#include "platform.h"

void handler_dummy()
__attribute__((naked));
void
handler_dummy()
{
    for (;;) {
    }
}

void handler_svc()
__attribute__((naked));
void
handler_svc()
{
    int code;
    asm(
        "MOV %0, r6"
        : "=r" (code)
    );

    switch(code) {
    case SVC_NICE:
        SysTick->VAL = 1;
        break;
    }
}


#ifdef DEBUG
void unwind_stack( uint32_t *pulFaultStackAddress )
{
  #define __UNUSED(x) (void)(x)
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr;
    volatile uint32_t pc;
    volatile uint32_t psr;

    r0 = pulFaultStackAddress[ 0+4 ];
    r1 = pulFaultStackAddress[ 1+4 ];
    r2 = pulFaultStackAddress[ 2+4 ];
    r3 = pulFaultStackAddress[ 3+4 ];

    r12 = pulFaultStackAddress[ 4+4 ];
    lr = pulFaultStackAddress[ 5+4 ];
    pc = pulFaultStackAddress[ 6+4 ];
    psr = pulFaultStackAddress[ 7+4 ];

    __UNUSED(r0);
    __UNUSED(r1);
    __UNUSED(r2);
    __UNUSED(r3);
    __UNUSED(r12);
    __UNUSED(lr);
    __UNUSED(pc);
    __UNUSED(psr);

    __asm("BKPT #0\n") ;

}
#endif

void handler_hardfault()
__attribute__((naked));
void
handler_hardfault()
{

#ifdef DEBUG
    __asm__ volatile	(
	" movs R0, #4 \n"
	" mov R1, LR \n"
	" tst R0, R1 \n"
	" beq _IS_MSP \n"
	" mrs R0, PSP \n"
	" ldr r2, unwind_stack_label\n"
	" bx r2  \n"
	"_IS_MSP: \n"
	" mrs R0, MSP \n"
	" ldr r2, unwind_stack_label\n"
	" bx r2\n"
	" unwind_stack_label: .word unwind_stack\n"
    );
#endif

    P1_OUT |= 0x1;
    for(;;) {
      __WFI();
    }
    return;
}
