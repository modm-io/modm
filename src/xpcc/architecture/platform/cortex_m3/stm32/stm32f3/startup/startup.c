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
 */
// ----------------------------------------------------------------------------

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>

#if defined(STM32F30X)
#	include <stm32f30x.h>
#endif

#include "xpcc_config.hpp"

// ----------------------------------------------------------------------------
#define FLASH_WAIT_STATE_0		0x0
#define FLASH_WAIT_STATE_1		0x1
#define FLASH_WAIT_STATE_2		0x2
#define FLASH_WAIT_STATE_3		0x3
#define FLASH_WAIT_STATE_4		0x4
#define FLASH_WAIT_STATE_5		0x5
#define FLASH_WAIT_STATE_6		0x6
#define FLASH_WAIT_STATE_7		0x7

#define NR_INTERRUPTS 82

// ----------------------------------------------------------------------------
/*
 * Provide weak aliases for each Exception handler to defaultHandler.
 * As they are weak aliases, any function with the same name will override
 * this definition.
 */
void Reset_Handler(void);
void NMI_Handler(void)				__attribute__ ((weak, alias("defaultHandler")));
void HardFault_Handler(void);
void MemManage_Handler(void)		__attribute__ ((weak, alias("defaultHandler")));
void BusFault_Handler(void)			__attribute__ ((weak, alias("defaultHandler")));
void UsageFault_Handler(void)		__attribute__ ((weak, alias("defaultHandler")));
void SVC_Handler(void)				__attribute__ ((weak, alias("defaultHandler")));
void PendSV_Handler(void)			__attribute__ ((weak, alias("defaultHandler")));
void SysTick_Handler(void)			__attribute__ ((weak, alias("defaultHandler")));

void WWDG_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void PVD_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void TAMP_STAMP_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void RTC_WKUP_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void FLASH_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void RCC_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void EXTI0_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void EXTI1_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void EXTI2_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void EXTI3_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void EXTI4_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void DMA1_CH1_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void DMA1_CH2_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void DMA1_CH3_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void DMA1_CH4_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void DMA1_CH5_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void DMA1_CH6_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void DMA1_CH7_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void ADC1_2_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void CAN1_TX_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void CAN1_RX0_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void CAN1_RX1_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void CAN1_SCE_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void EXTI9_5_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void TIM1_BRK_TIM15_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void TIM1_UP_TIM16_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void TIM1_TRG_COM_TIM17_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void TIM1_CC_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void TIM2_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void TIM3_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void TIM4_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void I2C1_EV_EXTI23_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void I2C1_ER_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void I2C2_EV_EXTI24_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void I2C2_ER_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void SPI1_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void SPI2_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void USART1_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void USART2_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void USART3_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void EXTI15_10_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void RTCAlarm_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void TIM8_BRK_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void TIM8_UP_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void TIM8_TRG_COM_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void TIM8_CC_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void ADC3_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void SPI3_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void UART4_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void UART5_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void TIM6_DAC_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void TIM7_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void DMA2_CH1_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void DMA2_CH2_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void DMA2_CH3_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void DMA2_CH4_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void DMA2_CH5_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void ADC4_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void COMP123_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void COMP456_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void COMP7_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void USB_HP_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void USB_LP_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void USB_WKUP_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void FPU_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));

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
	(FunctionPointer) &__stack_end,	// stack pointer
	Reset_Handler,				// code entry point
	NMI_Handler,				// NMI handler
	HardFault_Handler,			// hard fault handler
};

FunctionPointer ramVectors[] __attribute__ ((section(".vectors"))) =
#else
FunctionPointer flashVectors[] __attribute__ ((section(".reset"))) =
#endif
{
	(FunctionPointer) &__stack_end,	// stack pointer
	Reset_Handler,				// code entry point
	NMI_Handler,				// NMI handler
	HardFault_Handler,			// hard fault handler
	MemManage_Handler,			// MPU Fault Handler
	BusFault_Handler,			// Bus Fault Handler
	UsageFault_Handler,			// Usage Fault Handler
	0,
	0,
	0,
	0,
	SVC_Handler,				// SVCall Handler
	0,
	0,
	PendSV_Handler,				// PendSV Handler
	SysTick_Handler,			// SysTick Handler
	
	// Peripheral interrupts (STM32 specific)
	WWDG_IRQHandler,			//  0: Window Watchdog
	PVD_IRQHandler,				//  1: PVD through EXTI Line detect
	TAMP_STAMP_IRQHandler,		//  2: Tamper and TimeStamps through the EXTI line
	RTC_WKUP_IRQHandler,		//  3: Wakeup through the EXTI line
	FLASH_IRQHandler,			//  4: Flash
	RCC_IRQHandler,				//  5: RCC
	EXTI0_IRQHandler,			//     EXTI Line 0
	EXTI1_IRQHandler,			//     EXTI Line 1
	EXTI2_IRQHandler,			//     EXTI Line 2
	EXTI3_IRQHandler,			//     EXTI Line 3
	EXTI4_IRQHandler,			// 10: EXTI Line 4
	DMA1_CH1_IRQHandler,		//     DMA1 Channel 1
	DMA1_CH1_IRQHandler,		//     DMA1 Channel 2
	DMA1_CH2_IRQHandler,		//     DMA1 Channel 3
	DMA1_CH3_IRQHandler,		//     DMA1 Channel 4
	DMA1_CH4_IRQHandler,		// 15: DMA1 Channel 5
	DMA1_CH5_IRQHandler,		//     DMA1 Channel 6
	DMA1_CH6_IRQHandler,		//     DMA1 Channel 7
	ADC1_2_IRQHandler,			//     ADC1 and ADC2
	CAN1_TX_IRQHandler,  		//     CAN1 TX
	CAN1_RX0_IRQHandler, 		// 20: CAN1 RX0
	CAN1_RX1_IRQHandler,		//     CAN1 RX1
	CAN1_SCE_IRQHandler,		//     CAN1 SCE
	EXTI9_5_IRQHandler,			//     EXTI Line 9..5
	TIM1_BRK_TIM15_IRQHandler,	//     TIM1 Break
	TIM1_UP_TIM16_IRQHandler,	// 25: TIM1 Update
	TIM1_TRG_COM_TIM17_IRQHandler,	// TIM1 Trigger and Commutation
	TIM1_CC_IRQHandler,			//     TIM1 Capture Compare
	TIM2_IRQHandler,			//     TIM2
	TIM3_IRQHandler,			//     TIM3
	TIM4_IRQHandler,			// 30: TIM4
	I2C1_EV_EXTI23_IRQHandler,	//     I2C1 Event and EXTI Line23
	I2C1_ER_IRQHandler,			//     I2C1 Error
	I2C2_EV_EXTI24_IRQHandler,	//     I2C2 Event and EXTI Line24
	I2C2_ER_IRQHandler,			//     I2C2 Error
	SPI1_IRQHandler,			// 35: SPI1
	SPI2_IRQHandler,			//     SPI2
	USART1_IRQHandler,			//     USART1
	USART2_IRQHandler,			//     USART2
	USART3_IRQHandler,			//     USART3
	EXTI15_10_IRQHandler,		// 40: External Line[15:10]s
	RTCAlarm_IRQHandler,		//     RTC Alarm (A and B) through EXTI Line
	USB_WKUP_IRQHandler,		//     USB Wakeup through EXTI line
	TIM8_BRK_IRQHandler,		//     TIM8 Break
	TIM8_UP_IRQHandler,			//     TIM8 Update and TIM13
	TIM8_TRG_COM_IRQHandler,	// 45: TIM8 Trigger and Commutation
	TIM8_CC_IRQHandler,			//     TIM8 Capture compare
	ADC3_IRQHandler,			//     ADC3
	0,
	0,
	0,							// 50:
	SPI3_IRQHandler,
	UART4_IRQHandler,
	UART5_IRQHandler,
	TIM6_DAC_IRQHandler,		//     TIM6 and DAC1&2 under-run errors
	TIM7_IRQHandler,			// 55:
	DMA2_CH1_IRQHandler,
	DMA2_CH2_IRQHandler,
	DMA2_CH3_IRQHandler,
	DMA2_CH4_IRQHandler,
	DMA2_CH5_IRQHandler,		// 60:
	ADC4_IRQHandler,			//
	0,
	0,
	COMP123_IRQHandler,
	COMP456_IRQHandler,			// 65:
	COMP7_IRQHandler,
	0,
	0,							//
	0,							//
	0,							// 70:
	0,							//
	0,							//
	0,							//
	USB_HP_IRQHandler,			//     USB High priority
	USB_LP_IRQHandler,			// 75: USB Low prioroty
	USB_WKUP_IRQHandler,		//     USB Wakeup through EXTI
	0,							//
	0,							//
	0,							//
	0,							// 80:
	FPU_IRQHandler,				//     FPU
};

// FIXME set USB_IT_RMP bit in the Section 9.1.1: SYSCFG configuration register 1 (SYSCFG_CFGR1)

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

extern void
exit(int) __attribute__ ((noreturn, weak));

// ----------------------------------------------------------------------------
void
Reset_Handler(void)
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
	
#if defined(STM32F3XX)
	// prepare flash latency for working at 72MHz and supply voltage > 2.7
	FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_1;
#else
	#error "This file is not supposed to be used with given CPU (only STM32F3xx)"
#endif
	
	// enable flash prefetch
	FLASH->ACR |= FLASH_ACR_PRFTBE;

#if defined(STM32F3XX)
	// Enable FPU in privileged and user mode
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));  // set CP10 and CP11 Full Access
#endif
	
	// Enable GPIO clock
	// TODO adapt to actual pin count!
	// GPIOA-F
	RCC->AHBENR  |=
			  RCC_AHBENR_GPIOAEN
			| RCC_AHBENR_GPIOBEN
			| RCC_AHBENR_GPIOCEN
			| RCC_AHBENR_GPIODEN
			| RCC_AHBENR_GPIOEEN
			| RCC_AHBENR_GPIOFEN
			;
	RCC->AHBRSTR |=
			  RCC_AHBRSTR_GPIOARST
			| RCC_AHBRSTR_GPIOBRST
			| RCC_AHBRSTR_GPIOCRST
			| RCC_AHBRSTR_GPIODRST
			| RCC_AHBRSTR_GPIOFRST
			; // Reset value is 0, check if this can be left out.

	RCC->AHBRSTR &=
			~( RCC_AHBRSTR_GPIOARST
			 | RCC_AHBRSTR_GPIOBRST
			 | RCC_AHBRSTR_GPIOCRST
			 | RCC_AHBRSTR_GPIODRST
			 | RCC_AHBRSTR_GPIOFRST)
			;
	
	// Setup NVIC
	// Set vector table
	const uint32_t offset = 0;
	SCB->VTOR = 0x08000000 | (offset & 0x1FFFFF80);

	// Lower priority level for all peripheral interrupts to lowest possible
	for (uint32_t i = 0; i < NR_INTERRUPTS; i++) {
		const uint32_t priority = 0xF;
		NVIC->IP[i] = (priority & 0xF) << 4;
	}
	
	// Set the PRIGROUP[10:8] bits to
	// - 4 bits for pre-emption priority,
	// - 0 bits for subpriority
	SCB->AIRCR = 0x05FA0000 | 0x300;
	
	// Enable fault handlers
	/*SCB->SHCSR |=
			SCB_SHCSR_BUSFAULTENA_Msk |
			SCB_SHCSR_USGFAULTENA_Msk |
			SCB_SHCSR_MEMFAULTENA_Msk;*/
	
	// Call CTORS of static objects
	__libc_init_array();
	
	// Call the application's entry point
	main();
	
	exit(1);
	
	while (1)
	{
	}
}

// ----------------------------------------------------------------------------
/**
 * @brief	Default interrupt handler
 * 
 * This functions gets called if an interrupt handler is not defined. It just
 * enters an infinite loop leaving the processor state intact for a debugger
 * to be examined.
*/
void
defaultHandler(void)
{
	while (1)
	{
	}
}
