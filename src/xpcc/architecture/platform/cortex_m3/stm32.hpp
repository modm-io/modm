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
 *
 * $Id$
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
 * - TODO: STM32 F4 series of high-performance MCUs with DSP and FPU instructions
 *
 * The Value line (STM32F100-102) is not supported!
 */

#include "stm32/device.h"

#include "stm32/core.hpp"
#include "stm32/can.hpp"
#include "stm32/gpio.hpp"
#include "stm32/uart.hpp"
#include "stm32/spi.hpp"
#include "stm32/timer.hpp"
