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

	.cpu cortex-m0

  .equ  THREAD_MODE_MPS, 0x0
  .equ  THREAD_MODE_PSP, 0x2


	.text
	.code	16

	.section	.vectors
	@ .section	.vectors,"aw",%progbits
	.align	2
	.word	MAIN_STACK_TOP
	.word	_start
	.word	0
	.word	0
	.long   0x80000100
	.long   0x80000000

  @ This section is loaded in Flash, but the relocation is done in RAM at 0x20000000
	.section	.remapped_vectors, "ax"
	@.section	.remapped_vectors,"ax",%progbits
	.code 16
  .align 2
  .type _vector, function
_vectors:
  .long 0
	 @ Reset
  ldr r0, =_start
  mov pc, r0

  .long 0
	@ Hard Fault
  ldr r0, =HardFault_Handler
  mov pc,r0

  .long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	@ SVCall
  ldr r0,=SVC_Handler
  mov pc,r0

	.long 0
	.long 0

	ldr r0, =PendSV_Handler
	mov pc, r0

	@ SysTick
	ldr r0, =SysTick_Handler
	mov pc, r0

	.long 0                 @ IRQ0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
  .long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0

	.text
	.code	16
	.align	2

/*
  Function: init

  This assembly function is the entry point of the system after reset.

  It initialise BSS, Heap and Stack before running any C code.

  See Also:
  - <_main>
  - <c_init>
*/
	.global _start
	.func 	_start
_start:
  @ Disable interrupts
	CPSID   I
  @ Copy Data from flash to his relocation address
  LDR     r1, =(__data_flash)  @ Src register
  LDR     r3, =(__data_ram)  @ Dst register
  LDR     r5, =(__data_size)               @ Counter

vector_loop:
  LDR     r2,[r1, #0]
  STR     r2,[r3, #0]
  ADD     r1, r1, #4
  ADD     r3, r3, #4
  SUB     r5, #1
  BNE     vector_loop

  @ Initialise BSS
  LDR     r3, =(__bss_start)
  LDR     r5, =(__bss_size)
  MOV     r2, #0
bss_loop:
  STRB    r2,[r3, #0]
  ADD     r3, r3, #1
  SUB     r5, #1
  BNE     bss_loop

  @ Initialise Stack
  LDR     r3, =(__stack_start)
  LDR     r5, =(__stack_size)
  LDR     r2, =0xAB
stack_loop:
  STRB     r2,[r3, #0]
  ADD     r3, r3, #1
  SUB     r5, #1
  BNE     stack_loop

  @ Switch to Thread mode with PSP
  MOV     r3,#(THREAD_MODE_PSP)
  MSR     CONTROL,r3
  LDR     r4,=THREAD_STACK_TOP
  MOV     sp,r4
  ISB
#ifdef STACK_DEBUG
  LDR     r5,=0xDEADBEEF
  push    {r5}
#endif
  BL      c_init

startup:
  BL       _kernel_entry

  .size   startup, . - startup
  .endfunc
