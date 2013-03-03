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
#else
	#error "This file is not supposed to be used with given CPU (only STM32F3xx)"
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
	Reset_Handler,				// 0000 0004 code entry point
	NMI_Handler,				// 0000 0008 NMI handler
	HardFault_Handler,			// 0000 000C hard fault handler
	MemManage_Handler,			// 0000 0010 MPU Fault Handler
	BusFault_Handler,			// 0000 0014 Bus Fault Handler
	UsageFault_Handler,			// 0000 0018 Usage Fault Handler
	0,							// 0000 001C
	0,							// 0000 0020
	0,							// 0000 0024
	0,							// 0000 0028
	SVC_Handler,				// 0000 002C SVCall Handler
	0,							// 0000 0030
	0,							// 0000 0034
	PendSV_Handler,				// 0000 0038 PendSV Handler
	SysTick_Handler,			// 0000 003C SysTick Handler
	
	// Peripheral interrupts (STM32 specific)
	WWDG_IRQHandler,			// 0000 0040: Window Watchdog
	PVD_IRQHandler,				// 0000 0044 1: PVD through EXTI Line detect
	TAMP_STAMP_IRQHandler,		// 0000 0048 2: Tamper and TimeStamps through the EXTI line
	RTC_WKUP_IRQHandler,		// 0000 004C 3: Wakeup through the EXTI line
	FLASH_IRQHandler,			// 0000 0050 4: Flash
	RCC_IRQHandler,				// 0000 0054 5: RCC
	EXTI0_IRQHandler,			// 0000 0058    EXTI Line 0
	EXTI1_IRQHandler,			// 0000 005C    EXTI Line 1
	EXTI2_IRQHandler,			// 0000 0060    EXTI Line 2
	EXTI3_IRQHandler,			// 0000 0064    EXTI Line 3
	EXTI4_IRQHandler,			// 0000 0068	10: EXTI Line 4
	DMA1_CH1_IRQHandler,		// 0000 006C    	DMA1 Channel 1
	DMA1_CH1_IRQHandler,		// 0000 0070    	DMA1 Channel 2
	DMA1_CH2_IRQHandler,		// 0000 0074    	DMA1 Channel 3
	DMA1_CH3_IRQHandler,		// 0000 0078    	DMA1 Channel 4
	DMA1_CH4_IRQHandler,		// 0000 007C	15: DMA1 Channel 5
	DMA1_CH5_IRQHandler,		// 0000 0080    	DMA1 Channel 6
	DMA1_CH6_IRQHandler,		// 0000 0084    	DMA1 Channel 7
	ADC1_2_IRQHandler,			// 0000 0088    	ADC1 and ADC2
	CAN1_TX_IRQHandler,  		// 0000 008C    	CAN1 TX
	CAN1_RX0_IRQHandler, 		// 0000 0090	20: CAN1 RX0
	CAN1_RX1_IRQHandler,		// 0000 0094    	CAN1 RX1
	CAN1_SCE_IRQHandler,		// 0000 0098    	CAN1 SCE
	EXTI9_5_IRQHandler,			// 0000 009C    	EXTI Line 9..5
	TIM1_BRK_TIM15_IRQHandler,	// 0000 00A0    	TIM1 Break
	TIM1_UP_TIM16_IRQHandler,	// 0000 00A4	25: TIM1 Update
	TIM1_TRG_COM_TIM17_IRQHandler,	// 0000 00A8	TIM1 Trigger and Commutation
	TIM1_CC_IRQHandler,			// 0000 00AC    	TIM1 Capture Compare
	TIM2_IRQHandler,			// 0000 00B0    	TIM2
	TIM3_IRQHandler,			// 0000 00B4    	TIM3
	TIM4_IRQHandler,			// 0000 00B8	30: TIM4
	I2C1_EV_EXTI23_IRQHandler,	// 0000 00BC	    I2C1 Event and EXTI Line23
	I2C1_ER_IRQHandler,			// 0000 00C0	    I2C1 Error
	I2C2_EV_EXTI24_IRQHandler,	// 0000 00C4	    I2C2 Event and EXTI Line24
	I2C2_ER_IRQHandler,			// 0000 00C8	    I2C2 Error
	SPI1_IRQHandler,			// 0000 00CC	35: SPI1
	SPI2_IRQHandler,			// 0000 00D0	    SPI2
	USART1_IRQHandler,			// 0000 00D4	    USART1
	USART2_IRQHandler,			// 0000 00D8	    USART2
	USART3_IRQHandler,			// 0000 00DC	    USART3
	EXTI15_10_IRQHandler,		// 0000 00E0	40: External Line[15:10]s
	RTCAlarm_IRQHandler,		// 0000 00E4	    RTC Alarm (A and B) through EXTI Line
	USB_WKUP_IRQHandler,		// 0000 00E8	    USB Wakeup through EXTI line
	TIM8_BRK_IRQHandler,		// 0000 00EC	    TIM8 Break
	TIM8_UP_IRQHandler,			// 0000 00F0	    TIM8 Update and TIM13
	TIM8_TRG_COM_IRQHandler,	// 0000 00F4	45: TIM8 Trigger and Commutation
	TIM8_CC_IRQHandler,			// 0000 00F8	    TIM8 Capture compare
	ADC3_IRQHandler,			// 0000 00FC	    ADC3
	0,							// 0000 0100
	0,							// 0000 0104
	0,							// 0000 0108	50:
	SPI3_IRQHandler,			// 0000 010C
	UART4_IRQHandler,			// 0000 0110
	UART5_IRQHandler,			// 0000 0114
	TIM6_DAC_IRQHandler,		// 0000 0118    	TIM6 and DAC1&2 under-run errors
	TIM7_IRQHandler,			// 0000 011C	55:
	DMA2_CH1_IRQHandler,		// 0000 0120
	DMA2_CH2_IRQHandler,		// 0000 0124
	DMA2_CH3_IRQHandler,		// 0000 0128
	DMA2_CH4_IRQHandler,		// 0000 012C
	DMA2_CH5_IRQHandler,		// 0000 0130	60:
	ADC4_IRQHandler,			// 0000 0134
	0,							// 0000 0138
	0,							// 0000 013C
	COMP123_IRQHandler,			// 0000 0140
	COMP456_IRQHandler,			// 0000 0144	65:
	COMP7_IRQHandler,			// 0000 0148
	0,							// 0000 014C
	0,							// 0000 0150
	0,							// 0000 0154
	0,							// 0000 0158	70:
	0,							// 0000 015C
	0,							// 0000 0160
	0,							// 0000 0164
	USB_HP_IRQHandler,			// 0000 0168	     USB High priority
	USB_LP_IRQHandler,			// 0000 016C	 75: USB Low prioroty
	USB_WKUP_IRQHandler,		// 0000 0170	     USB Wakeup through EXTI
	0,							// 0000 0174
	0,							// 0000 0178
	0,							// 0000 017C
	0,							// 0000 0180	 80:
	FPU_IRQHandler,				// 0000 0184	     FPU
};

// FIXME set USB_IT_RMP bit in the Section 9.1.1: SYSCFG configuration register 1 (SYSCFG_CFGR1)

// ----------------------------------------------------------------------------
// defined in the linker script
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
	
	// prepare flash latency for working at 72MHz and supply voltage > 2.7
	FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_1;
	
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
