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
 *
 * $hello please remove me$
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
void SVCall_Handler(void)		ALIAS(defaultHandler);
void PendSV_Handler(void)		ALIAS(defaultHandler);
void SysTick_Handler(void)		ALIAS(defaultHandler);

// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
void CAN_IRQHandler(void)		ALIAS(defaultHandler);
void SSP1_IRQHandler(void)		ALIAS(defaultHandler);
void I2C_IRQHandler(void)		ALIAS(defaultHandler);
void TIMER16_0_IRQHandler(void)	ALIAS(defaultHandler);
void TIMER16_1_IRQHandler(void)	ALIAS(defaultHandler);
void TIMER32_0_IRQHandler(void)	ALIAS(defaultHandler);
void TIMER32_1_IRQHandler(void)	ALIAS(defaultHandler);
void SSP0_IRQHandler(void)		ALIAS(defaultHandler);
void UART_IRQHandler(void)		ALIAS(defaultHandler);
void ADC_IRQHandler(void)		ALIAS(defaultHandler);
void WDT_IRQHandler(void)		ALIAS(defaultHandler);
void BOD_IRQHandler(void)		ALIAS(defaultHandler);
void PIOINT3_IRQHandler(void)	ALIAS(defaultHandler);
void PIOINT2_IRQHandler(void)	ALIAS(defaultHandler);
void PIOINT1_IRQHandler(void)	ALIAS(defaultHandler);
void PIOINT0_IRQHandler(void)	ALIAS(defaultHandler);
void WAKEUP_IRQHandler(void)	ALIAS(defaultHandler);

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
    NMI_Handler,                            // The NMI handler
    HardFault_Handler,                      // The hard fault handler
    0,                      				// Reserved
    0,                      				// Reserved
    0,                      				// Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    SVCall_Handler,                      	// SVCall handler
    0,                      				// Reserved
    0,                                      // Reserved
    PendSV_Handler,                      	// The PendSV handler
    SysTick_Handler,                      	// The SysTick handler

    // Wakeup sources for the I/O pins:
    //   PIO0 (0:11)
    //   PIO1 (0)
    WAKEUP_IRQHandler,                      // PIO0_0  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_1  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_2  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_3  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_4  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_5  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_6  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_7  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_8  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_9  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_10 Wakeup
    WAKEUP_IRQHandler,                      // PIO0_11 Wakeup
    WAKEUP_IRQHandler,                      // PIO1_0  Wakeup
    
    CAN_IRQHandler,							// C_CAN Interrupt
    SSP1_IRQHandler, 						// SPI/SSP1 Interrupt
    I2C_IRQHandler,                      	// I2C0
    TIMER16_0_IRQHandler,                   // CT16B0 (16-bit Timer 0)
    TIMER16_1_IRQHandler,                   // CT16B1 (16-bit Timer 1)
    TIMER32_0_IRQHandler,                   // CT32B0 (32-bit Timer 0)
    TIMER32_1_IRQHandler,                   // CT32B1 (32-bit Timer 1)
    SSP0_IRQHandler,                      	// SPI/SSP0 Interrupt
    UART_IRQHandler,                      	// UART0

    0, 				                     	// Reserved
    0,                      				// Reserved

    ADC_IRQHandler,                      	// ADC   (A/D Converter)
    WDT_IRQHandler,                      	// WDT   (Watchdog Timer)
    BOD_IRQHandler,                      	// BOD   (Brownout Detect)
    0,                      				// Reserved
    PIOINT3_IRQHandler,                     // PIO INT3
    PIOINT2_IRQHandler,                     // PIO INT2
    PIOINT1_IRQHandler,                     // PIO INT1
    PIOINT0_IRQHandler,                     // PIO INT0
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
extern void __xpcc_initialize_memory(void);
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
	
	// Enable AHB clock to the GPIO domain.
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6);
	
	__xpcc_initialize_memory();
	
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

