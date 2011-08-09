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
#include <libmaple/flash.h>
#include <libmaple/rcc.h>
#include <libmaple/nvic.h>
#include <libmaple/gpio.h>
#include <libmaple/systick.h>

#define SYSTICK_RELOAD_VAL		71999

// ----------------------------------------------------------------------------
void resetHandler(void);
void ATTRIBUTE_WEAK __exc_nmi(void);
void ATTRIBUTE_WEAK __exc_hardfault(void);

void ATTRIBUTE_WEAK __exc_memmanage(void);
void ATTRIBUTE_WEAK __exc_busfault(void);
void ATTRIBUTE_WEAK __exc_usagefault(void);
void ATTRIBUTE_WEAK __exc_svc(void);
void ATTRIBUTE_WEAK __exc_debug_monitor(void);
void ATTRIBUTE_WEAK __exc_pendsv(void);
void ATTRIBUTE_WEAK __exc_systick(void);

/* Peripheral interrupts */
/* Peripheral interrupts */
void ATTRIBUTE_WEAK __irq_wwdg(void);
void ATTRIBUTE_WEAK __irq_pvd(void);
void ATTRIBUTE_WEAK __irq_tamper(void);
void ATTRIBUTE_WEAK __irq_rtc(void);
void ATTRIBUTE_WEAK __irq_flash(void);
void ATTRIBUTE_WEAK __irq_rcc(void);
void ATTRIBUTE_WEAK __irq_exti0(void);
void ATTRIBUTE_WEAK __irq_exti1(void);
void ATTRIBUTE_WEAK __irq_exti2(void);
void ATTRIBUTE_WEAK __irq_exti3(void);
void ATTRIBUTE_WEAK __irq_exti4(void);
void ATTRIBUTE_WEAK __irq_dma1_channel1(void);
void ATTRIBUTE_WEAK __irq_dma1_channel2(void);
void ATTRIBUTE_WEAK __irq_dma1_channel3(void);
void ATTRIBUTE_WEAK __irq_dma1_channel4(void);
void ATTRIBUTE_WEAK __irq_dma1_channel5(void);
void ATTRIBUTE_WEAK __irq_dma1_channel6(void);
void ATTRIBUTE_WEAK __irq_dma1_channel7(void);
void ATTRIBUTE_WEAK __irq_adc(void);
void ATTRIBUTE_WEAK __irq_usb_hp_can_tx(void);
void ATTRIBUTE_WEAK __irq_usb_lp_can_rx0(void);
void ATTRIBUTE_WEAK __irq_can_rx1(void);
void ATTRIBUTE_WEAK __irq_can_sce(void);
void ATTRIBUTE_WEAK __irq_exti9_5(void);
void ATTRIBUTE_WEAK __irq_tim1_brk(void);
void ATTRIBUTE_WEAK __irq_tim1_up(void);
void ATTRIBUTE_WEAK __irq_tim1_trg_com(void);
void ATTRIBUTE_WEAK __irq_tim1_cc(void);
void ATTRIBUTE_WEAK __irq_tim2(void);
void ATTRIBUTE_WEAK __irq_tim3(void);
void ATTRIBUTE_WEAK __irq_tim4(void);
void ATTRIBUTE_WEAK __irq_i2c1_ev(void);
void ATTRIBUTE_WEAK __irq_i2c1_er(void);
void ATTRIBUTE_WEAK __irq_i2c2_ev(void);
void ATTRIBUTE_WEAK __irq_i2c2_er(void);
void ATTRIBUTE_WEAK __irq_spi1(void);
void ATTRIBUTE_WEAK __irq_spi2(void);
void ATTRIBUTE_WEAK __irq_usart1(void);
void ATTRIBUTE_WEAK __irq_usart2(void);
void ATTRIBUTE_WEAK __irq_usart3(void);
void ATTRIBUTE_WEAK __irq_exti15_10(void);
void ATTRIBUTE_WEAK __irq_rtcalarm(void);
void ATTRIBUTE_WEAK __irq_usbwakeup(void);
#if defined (STM32_HIGH_DENSITY)
void ATTRIBUTE_WEAK __irq_tim8_brk(void);
void ATTRIBUTE_WEAK __irq_tim8_up(void);
void ATTRIBUTE_WEAK __irq_tim8_trg_com(void);
void ATTRIBUTE_WEAK __irq_tim8_cc(void);
void ATTRIBUTE_WEAK __irq_adc3(void);
void ATTRIBUTE_WEAK __irq_fsmc(void);
void ATTRIBUTE_WEAK __irq_sdio(void);
void ATTRIBUTE_WEAK __irq_tim5(void);
void ATTRIBUTE_WEAK __irq_spi3(void);
void ATTRIBUTE_WEAK __irq_uart4(void);
void ATTRIBUTE_WEAK __irq_uart5(void);
void ATTRIBUTE_WEAK __irq_tim6(void);
void ATTRIBUTE_WEAK __irq_tim7(void);
void ATTRIBUTE_WEAK __irq_dma2_channel1(void);
void ATTRIBUTE_WEAK __irq_dma2_channel2(void);
void ATTRIBUTE_WEAK __irq_dma2_channel3(void);
void ATTRIBUTE_WEAK __irq_dma2_channel4_5(void);
#endif

/*
 * Provide weak aliases for each Exception handler to defaultHandler.
 * As they are weak aliases, any function with the same name will override
 * this definition.
 */
#pragma weak __exc_nmi = defaultHandler
#pragma weak __exc_memmanage = defaultHandler
#pragma weak __exc_busfault = defaultHandler
#pragma weak __exc_usagefault = defaultHandler
#pragma weak __exc_svc = defaultHandler
#pragma weak __exc_debug_monitor = defaultHandler
#pragma weak __exc_pendsv = defaultHandler
#pragma weak __exc_systick = defaultHandler
#pragma weak __irq_wwdg = defaultHandler
#pragma weak __irq_pvd = defaultHandler
#pragma weak __irq_tamper = defaultHandler
#pragma weak __irq_rtc = defaultHandler
#pragma weak __irq_flash = defaultHandler
#pragma weak __irq_rcc = defaultHandler
#pragma weak __irq_exti0 = defaultHandler
#pragma weak __irq_exti1 = defaultHandler
#pragma weak __irq_exti2 = defaultHandler
#pragma weak __irq_exti3 = defaultHandler
#pragma weak __irq_exti4 = defaultHandler
#pragma weak __irq_dma1_channel1 = defaultHandler
#pragma weak __irq_dma1_channel2 = defaultHandler
#pragma weak __irq_dma1_channel3 = defaultHandler
#pragma weak __irq_dma1_channel4 = defaultHandler
#pragma weak __irq_dma1_channel5 = defaultHandler
#pragma weak __irq_dma1_channel6 = defaultHandler
#pragma weak __irq_dma1_channel7 = defaultHandler
#pragma weak __irq_adc = defaultHandler
#pragma weak __irq_usb_hp_can_tx = defaultHandler
#pragma weak __irq_usb_lp_can_rx0 = defaultHandler
#pragma weak __irq_can_rx1 = defaultHandler
#pragma weak __irq_can_sce = defaultHandler
#pragma weak __irq_exti9_5 = defaultHandler
#pragma weak __irq_tim1_brk = defaultHandler
#pragma weak __irq_tim1_up = defaultHandler
#pragma weak __irq_tim1_trg_com = defaultHandler
#pragma weak __irq_tim1_cc = defaultHandler
#pragma weak __irq_tim2 = defaultHandler
#pragma weak __irq_tim3 = defaultHandler
#pragma weak __irq_tim4 = defaultHandler
#pragma weak __irq_i2c1_ev = defaultHandler
#pragma weak __irq_i2c1_er = defaultHandler
#pragma weak __irq_i2c2_ev = defaultHandler
#pragma weak __irq_i2c2_er = defaultHandler
#pragma weak __irq_spi1 = defaultHandler
#pragma weak __irq_spi2 = defaultHandler
#pragma weak __irq_usart1 = defaultHandler
#pragma weak __irq_usart2 = defaultHandler
#pragma weak __irq_usart3 = defaultHandler
#pragma weak __irq_exti15_10 = defaultHandler
#pragma weak __irq_rtcalarm = defaultHandler
#pragma weak __irq_usbwakeup = defaultHandler
#if defined (STM32_HIGH_DENSITY)
#pragma weak __irq_tim8_brk = defaultHandler
#pragma weak __irq_tim8_up = defaultHandler
#pragma weak __irq_tim8_trg_com = defaultHandler
#pragma weak __irq_tim8_cc = defaultHandler
#pragma weak __irq_adc3 = defaultHandler
#pragma weak __irq_fsmc = defaultHandler
#pragma weak __irq_sdio = defaultHandler
#pragma weak __irq_tim5 = defaultHandler
#pragma weak __irq_spi3 = defaultHandler
#pragma weak __irq_uart4 = defaultHandler
#pragma weak __irq_uart5 = defaultHandler
#pragma weak __irq_tim6 = defaultHandler
#pragma weak __irq_tim7 = defaultHandler
#pragma weak __irq_dma2_channel1 = defaultHandler
#pragma weak __irq_dma2_channel2 = defaultHandler
#pragma weak __irq_dma2_channel3 = defaultHandler
#pragma weak __irq_dma2_channel4_5 = defaultHandler
#endif /* STM32_HIGH_DENSITY */

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
	__exc_nmi,				// NMI handler
	__exc_hardfault,		// hard fault handler
};

FunctionPointer ramVectors[] __attribute__ ((section(".vectors"))) =
#else
FunctionPointer flashVectors[] __attribute__ ((section(".reset"))) =
#endif
{
	(FunctionPointer) &__stack_end,			// stack pointer
	resetHandler,				// code entry point
	__exc_nmi,					// NMI handler
	__exc_hardfault,			// hard fault handler
	__exc_memmanage,          /* MPU Fault Handler */
	__exc_busfault,           /* Bus Fault Handler */
	__exc_usagefault,         /* Usage Fault Handler */
	0,                          /* Reserved */
	0,                          /* Reserved */
	0,                          /* Reserved */
	0,                          /* Reserved */
	__exc_svc,                /* SVCall Handler */
	__exc_debug_monitor,           /* Debug Monitor Handler */
	0,                          /* Reserved */
	__exc_pendsv,             /* PendSV Handler */
	__exc_systick,            /* SysTick Handler */
	/* Peripheral interrupts */
	__irq_wwdg,            /* Window Watchdog */
	__irq_pvd,             /* PVD through EXTI Line detect */
	__irq_tamper,          /* Tamper */
	__irq_rtc,             /* RTC */
	__irq_flash,           /* Flash */
	__irq_rcc,             /* RCC */
	__irq_exti0,           /* EXTI Line 0 */
	__irq_exti1,           /* EXTI Line 1 */
	__irq_exti2,           /* EXTI Line 2 */
	__irq_exti3,           /* EXTI Line 3 */
	__irq_exti4,           /* EXTI Line 4 */
	__irq_dma1_channel1,   /* DMA1 Channel 1 */
	__irq_dma1_channel2,   /* DMA1 Channel 2 */
	__irq_dma1_channel3,   /* DMA1 Channel 3 */
	__irq_dma1_channel4,   /* DMA1 Channel 4 */
	__irq_dma1_channel5,   /* DMA1 Channel 5 */
	__irq_dma1_channel6,   /* DMA1 Channel 6 */
	__irq_dma1_channel7,   /* DMA1 Channel 7 */
	__irq_adc,          /* ADC1 & ADC2 */
	__irq_usb_hp_can_tx,  /* USB High Priority or CAN1 TX */
	__irq_usb_lp_can_rx0, /* USB Low  Priority or CAN1 RX0 */
	__irq_can_rx1,        /* CAN1 RX1 */
	__irq_can_sce,        /* CAN1 SCE */
	__irq_exti9_5,         /* EXTI Line 9..5 */
	__irq_tim1_brk,        /* TIM1 Break */
	__irq_tim1_up,         /* TIM1 Update */
	__irq_tim1_trg_com,    /* TIM1 Trigger and Commutation */
	__irq_tim1_cc,         /* TIM1 Capture Compare */
	__irq_tim2,            /* TIM2 */
	__irq_tim3,            /* TIM3 */
	__irq_tim4,            /* TIM4 */
	__irq_i2c1_ev,         /* I2C1 Event */
	__irq_i2c1_er,         /* I2C1 Error */
	__irq_i2c2_ev,         /* I2C2 Event */
	__irq_i2c2_er,         /* I2C2 Error */
	__irq_spi1,            /* SPI1 */
	__irq_spi2,            /* SPI2 */
	__irq_usart1,          /* USART1 */
	__irq_usart2,          /* USART2 */
	__irq_usart3,          /* USART3 */
	__irq_exti15_10,       /* EXTI Line 15..10 */
	__irq_rtcalarm,        /* RTC Alarm through EXTI Line */
	__irq_usbwakeup,       /* USB Wakeup from suspend */
#if defined (STM32_HIGH_DENSITY)
	__irq_tim8_brk,
	__irq_tim8_brk
	__irq_tim8_up
	__irq_tim8_trg_com
	__irq_tim8_cc
	__irq_adc3
	__irq_fsmc
	__irq_sdio
	__irq_tim5
	__irq_spi3
	__irq_uart4
	__irq_uart5
	__irq_tim6
	__irq_tim7
	__irq_dma2_channel1
	__irq_dma2_channel2
	__irq_dma2_channel3
	__irq_dma2_channel4_5
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

extern void exit (int) __attribute__ ((noreturn, weak));

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
	
	// TODO only for STM32
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
