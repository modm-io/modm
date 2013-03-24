// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
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
#include <xpcc_config.hpp>

#include <lpc11xx/cmsis/LPC11xx.h>

#define ALIAS(f)	__attribute__ ((weak, alias (#f)))

// Forward declaration of the default handlers. These are aliased.
// When the application defines a handler (with the same name), this will 
// automatically take precedence over these weak definitions
void Reset_Handler(void);
void NMI_Handler(void)			ALIAS(defaultHandler);
void HardFault_Handler(void)	ALIAS(defaultHandler);
void MemManage_Handler(void)	ALIAS(defaultHandler);
void BusFault_Handler(void)		ALIAS(defaultHandler);
void UsageFault_Handler(void)	ALIAS(defaultHandler);
void SVCall_Handler(void)		ALIAS(defaultHandler);
void DebugMon_Handler(void)		ALIAS(defaultHandler);
void PendSV_Handler(void)		ALIAS(defaultHandler);
void SysTick_Handler(void)		ALIAS(defaultHandler);

// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
void WDT_IRQHandler(void)		ALIAS(defaultHandler);
void TIMER0_IRQHandler(void)	ALIAS(defaultHandler);
void TIMER1_IRQHandler(void)	ALIAS(defaultHandler);
void TIMER2_IRQHandler(void)	ALIAS(defaultHandler);
void TIMER3_IRQHandler(void)	ALIAS(defaultHandler);
void UART0_IRQHandler(void)		ALIAS(defaultHandler);
void UART1_IRQHandler(void)		ALIAS(defaultHandler);
void UART2_IRQHandler(void)		ALIAS(defaultHandler);
void UART3_IRQHandler(void)		ALIAS(defaultHandler);
void PWM1_IRQHandler(void)		ALIAS(defaultHandler);
void I2C0_IRQHandler(void)		ALIAS(defaultHandler);
void I2C1_IRQHandler(void)		ALIAS(defaultHandler);
void I2C2_IRQHandler(void)		ALIAS(defaultHandler);
void SPI_IRQHandler(void)		ALIAS(defaultHandler);
void SSP0_IRQHandler(void)		ALIAS(defaultHandler);
void SSP1_IRQHandler(void)		ALIAS(defaultHandler);
void PLL0_IRQHandler(void)		ALIAS(defaultHandler);
void RTC_IRQHandler(void)		ALIAS(defaultHandler);
void EINT0_IRQHandler(void)		ALIAS(defaultHandler);
void EINT1_IRQHandler(void)		ALIAS(defaultHandler);
void EINT2_IRQHandler(void)		ALIAS(defaultHandler);
void EINT3_IRQHandler(void)		ALIAS(defaultHandler);
void ADC_IRQHandler(void)		ALIAS(defaultHandler);
void BOD_IRQHandler(void)		ALIAS(defaultHandler);
void USB_IRQHandler(void)		ALIAS(defaultHandler);
void CAN_IRQHandler(void)		ALIAS(defaultHandler);
void DMA_IRQHandler(void)		ALIAS(defaultHandler);
void I2S_IRQHandler(void)		ALIAS(defaultHandler);
void ENET_IRQHandler(void)		ALIAS(defaultHandler);
void RIT_IRQHandler(void)		ALIAS(defaultHandler);
void MCPWM_IRQHandler(void)		ALIAS(defaultHandler);
void QEI_IRQHandler(void)		ALIAS(defaultHandler);
void PLL1_IRQHandler(void)		ALIAS(defaultHandler);
void USBActivity_IRQHandler(void)		ALIAS(defaultHandler);
void CANActivity_IRQHandler(void)		ALIAS(defaultHandler);

// ----------------------------------------------------------------------------
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x00000000.
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
	(FunctionPointer) &__stack_end,		    // The initial stack pointer
    Reset_Handler,                               // The reset handler
    NMI_Handler,							// The NMI handler
	HardFault_Handler,						// The hard fault handler
	MemManage_Handler,						// The MPU fault handler
	BusFault_Handler,						// The bus fault handler
	UsageFault_Handler,						// The usage fault handler
	0,										// Reserved
	0,										// Reserved
	0,										// Reserved
	0,										// Reserved
	SVCall_Handler,							// SVCall handler
	DebugMon_Handler,						// Debug monitor handler
	0,										// Reserved
	PendSV_Handler,							// The PendSV handler
	SysTick_Handler,						// The SysTick handler

	// Chip Level - LPC17
	WDT_IRQHandler,							// 16, 0x40 - WDT
	TIMER0_IRQHandler,						// 17, 0x44 - TIMER0
	TIMER1_IRQHandler,						// 18, 0x48 - TIMER1
	TIMER2_IRQHandler,						// 19, 0x4c - TIMER2
	TIMER3_IRQHandler,						// 20, 0x50 - TIMER3
	UART0_IRQHandler,						// 21, 0x54 - UART0
	UART1_IRQHandler,						// 22, 0x58 - UART1
	UART2_IRQHandler,						// 23, 0x5c - UART2
	UART3_IRQHandler,						// 24, 0x60 - UART3
	PWM1_IRQHandler,						// 25, 0x64 - PWM1
	I2C0_IRQHandler,						// 26, 0x68 - I2C0
	I2C1_IRQHandler,						// 27, 0x6c - I2C1
	I2C2_IRQHandler,						// 28, 0x70 - I2C2
	SPI_IRQHandler,							// 29, 0x74 - SPI
	SSP0_IRQHandler,						// 30, 0x78 - SSP0
	SSP1_IRQHandler,						// 31, 0x7c - SSP1
	PLL0_IRQHandler,						// 32, 0x80 - PLL0 (Main PLL)
	RTC_IRQHandler,							// 33, 0x84 - RTC
	EINT0_IRQHandler,						// 34, 0x88 - EINT0
	EINT1_IRQHandler,						// 35, 0x8c - EINT1
	EINT2_IRQHandler,						// 36, 0x90 - EINT2
	EINT3_IRQHandler,						// 37, 0x94 - EINT3
	ADC_IRQHandler,							// 38, 0x98 - ADC
	BOD_IRQHandler,							// 39, 0x9c - BOD
	USB_IRQHandler,							// 40, 0xA0 - USB
	CAN_IRQHandler,							// 41, 0xa4 - CAN
	DMA_IRQHandler,							// 42, 0xa8 - GP DMA
	I2S_IRQHandler,							// 43, 0xac - I2S
	ENET_IRQHandler,						// 44, 0xb0 - Ethernet
	RIT_IRQHandler,							// 45, 0xb4 - RITINT
	MCPWM_IRQHandler,						// 46, 0xb8 - Motor Control PWM
	QEI_IRQHandler,							// 47, 0xbc - Quadrature Encoder
	PLL1_IRQHandler,						// 48, 0xc0 - PLL1 (USB PLL)
	USBActivity_IRQHandler,					// 49, 0xc4 - USB Activity interrupt to wakeup
	CANActivity_IRQHandler, 				// 50, 0xc8 - CAN Activity interrupt to wakeup
};

// ----------------------------------------------------------------------------
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
extern uint32_t __fastcode_load;
extern uint32_t __fastcode_start;
extern uint32_t __fastcode_end;

extern uint32_t __data_load;
extern uint32_t __data_start;
extern uint32_t __data_end;

extern uint32_t __bss_start;
extern uint32_t __bss_end;

// Application's main function
extern int main(void);
extern void __libc_init_array(void);
extern void exit(int) __attribute__ ((noreturn, weak));

// ----------------------------------------------------------------------------
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied main() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
void
Reset_Handler(void)
{
	// startup delay
	for (volatile unsigned long i = 0; i < 50000; i++)
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
	
	// Call C++ library initialization
	__libc_init_array();
	
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
