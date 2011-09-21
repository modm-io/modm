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

#include <libmaple/flash.h>
#include <libmaple/rcc.h>
#include <libmaple/nvic.h>
#include <libmaple/scb.h>
#include <libmaple/gpio.h>
#include <libmaple/systick.h>

#define SYSTICK_RELOAD_VAL		71999

// ----------------------------------------------------------------------------
/*
 * Provide weak aliases for each Exception handler to defaultHandler.
 * As they are weak aliases, any function with the same name will override
 * this definition.
 */
void Reset_Handler(void);
void NMI_Handler(void)				__attribute__ ((weak, alias("defaultHandler")));
void HardFault_Handler(void);//		__attribute__ ((weak, alias("defaultHandler")));
void MemManage_Handler(void)		__attribute__ ((weak, alias("defaultHandler")));
void BusFault_Handler(void)			__attribute__ ((weak, alias("defaultHandler")));
void UsageFault_Handler(void)		__attribute__ ((weak, alias("defaultHandler")));
void SVC_Handler(void)				__attribute__ ((weak, alias("defaultHandler")));
void DebugMon_Handler(void)			__attribute__ ((weak, alias("defaultHandler")));
void PendSV_Handler(void)			__attribute__ ((weak, alias("defaultHandler")));
void SysTick_Handler(void)			__attribute__ ((weak, alias("defaultHandler")));

void WWDG_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void PVD_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void TAMPER_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void RTC_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void FLASH_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void RCC_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void EXTI0_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void EXTI1_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void EXTI2_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void EXTI3_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void EXTI4_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void DMA1_Channel1_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void DMA1_Channel2_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void DMA1_Channel3_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void DMA1_Channel4_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void DMA1_Channel5_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void DMA1_Channel6_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void DMA1_Channel7_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void ADC1_2_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void USB_HP_CAN1_TX_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void USB_LP_CAN1_RX0_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
void CAN1_RX1_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void CAN1_SCE_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void EXTI9_5_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
#if defined (STM32F10X_XL)
	void TIM1_BRK_TIM9_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
	void TIM1_UP_TIM10_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
	void TIM1_TRG_COM_TIM11_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
#else
	void TIM1_BRK_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
	void TIM1_UP_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
	void TIM1_TRG_COM_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
#endif
void TIM1_CC_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void TIM2_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void TIM3_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void TIM4_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void I2C1_EV_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void I2C1_ER_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void I2C2_EV_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void I2C2_ER_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void SPI1_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void SPI2_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
void USART1_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void USART2_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void USART3_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void EXTI15_10_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
void RTCAlarm_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
#if defined (STM32F10X_CL)
	void OTG_FS_WKUP_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
	void TIM5_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
	void SPI3_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
	void UART4_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
	void UART5_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
	void TIM6_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
	void TIM7_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
	void DMA2_Channel1_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
	void DMA2_Channel2_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
	void DMA2_Channel3_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
	void DMA2_Channel4_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
	void DMA2_Channel5_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
	void ETH_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
	void ETH_WKUP_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
	void CAN2_TX_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
	void CAN2_RX0_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
	void CAN2_RX1_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
	void CAN2_SCE_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
	void OTG_FS_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
#else
	void USBWakeUp_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
	
	#if defined (STM32F10X_HD) || defined (STM32F10X_XL)
		void TIM8_BRK_TIM12_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void TIM8_UP_TIM13_IRQHandler(void)			__attribute__ ((weak, alias("defaultHandler")));
		void TIM8_TRG_COM_TIM14_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
		void TIM8_CC_IRQHandler(void)				__attribute__ ((weak, alias("defaultHandler")));
		void ADC3_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void FSMC_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void SDIO_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void TIM5_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void SPI3_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void UART4_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void UART5_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void TIM6_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void TIM7_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void DMA2_Channel1_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void DMA2_Channel2_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void DMA2_Channel3_IRQHandler(void)		__attribute__ ((weak, alias("defaultHandler")));
		void DMA2_Channel4_5_IRQHandler(void)	__attribute__ ((weak, alias("defaultHandler")));
	#endif
#endif

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
	DebugMon_Handler,			// Debug Monitor Handler
	0,
	PendSV_Handler,				// PendSV Handler
	SysTick_Handler,			// SysTick Handler
	
	// Peripheral interrupts (STM32 specific)
	WWDG_IRQHandler,			// Window Watchdog
	PVD_IRQHandler,				// PVD through EXTI Line detect
	TAMPER_IRQHandler,			// Tamper
	RTC_IRQHandler,				// RTC
	FLASH_IRQHandler,			// Flash
	RCC_IRQHandler,				// RCC
	EXTI0_IRQHandler,			// EXTI Line 0
	EXTI1_IRQHandler,			// EXTI Line 1
	EXTI2_IRQHandler,			// EXTI Line 2
	EXTI3_IRQHandler,			// EXTI Line 3
	EXTI4_IRQHandler,			// EXTI Line 4
	DMA1_Channel1_IRQHandler,	// DMA1 Channel 1
	DMA1_Channel2_IRQHandler,	// DMA1 Channel 2
	DMA1_Channel3_IRQHandler,	// DMA1 Channel 3
	DMA1_Channel4_IRQHandler,	// DMA1 Channel 4
	DMA1_Channel5_IRQHandler,	// DMA1 Channel 5
	DMA1_Channel6_IRQHandler,	// DMA1 Channel 6
	DMA1_Channel7_IRQHandler,	// DMA1 Channel 7
	ADC1_2_IRQHandler,			// ADC1 & ADC2
	USB_HP_CAN1_TX_IRQHandler,  // USB High Priority or CAN1 TX
	USB_LP_CAN1_RX0_IRQHandler, // USB Low  Priority or CAN1 RX0
	CAN1_RX1_IRQHandler,		// CAN1 RX1
	CAN1_SCE_IRQHandler,		// CAN1 SCE
	EXTI9_5_IRQHandler,			// EXTI Line 9..5
#if defined (STM32F10X_XL)
	TIM1_BRK_TIM9_IRQHandler,	// TIM1 Break
	TIM1_UP_TIM10_IRQHandler,	// TIM1 Update
	TIM1_TRG_COM_TIM11_IRQHandler,	// TIM1 Trigger and Commutation
#else
	TIM1_BRK_IRQHandler,		// TIM1 Break
	TIM1_UP_IRQHandler,			// TIM1 Update
	TIM1_TRG_COM_IRQHandler,	// TIM1 Trigger and Commutation
#endif
	TIM1_CC_IRQHandler,			// TIM1 Capture Compare
	TIM2_IRQHandler,			// TIM2
	TIM3_IRQHandler,			// TIM3
	TIM4_IRQHandler,			// TIM4
	I2C1_EV_IRQHandler,			// I2C1 Event
	I2C1_ER_IRQHandler,			// I2C1 Error
	I2C2_EV_IRQHandler,			// I2C2 Event
	I2C2_ER_IRQHandler,			// I2C2 Error
	SPI1_IRQHandler,			// SPI1
	SPI2_IRQHandler,			// SPI2
	USART1_IRQHandler,			// USART1
	USART2_IRQHandler,			// USART2
	USART3_IRQHandler,			// USART3
	EXTI15_10_IRQHandler,		// EXTI Line 15..10
	RTCAlarm_IRQHandler,		// RTC Alarm through EXTI Line
#if defined(STM32F10X_CL)
	OTG_FS_WKUP_IRQHandler,
#else
	USBWakeUp_IRQHandler,		// USB Wakeup from suspend
#endif
#if defined (STM32F10X_HD) || defined (STM32F10X_XL) || defined(STM32F10X_CL)
	#if defined(STM32F10X_CL)
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	#else
		TIM8_BRK_TIM12_IRQHandler,
		TIM8_UP_TIM13_IRQHandler,
		TIM8_TRG_COM_TIM14_IRQHandler,
		TIM8_CC_IRQHandler,
		ADC3_IRQHandler,
		FSMC_IRQHandler,
		SDIO_IRQHandler,
	#endif
	TIM5_IRQHandler,
	SPI3_IRQHandler,
	UART4_IRQHandler,
	UART5_IRQHandler,
	TIM6_IRQHandler,
	TIM7_IRQHandler,
	DMA2_Channel1_IRQHandler,
	DMA2_Channel2_IRQHandler,
	DMA2_Channel3_IRQHandler,
	#if defined(STM32F10X_CL)
		DMA2_Channel4_IRQHandler,
		DMA2_Channel5_IRQHandler,
		ETH_IRQHandler,
		ETH_WKUP_IRQHandler,
		CAN2_TX_IRQHandler,
		CAN2_RX0_IRQHandler,
		CAN2_RX1_IRQHandler,
		CAN2_SCE_IRQHandler,
		OTG_FS_IRQHandler
	#else
		DMA2_Channel4_5_IRQHandler,
	#endif
#endif
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

extern void
exit(int) __attribute__ ((noreturn, weak));

// TODO
#define SCB_SHCSR_USGFAULTENA_Pos          18                                             /*!< SCB SHCSR: USGFAULTENA Position */
#define SCB_SHCSR_USGFAULTENA_Msk          (1ul << SCB_SHCSR_USGFAULTENA_Pos)             /*!< SCB SHCSR: USGFAULTENA Mask */

#define SCB_SHCSR_BUSFAULTENA_Pos          17                                             /*!< SCB SHCSR: BUSFAULTENA Position */
#define SCB_SHCSR_BUSFAULTENA_Msk          (1ul << SCB_SHCSR_BUSFAULTENA_Pos)             /*!< SCB SHCSR: BUSFAULTENA Mask */

#define SCB_SHCSR_MEMFAULTENA_Pos          16                                             /*!< SCB SHCSR: MEMFAULTENA Position */
#define SCB_SHCSR_MEMFAULTENA_Msk          (1ul << SCB_SHCSR_MEMFAULTENA_Pos)             /*!< SCB SHCSR: MEMFAULTENA Mask */


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
	
	// setup flash
	flash_enable_prefetch();
	flash_set_latency(FLASH_WAIT_STATE_2);
	
	// setup clocks
	rcc_clk_init(RCC_CLKSRC_PLL, RCC_PLLSRC_HSE, RCC_PLLMUL_9);
	rcc_set_prescaler(RCC_PRESCALER_AHB, RCC_AHB_SYSCLK_DIV_1);
	rcc_set_prescaler(RCC_PRESCALER_APB1, RCC_APB1_HCLK_DIV_2);
	rcc_set_prescaler(RCC_PRESCALER_APB2, RCC_APB2_HCLK_DIV_1);
	
	rcc_clk_enable(RCC_GPIOA);
	rcc_clk_enable(RCC_GPIOB);
	rcc_clk_enable(RCC_GPIOC);
	
	// setup NVIC
	nvic_init(0x08000000, 0);
	
	systick_init(SYSTICK_RELOAD_VAL);
	
	gpio_init_all();
	afio_init();
	
	// enable fault handlers
	/*SCB_BASE->SHCSR |= 
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
