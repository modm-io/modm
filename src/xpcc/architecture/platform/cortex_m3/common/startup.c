// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above copyright
 *	   notice, this list of conditions and the following disclaimer in the
 *	   documentation and/or other materials provided with the distribution.
 *	 * Neither the name of the Roboterclub Aachen e.V. nor the
 *	   names of its contributors may be used to endorse or promote products
 *	   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>
#include "xpcc_config.hpp"

// TODO only for STM32
#include <stm32f10x.h>

// ----------------------------------------------------------------------------
void resetHandler(void);
void ATTRIBUTE_WEAK nmiHandler(void);
void ATTRIBUTE_WEAK hardfaultHandler(void);

void ATTRIBUTE_WEAK MemManage_Handler(void);
void ATTRIBUTE_WEAK BusFault_Handler(void);
void ATTRIBUTE_WEAK UsageFault_Handler(void);
//mthomas void ATTRIBUTE_WEAK MemManage_Handler(void);
void ATTRIBUTE_WEAK SVC_Handler(void);
void ATTRIBUTE_WEAK DebugMon_Handler(void);
void ATTRIBUTE_WEAK PendSV_Handler(void);
void ATTRIBUTE_WEAK SysTick_Handler(void);

/* External Interrupts */
void ATTRIBUTE_WEAK WWDG_IRQHandler(void);
void ATTRIBUTE_WEAK PVD_IRQHandler(void);
void ATTRIBUTE_WEAK TAMPER_IRQHandler(void);
void ATTRIBUTE_WEAK RTC_IRQHandler(void);
void ATTRIBUTE_WEAK FLASH_IRQHandler(void);
void ATTRIBUTE_WEAK RCC_IRQHandler(void);
void ATTRIBUTE_WEAK EXTI0_IRQHandler(void);
void ATTRIBUTE_WEAK EXTI1_IRQHandler(void);
void ATTRIBUTE_WEAK EXTI2_IRQHandler(void);
void ATTRIBUTE_WEAK EXTI3_IRQHandler(void);
void ATTRIBUTE_WEAK EXTI4_IRQHandler(void);
void ATTRIBUTE_WEAK DMA1_Channel1_IRQHandler(void);
void ATTRIBUTE_WEAK DMA1_Channel2_IRQHandler(void);
void ATTRIBUTE_WEAK DMA1_Channel3_IRQHandler(void);
void ATTRIBUTE_WEAK DMA1_Channel4_IRQHandler(void);
void ATTRIBUTE_WEAK DMA1_Channel5_IRQHandler(void);
void ATTRIBUTE_WEAK DMA1_Channel6_IRQHandler(void);
void ATTRIBUTE_WEAK DMA1_Channel7_IRQHandler(void);
void ATTRIBUTE_WEAK ADC1_2_IRQHandler(void);
void ATTRIBUTE_WEAK USB_HP_CAN1_TX_IRQHandler(void);
void ATTRIBUTE_WEAK USB_LP_CAN1_RX0_IRQHandler(void);
void ATTRIBUTE_WEAK CAN1_RX1_IRQHandler(void);
void ATTRIBUTE_WEAK CAN1_SCE_IRQHandler(void);
void ATTRIBUTE_WEAK EXTI9_5_IRQHandler(void);
void ATTRIBUTE_WEAK TIM1_BRK_IRQHandler(void);
void ATTRIBUTE_WEAK TIM1_UP_IRQHandler(void);
void ATTRIBUTE_WEAK TIM1_TRG_COM_IRQHandler(void);
void ATTRIBUTE_WEAK TIM1_CC_IRQHandler(void);
void ATTRIBUTE_WEAK TIM2_IRQHandler(void);
void ATTRIBUTE_WEAK TIM3_IRQHandler(void);
void ATTRIBUTE_WEAK TIM4_IRQHandler(void);
void ATTRIBUTE_WEAK I2C1_EV_IRQHandler(void);
void ATTRIBUTE_WEAK I2C1_ER_IRQHandler(void);
void ATTRIBUTE_WEAK I2C2_EV_IRQHandler(void);
void ATTRIBUTE_WEAK I2C2_ER_IRQHandler(void);
void ATTRIBUTE_WEAK SPI1_IRQHandler(void);
void ATTRIBUTE_WEAK SPI2_IRQHandler(void);
void ATTRIBUTE_WEAK USART1_IRQHandler(void);
void ATTRIBUTE_WEAK USART2_IRQHandler(void);
void ATTRIBUTE_WEAK USART3_IRQHandler(void);
void ATTRIBUTE_WEAK EXTI15_10_IRQHandler(void);
void ATTRIBUTE_WEAK RTCAlarm_IRQHandler(void);
void ATTRIBUTE_WEAK USBWakeUp_IRQHandler(void);

/*
 * Provide weak aliases for each Exception handler to defaultHandler.
 * As they are weak aliases, any function with the same name will override
 * this definition.
 */
#pragma weak MMI_Handler = defaultHandler
#pragma weak MemManage_Handler = defaultHandler
#pragma weak BusFault_Handler = defaultHandler
#pragma weak UsageFault_Handler = defaultHandler
#pragma weak SVC_Handler = defaultHandler
#pragma weak DebugMon_Handler = defaultHandler
#pragma weak PendSV_Handler = defaultHandler
#pragma weak SysTick_Handler = defaultHandler
#pragma weak WWDG_IRQHandler = defaultHandler
#pragma weak PVD_IRQHandler = defaultHandler
#pragma weak TAMPER_IRQHandler = defaultHandler
#pragma weak RTC_IRQHandler = defaultHandler
#pragma weak FLASH_IRQHandler = defaultHandler
#pragma weak RCC_IRQHandler = defaultHandler
#pragma weak EXTI0_IRQHandler = defaultHandler
#pragma weak EXTI1_IRQHandler = defaultHandler
#pragma weak EXTI2_IRQHandler = defaultHandler
#pragma weak EXTI3_IRQHandler = defaultHandler
#pragma weak EXTI4_IRQHandler = defaultHandler
#pragma weak DMA1_Channel1_IRQHandler = defaultHandler
#pragma weak DMA1_Channel2_IRQHandler = defaultHandler
#pragma weak DMA1_Channel3_IRQHandler = defaultHandler
#pragma weak DMA1_Channel4_IRQHandler = defaultHandler
#pragma weak DMA1_Channel5_IRQHandler = defaultHandler
#pragma weak DMA1_Channel6_IRQHandler = defaultHandler
#pragma weak DMA1_Channel7_IRQHandler = defaultHandler
#pragma weak ADC1_2_IRQHandler = defaultHandler
#pragma weak USB_HP_CAN1_TX_IRQHandler = defaultHandler
#pragma weak USB_LP_CAN1_RX0_IRQHandler = defaultHandler
#pragma weak CAN1_RX1_IRQHandler = defaultHandler
#pragma weak CAN1_SCE_IRQHandler = defaultHandler
#pragma weak EXTI9_5_IRQHandler = defaultHandler
#pragma weak TIM1_BRK_IRQHandler = defaultHandler
#pragma weak TIM1_UP_IRQHandler = defaultHandler
#pragma weak TIM1_TRG_COM_IRQHandler = defaultHandler
#pragma weak TIM1_CC_IRQHandler = defaultHandler
#pragma weak TIM2_IRQHandler = defaultHandler
#pragma weak TIM3_IRQHandler = defaultHandler
#pragma weak TIM4_IRQHandler = defaultHandler
#pragma weak I2C1_EV_IRQHandler = defaultHandler
#pragma weak I2C1_ER_IRQHandler = defaultHandler
#pragma weak I2C2_EV_IRQHandler = defaultHandler
#pragma weak I2C2_ER_IRQHandler = defaultHandler
#pragma weak SPI1_IRQHandler = defaultHandler
#pragma weak SPI2_IRQHandler = defaultHandler
#pragma weak USART1_IRQHandler = defaultHandler
#pragma weak USART2_IRQHandler = defaultHandler
#pragma weak USART3_IRQHandler = defaultHandler
#pragma weak EXTI15_10_IRQHandler = defaultHandler
#pragma weak RTCAlarm_IRQHandler = defaultHandler
#pragma weak USBWakeUp_IRQHandler = defaultHandler

// ----------------------------------------------------------------------------
// Interrupt vectors
typedef void (* const FunctionPointer)(void);

// defined in the linkerscript
extern uint32_t __stack_end;

#if CORTEX_VECTORS_RAM
// Define the vector table
FunctionPointer flashVectors[4] 
__attribute__ ((section(".reset"))) =
{
	(FunctionPointer) &__stack_end,			// stack pointer
	resetHandler,			// code entry point
	nmiHandler,			// NMI handler
	hardfaultHandler		// hard fault handler
};

FunctionPointer ramVectors[] __attribute__ ((section(".vectors"))) =
#else
FunctionPointer flashVectors[] __attribute__ ((section(".reset"))) =
#endif
{
	(FunctionPointer) &__stack_end,			// stack pointer
	resetHandler,				// code entry point
	nmiHandler,					// NMI handler
	hardfaultHandler,			// hard fault handler
	MemManage_Handler,          /* MPU Fault Handler */
    BusFault_Handler,           /* Bus Fault Handler */
    UsageFault_Handler,         /* Usage Fault Handler */
    0,                          /* Reserved */
    0,                          /* Reserved */
    0,                          /* Reserved */
    0,                          /* Reserved */
    SVC_Handler,                /* SVCall Handler */
    DebugMon_Handler,           /* Debug Monitor Handler */
    0,                          /* Reserved */
    PendSV_Handler,             /* PendSV Handler */
    SysTick_Handler,            /* SysTick Handler */

    /* External Interrupts */
    WWDG_IRQHandler,            /* Window Watchdog */
    PVD_IRQHandler,             /* PVD through EXTI Line detect */
    TAMPER_IRQHandler,          /* Tamper */
    RTC_IRQHandler,             /* RTC */
    FLASH_IRQHandler,           /* Flash */
    RCC_IRQHandler,             /* RCC */
    EXTI0_IRQHandler,           /* EXTI Line 0 */
    EXTI1_IRQHandler,           /* EXTI Line 1 */
    EXTI2_IRQHandler,           /* EXTI Line 2 */
    EXTI3_IRQHandler,           /* EXTI Line 3 */
    EXTI4_IRQHandler,           /* EXTI Line 4 */
    DMA1_Channel1_IRQHandler,   /* DMA1 Channel 1 */
    DMA1_Channel2_IRQHandler,   /* DMA1 Channel 2 */
    DMA1_Channel3_IRQHandler,   /* DMA1 Channel 3 */
    DMA1_Channel4_IRQHandler,   /* DMA1 Channel 4 */
    DMA1_Channel5_IRQHandler,   /* DMA1 Channel 5 */
    DMA1_Channel6_IRQHandler,   /* DMA1 Channel 6 */
    DMA1_Channel7_IRQHandler,   /* DMA1 Channel 7 */
    ADC1_2_IRQHandler,          /* ADC1 & ADC2 */
    USB_HP_CAN1_TX_IRQHandler,  /* USB High Priority or CAN1 TX */
    USB_LP_CAN1_RX0_IRQHandler, /* USB Low  Priority or CAN1 RX0 */
    CAN1_RX1_IRQHandler,        /* CAN1 RX1 */
    CAN1_SCE_IRQHandler,        /* CAN1 SCE */
    EXTI9_5_IRQHandler,         /* EXTI Line 9..5 */
    TIM1_BRK_IRQHandler,        /* TIM1 Break */
    TIM1_UP_IRQHandler,         /* TIM1 Update */
    TIM1_TRG_COM_IRQHandler,    /* TIM1 Trigger and Commutation */
    TIM1_CC_IRQHandler,         /* TIM1 Capture Compare */
    TIM2_IRQHandler,            /* TIM2 */
    TIM3_IRQHandler,            /* TIM3 */
    TIM4_IRQHandler,            /* TIM4 */
    I2C1_EV_IRQHandler,         /* I2C1 Event */
    I2C1_ER_IRQHandler,         /* I2C1 Error */
    I2C2_EV_IRQHandler,         /* I2C2 Event */
    I2C2_ER_IRQHandler,         /* I2C2 Error */
    SPI1_IRQHandler,            /* SPI1 */
    SPI2_IRQHandler,            /* SPI2 */
    USART1_IRQHandler,          /* USART1 */
    USART2_IRQHandler,          /* USART2 */
    USART3_IRQHandler,          /* USART3 */
    EXTI15_10_IRQHandler,       /* EXTI Line 15..10 */
    RTCAlarm_IRQHandler,        /* RTC Alarm through EXTI Line */
    USBWakeUp_IRQHandler,       /* USB Wakeup from suspend */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (FunctionPointer) 0xF108F85F	/* TODO: @0x108. This is for boot in RAM mode for
										STM32F10x Medium Density devices. */
};

// ----------------------------------------------------------------------------
// defined in the linkerscript
extern uint32_t __fastcode_load;
extern uint32_t __fastcode_start;
extern uint32_t __fastcode_end;

extern uint32_t __data_load;
extern uint32_t __data_start;
extern uint32_t __data_end;

extern uint32_t __bss_start;
extern uint32_t __bss_end;

// Application's main function
int
main(void);

// calls CTORS of static objects
void
__libc_init_array(void);

// ----------------------------------------------------------------------------
void
resetHandler(void)
{
	// startup delay
	for (volatile unsigned long i = 0; i < 500000; i++)
	{
	}
	
	// Copy functions to RAM (.fastcode)
	uint32_t* src = &__fastcode_load;
	uint32_t* dest = &__fastcode_start;
	while (dest < &__fastcode_end)
	{
		*(dest++) = *(src++);
	}
	
	// Copy the data segment initializers from flash to RAM (.data)
	src = &__data_load;
	dest = &__data_start;
	while (dest < &__data_end)
	{
		*(dest++) = *(src++);
	}
	
	// Fill the bss segment with zero (.bss)
	dest = &__bss_start;
	while (dest < &__bss_end)
	{
		*(dest++) = 0;
	}
	
	// Call CTORS of static objects
	__libc_init_array();
	
	// TODO only for STM32
	//SystemInit();
	
	// Call the application's entry point
	main();
	
	while (1)
	{
	}
}

// ----------------------------------------------------------------------------
/**
 * @brief	Default interrupt handler
 * 
 * This functions gets called if an interrupt handler is not defines. It just
 * enters an infinite loop leaving the processor state intact for an debugger
 * to be examined.
*/
void
defaultHandler(void)
{
	while (1)
	{
	}
}
