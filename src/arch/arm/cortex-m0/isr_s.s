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
.weak SysTick_Handler
.type SysTick_Handler, %function
SysTick_Handler:
    bl     task_save_context
    bl     SysTick_C_Handler
    bl     task_restore_context

.size   SysTick_Handler, . - SysTick_Handler

.thumb_func
.weak SVC_Handler
.type SVC_Handler, %function
SVC_Handler:
    bl     task_save_context
    mrs    r0, psp
    bl     SVC_C_Handler
    bl     task_restore_context

.size   SVC_Handler, . - SVC_Handler

.thumb_func
.weak Default_handler
.type Default_handler, %function
Default_handler:
    b  .

.size Default_handler, . - Default_handler

.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_handler

.weak PendSV_Handler
.thumb_set PendSV_Handler, Default_handler
