// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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
/**
 * \ingroup		cortex_m3
 * \defgroup	stm32		STM32Fxxx
 * 
 * STM32 is a family of 32-bits ARM Cortex-M3 microcontrollers developed and
 * marketed by STMicroelectronics.
 * 
 * Supported Devices:
 * - STM32 F1 series of mainstream MCUs
 *   - Performance line STM32F103  - 72 MHz, up to 1 Mbyte Flash with motor
 *     control, USB and CAN
 *   - Connectivity line STM32F105/107 - 72 MHz CPU with Ethernet MAC, CAN
 *     and USB 2.0 OTG
 * - TODO: STM32 F2 series of high-performance MCUs
 * - TODO: STM32 F3 series of high-performance MCUs with DSP and FPU instructions and analog peripherals
 * - TODO: STM32 F4 series of high-performance MCUs with DSP and FPU instructions
 *
 * Defines:
 * - Frequencies: These defines are defined as maximum possible frequencies of
 * 	 the busses. Hardware drivers like USARTs are using them for configuration.
 * 	 Remember, you can override defines in the defines section of your project.cfg
 * 	 if you need.
 *   - STM32_AHB_FREQUENCY
 *   - STM32_APB1_FREQUENCY
 *   - STM32_APB2_FREQUENCY
 *
 *
 * The Value line (STM32F100-102) is not supported!
 */

/*
 * Common to all STM32 processors.
 */

#include "stm32/device.h"

#include "stm32/core.hpp"
#include "stm32/gpio.hpp"
#include "stm32/clock.hpp"


/**
 * Specific hardware for different processors.
 */




/**
 * CAN Controller
 */

// CAN1
#if defined(STM32F10X) || defined(STM32F3XX) || defined(STM32F2XX) || defined(STM32F4XX)
#include "stm32/can/can_1.hpp"
#include "stm32/can/can_filter.hpp"
#endif

// CAN2
#if defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
#include "stm32/can/can_2.hpp"
#endif

// External Interrupt only tested on STM32F4XX
#if defined(STM32F4XX)
#include "stm32/exti.hpp"
#endif

// HW drivers enabled for all supported STM32 series
#if defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F3XX) \
	|| defined(STM32F4XX)
#include "stm32/uart/usart_1.hpp"
#include "stm32/uart/usart_2.hpp"
#include "stm32/uart/usart_3.hpp"
#include "stm32/uart/uart_4.hpp"
#include "stm32/uart/uart_5.hpp"
#endif

// COMP1-7 only available on STM32F3 series
#if defined(STM32F3XX)
#include "stm32/comp/comp_1.hpp"
#include "stm32/comp/comp_2.hpp"
#include "stm32/comp/comp_3.hpp"
#include "stm32/comp/comp_4.hpp"
#include "stm32/comp/comp_5.hpp"
#include "stm32/comp/comp_6.hpp"
#include "stm32/comp/comp_7.hpp"
#endif


// Other hardware, not yet implemented for F3
#if defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
#include "stm32/fsmc.hpp"

#include "stm32/uart/usart_6.hpp"

#include "stm32/spi/spi_1.hpp"
#include "stm32/spi/spi_2.hpp"
#include "stm32/spi/spi_3.hpp"

#include "stm32/adc/adc_1.hpp"
#include "stm32/adc/adc_2.hpp"
#include "stm32/adc/adc_3.hpp"

#include "stm32/i2c/i2c_master_1.hpp"
#include "stm32/i2c/i2c_master_2.hpp"
#include "stm32/i2c/i2c_master_3.hpp"

#include "stm32/systick_timer.hpp"
#include "stm32/timer.hpp"

#endif
