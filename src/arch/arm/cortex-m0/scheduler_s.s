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

.thumb_func
.weak task_save_context
.type task_save_context, %function
task_save_context:
    ldr   r0, =tasks
    ldr   r2, =current_tid
    ldr   r1, [r2]
    mov   r2, #32
    mul   r1, r1, r2
    add   r0, r0, r1    // r0 -> tasks[current_tid].sp
    mrs   r1, psp
    stm   r0!, {r1}     // r0 -> tasks[current_tid].registers
    stmia r0!, {r4-r7}
    isb
    mov   pc, lr

.size   task_save_context, . - task_save_context



.thumb_func
.weak task_restore_context
.type task_restore_context, %function
task_restore_context:
    ldr   r0, =tasks
    ldr   r2, =current_tid
    ldr   r1, [r2]
    mov   r2, #32
    mul   r1, r1, r2
    add   r0, r0, r1 // r0 -> tasks[current_tid].sp
    ldm   r0!, {r1}  // r0 -> tasks[current_tid].registers
    msr   psp, r1
    ldmia r0!, {r4-r7}
    isb
    ldr   r0, =0xfffffffd
    mov   lr, r0
    bx    lr

.size   task_restore_context, . - task_restore_context
