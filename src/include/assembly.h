#ifndef ASSEMBLY_H
#define ASSEMBLY_H
 
#define _nop  asm("nop")

#define _enable_interrupts  asm("cpsie i")
#define _disable_interrupts  asm("cpsid i")


#define __WFE  asm("WFE")
#define __WFI  asm("WFI")

#endif
