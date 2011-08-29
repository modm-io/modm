/*
    FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

#include "FreeRTOSConfig.h"

	.section .text
	.syntax	unified
	.align	4
	.thumb
	
	// ------------------------------------------------------------------------
	.global	SVC_Handler
	.type	SVC_Handler, %function
	.thumb_func
	.func	SVC_Handler
SVC_Handler:
	ldr		r3, =pxCurrentTCB	/* Load address of pxCurrentTCB */
	ldr		r1, [r3]
	ldr		r0, [r1]			/* The first item in pxCurrentTCB is the task top of stack. */
	ldmia	r0!, {r4-r11}		/* Pop the registers that are not automatically
								   saved on exception entry and the critical nesting count. */
	msr		psp, r0				/* Restore the task stack pointer. */
	mov		r0, #0
	msr		basepri, r0
	orr		r14, #0xd
	bx		r14
	.endfunc

	// ------------------------------------------------------------------------
	.global	vPortStartFirstTask
	.type	vPortStartFirstTask, %function
	.thumb_func
	.func	vPortStartFirstTask
vPortStartFirstTask:
	ldr		r0, =0xE000ED08		/* Use the NVIC offset register to locate the stack. */
	ldr		r0, [r0]
	ldr		r0, [r0]
	msr		msp, r0				/* Set the msp back to the start of the stack. */
	cpsie	i					/* Globally enable interrupts. */
	svc		0					/* System call to start first task. */
	nop
	.endfunc

// ------------------------------------------------------------------------
	.global	PendSV_Handler
	.type	PendSV_Handler, %function
	.thumb_func
	.func	PendSV_Handler
PendSV_Handler:
	mrs		r0, psp
	
	ldr		r3, =pxCurrentTCB	/* Get the location of the current TCB. */
	ldr		r2, [r3]
	
	stmdb	r0!, {r4-r11}		/* Save the remaining registers. */
	str		r0, [r2]			/* Save the new top of stack into the first member of the TCB. */
	
	stmdb	sp!, {r3, r14}
	mov		r0, configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr		basepri, r0
	bl		vTaskSwitchContext
	mov		r0, #0
	msr		basepri, r0
	ldmia	sp!, {r3, r14}		/* Restore the context, including the critical nesting count. */
	
	ldr		r1, [r3]
	ldr		r0, [r1]			/* The first item in pxCurrentTCB is the task top of stack. */
	ldmia	r0!, {r4-r11}		/* Pop the registers. */
	msr		psp, r0
	bx		r14
	.endfunc

