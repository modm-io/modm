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
 * \ingroup		cortex_m0
 * \defgroup	lpc1100		LPC11xx
 * 
 * <h2>Features</h2>
 * 
 * - Based on ARM Cortex processor cores: 
 *   - Cortex-M0 core (LPC11xx, LPC12xx)
 *   - Cortex-M3 core (LPC13xx, LPC17xx and LPC18xx)
 * - Speeds up to 100MHz from flash or RAM 
 * - Available with: 
 *   - Ethernet, USB Host/OTG/Device, CAN, I2S 
 *   - Fast-mode Plus (Fm+), I2C, SPI/SSP, UARTs 
 *   - 12-bit ADC @ 1MHz conversion rates 
 *   - Low-power Real Time Clock 
 *   - Motor Control PWM and Quadrature Encoder Interface 
 * 
 * \see		http://ics.nxp.com/products/lpc1000/all/
 */

#if defined __ARM_LPC11XX__
#	include "lpc/lpc11xx.hpp"
#endif
