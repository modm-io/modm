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
 * \ingroup		arm7
 * \defgroup	lpc2000		LPC2xxx
 * 
 * <h2>Features:</h2>
 * 
 * - ARM7TDMI-S core
 *   - Speeds up to 72MHz (from Flash)
 *   - 8-512 kB Flash
 *   - 2-96 kB RAM
 * - 2-4x UART
 * - 1-2x SPI
 * - CAN, LIN, I2C
 * - USB, I2S, and SD/MMC interface options 
 * - 10-bit ADC and DAC options 
 * - Low power consumption
 * - LQFP48-package (LPC210x) up to LQFP208 (LPC24xx) 
 * - 5V-tolerant IOs
 * - two voltages needed:
 *   - 3,0-3,6V IO voltage
 *   - 1,65-1,95V core voltage
 * 
 * Devices:
 * - LPC2101, LPC2102, and LPC2103 Low-Cost, Tiny MCUs
 * - LPC214x Full-Speed USB 2.0 Device MCUs
 * - LPC23xx Dual AHB Bus MCUs
 * - LPC24xx Dual AHB Bus MCUs with External Memory Interface
 * 
 * <h2>LPC210x/LPC22xx/LPC21xx vs. LPC23xx/LPC24xx</h2>
 * 
 * The LPC2000 series consists of two main types: The older LPC210x/LPC22xx/LPC21xx
 * devices and the newer, improved LPC23xx/LPC24xx devices. As there are
 * some differences concerning the clock nets, interrupt controller or DMA
 * channels, a define (\c __ARM_LPC23_24__) is available to identify the device
 * type.
 * 
 * \see	http://ics.nxp.com/products/lpc2000/
 * \see	http://ics.nxp.com/products/lpc2000/all/
 * \see	http://www.mikrocontroller.net/articles/LPC2000
 */

#include "lpc/registers.h"

#include "lpc/clock.hpp"
#include "lpc/gpio.hpp"
