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

/**
 * Platform support for
 * 		NXP Cortex-M0 Series LPC11xx and LPC11Cxx
 */

// Use NXP CMSIS library for register name definitions
#include <lpc11xx/cmsis/LPC11xx.h>
#include <lpc11xx/cmsis/core_cm0.h>
#include <lpc11xx/cmsis/system_LPC11xx.h>

/**
 * XPCC drivers for various peripherals and core components.
 *
 * Reference is
 * 		LPC111x/LPC11Cxx User manual
 *		Rev. 11 — 26 July 2012
 *		UM10398
 */

// Chapter 12: General Purpose I/O (GPIO)
#include "gpio.hpp"

// Chapter 13: UART
#include "uart/uart_1.hpp"

// Chapter 14: SPI0/1 with SSP
#include "spi/spi_0.hpp"
#include "spi/spi_1.hpp"

// Chapter 15: I2C bus
// TODO

// Chapter 16: C_CAN controller
// The C_CAN block is available in LPC11Cxx parts only (LPC11C00 series).
#if defined(__ARM_LPC11CXX__)
// TODO Write own CAN driver if needed
#endif

// Chapter 17: C_CAN on-chip drivers
// The C_CAN block is available in LPC11Cxx parts only (LPC11C00 series).
#if defined(__ARM_LPC11CXX__)
#include "c_can/c_can.hpp"
#endif

// Chapter 18: LPC1100/LPC1100C/LPC1100L series 16-bit counter/timer CT16B0/1
#if defined(__ARM_LPC11XX__) || defined(__ARM_LPC11CXX__) || defined(__ARM_LPC11XXL__)
// TODO
#endif

// Chapter 19: LPC1100XL series: 16-bit counter/timer CT16B0/1
#if defined(__ARM_LPC1100XL__)
// TODO
#endif

// Chapter 20: LPC1100/LPC1100C/LPC1100L series: 32-bit counter/timer CT32B0/1
#if defined(__ARM_LPC11XX__) || defined(__ARM_LPC11CXX__) || defined(__ARM_LPC11XXL__)
// TODO
#endif

// Chapter 21: LPC1100XL series: 32-bit counter/timer CT32B0/1
#if defined(__ARM_LPC1100XL__)
// TODO
#endif

// Chapter 22/23: LPC111x/LPC11Cxx (Windowed) WatchDog Timer (WDT)
#if defined(__ARM_LPC11XX__) || defined(__ARM_LPC11CXX__)
// TODO
#endif

// Chapter 24: LPC111x/LPC11Cxx System tick timer (SysTick)
#if defined(__ARM_LPC11XX__) || defined(__ARM_LPC11CXX__)
#include "systick_timer.hpp"
#endif

// Chapter 25: LPC111x/LPC11Cxx ADC
#if defined(__ARM_LPC11XX__) || defined(__ARM_LPC11CXX__)
#include "adc/adc.hpp"
#endif

// Chapter 26: LPC111x/LPC11Cxx Flash programming firmware
#if defined(__ARM_LPC11XX__) || defined(__ARM_LPC11CXX__)
// TODO
#endif

extern "C"
{
	#include <lpc11xx/driver/driver_config.h>
	
	#include <lpc11xx/driver/gpio.h>
	#include <lpc11xx/driver/timer32.h>
}

