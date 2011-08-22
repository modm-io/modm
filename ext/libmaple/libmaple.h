/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

/**
 *  @file libmaple.h
 *  @brief General include file for libmaple
 */

#ifndef _LIBMAPLE_H_
#define _LIBMAPLE_H_

#include "libmaple_types.h"
#include "stm32.h"
#include "util.h"
#include "delay.h"

/*
 * Where to put usercode, based on space reserved for bootloader.
 *
 * FIXME this has no business being here
 */
#define USER_ADDR_ROM 0x08005000
#define USER_ADDR_RAM 0x20000C00
#define STACK_TOP     0x20000800

/* MCU-specific configuration */
#if defined(MCU_STM32F103RB)
    /* e.g., LeafLabs Maple */

    /* Number of GPIO ports (GPIOA, GPIOB, etc.) */
    #define NR_GPIO_PORTS              4

    /* SRAM size, in bytes */
    #define SRAM_SIZE             0x5000

#elif defined(MCU_STM32F103ZE)
    /* e.g., LeafLabs Maple Native */

    #define NR_GPIO_PORTS              7
    #define SRAM_SIZE            0x10000

#elif defined(MCU_STM32F103CB)
    /* e.g., LeafLabs Maple Mini */

    /* Note that this is not, strictly speaking, true.  But only pins
       0 and 1 exist, and they're used for OSC on the Mini, so we'll
       live with this for now. */
    #define NR_GPIO_PORTS              3

    #define SRAM_SIZE             0x5000

#elif defined(MCU_STM32F103RE)
    /* e.g., LeafLabs Maple RET6 edition */

    #define NR_GPIO_PORTS              4
    #define SRAM_SIZE            0x10000

#else
	#error "No MCU type specified. Add something like -DMCU_STM32F103RB "   \
    	   "to your compiler arguments (probably in a Makefile)."
#endif

#endif

// CMSIS compatible interrupts
#define	__exc_nmi				NMI_Handler
#define	__exc_hardfault			HardFault_Handler
#define	__exc_memmanage			MemManage_Handler
#define	__exc_busfault			BusFault_Handler
#define	__exc_usagefault		UsageFault_Handler
#define	__exc_svc				SVC_Handler
#define	__exc_debug_monitor		DebugMon_Handler
#define	__exc_pendsv			PendSV_Handler
#define	__exc_systick			SysTick_Handler

#define	__irq_wwdg				WWDG_IRQHandler
#define	__irq_pvd				PVD_IRQHandler
#define	__irq_tamper			TAMPER_IRQHandler
#define	__irq_rtc				RTC_IRQHandler
#define	__irq_flash				FLASH_IRQHandler
#define	__irq_rcc				RCC_IRQHandler
#define	__irq_exti0				EXTI0_IRQHandler
#define	__irq_exti1				EXTI1_IRQHandler
#define	__irq_exti2				EXTI2_IRQHandler
#define	__irq_exti3				EXTI3_IRQHandler
#define	__irq_exti4				EXTI4_IRQHandler
#define	__irq_dma1_channel1		DMA1_Channel1_IRQHandler
#define	__irq_dma1_channel2		DMA1_Channel2_IRQHandler
#define	__irq_dma1_channel3		DMA1_Channel3_IRQHandler
#define	__irq_dma1_channel4		DMA1_Channel4_IRQHandler
#define	__irq_dma1_channel5		DMA1_Channel5_IRQHandler
#define	__irq_dma1_channel6		DMA1_Channel6_IRQHandler
#define	__irq_dma1_channel7		DMA1_Channel7_IRQHandler
#define	__irq_adc				ADC1_2_IRQHandler
#define	__irq_usb_hp_can_tx		USB_HP_CAN1_TX_IRQHandler
#define	__irq_usb_lp_can_rx0	USB_LP_CAN1_RX0_IRQHandler
#define	__irq_can_rx1			CAN1_RX1_IRQHandler
#define	__irq_can_sce			CAN1_SCE_IRQHandler
#define	__irq_exti9_5			EXTI9_5_IRQHandler
#if defined STM32_XL_DENSITY
	#define	__irq_tim1_brk		TIM1_BRK_TIM9_IRQHandler
	#define	__irq_tim1_up		TIM1_UP_TIM10_IRQHandler
	#define	__irq_tim1_trg_com	TIM1_TRG_COM_TIM11_IRQHandler
#else
	#define	__irq_tim1_brk		TIM1_BRK_IRQHandler
	#define	__irq_tim1_up		TIM1_UP_IRQHandler
	#define	__irq_tim1_trg_com	TIM1_TRG_COM_IRQHandler
#endif
#define	__irq_tim1_cc			TIM1_CC_IRQHandler
#define	__irq_tim2				TIM2_IRQHandler
#define	__irq_tim3				TIM3_IRQHandler
#define	__irq_tim4				TIM4_IRQHandler
#define	__irq_i2c1_ev			I2C1_EV_IRQHandler
#define	__irq_i2c1_er			I2C1_ER_IRQHandler
#define	__irq_i2c2_ev			I2C2_EV_IRQHandler
#define	__irq_i2c2_er			I2C2_ER_IRQHandler
#define	__irq_spi1				SPI1_IRQHandler
#define	__irq_spi2				SPI2_IRQHandler
#define	__irq_usart1			USART1_IRQHandler
#define	__irq_usart2			USART2_IRQHandler
#define	__irq_usart3			USART3_IRQHandler
#define	__irq_exti15_10			EXTI15_10_IRQHandler
#define	__irq_rtcalarm			RTCAlarm_IRQHandler
#define	__irq_usbwakeup			USBWakeUp_IRQHandler
#if defined (STM32_HIGH_DENSITY) || defined (STM32_XL_DENSITY)
	#define	__irq_tim8_brk		TIM8_BRK_TIM12_IRQHandler
	#define	__irq_tim8_up		TIM8_UP_TIM13_IRQHandler
	#define	__irq_tim8_trg_com	TIM8_TRG_COM_TIM14_IRQHandler
	#define	__irq_tim8_cc		TIM8_CC_IRQHandler
	#define	__irq_adc3			ADC3_IRQHandler
	#define	__irq_fsmc			FSMC_IRQHandler
	#define	__irq_sdio			SDIO_IRQHandler
	#define	__irq_tim5			TIM5_IRQHandler
	#define	__irq_spi3			SPI3_IRQHandler
	#define	__irq_uart4			UART4_IRQHandler
	#define	__irq_uart5			UART5_IRQHandler
	#define	__irq_tim6			TIM6_IRQHandler
	#define	__irq_tim7			TIM7_IRQHandler
	#define	__irq_dma2_channel1	DMA2_Channel1_IRQHandler
	#define	__irq_dma2_channel2	DMA2_Channel2_IRQHandler
	#define	__irq_dma2_channel3	DMA2_Channel3_IRQHandler
	#define	__irq_dma2_channel4_5	DMA2_Channel4_5_IRQHandler
#endif
