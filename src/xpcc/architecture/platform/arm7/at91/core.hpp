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
 * $Id: core.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_AT91__CORE_HPP
#define XPCC_AT91__CORE_HPP

namespace xpcc
{
	namespace at91
	{
		/**
		 * \brief	Core - Collection of functions like the reset controller,
		 * 					clock generator or power managment
		 * 
		 * \ingroup	at91
		 */
		class Core
		{
			//static void
			//initialize();
			
			// Reset Controller (RSTC)
			// RTT as slow
			// Voltage Regulator Mode Controller (VREG)
			// PMC:
			/*
			#define AT91C_ID_FIQ    ((unsigned int)  0) // Advanced Interrupt Controller (FIQ)
			#define AT91C_ID_SYS    ((unsigned int)  1) // System Peripheral (PIT, RTT, WDT, DBGU, PMC, RSTC, EFC)
			#define AT91C_ID_PIOA   ((unsigned int)  2) // Parallel IO Controller
			#define AT91C_ID_ADC    ((unsigned int)  4) // Analog-to-Digital Converter
			#define AT91C_ID_SPI    ((unsigned int)  5) // Serial Peripheral Interface
			#define AT91C_ID_US0    ((unsigned int)  6) // USART 0
			#define AT91C_ID_US1    ((unsigned int)  7) // USART 1
			#define AT91C_ID_SSC    ((unsigned int)  8) // Serial Synchronous Controller
			#define AT91C_ID_TWI    ((unsigned int)  9) // Two-Wire Interface
			#define AT91C_ID_PWMC   ((unsigned int) 10) // PWM Controller
			#define AT91C_ID_UDP    ((unsigned int) 11) // USB Device Port
			#define AT91C_ID_TC0    ((unsigned int) 12) // Timer Counter 0
			#define AT91C_ID_TC1    ((unsigned int) 13) // Timer Counter 1
			#define AT91C_ID_TC2    ((unsigned int) 14) // Timer Counter 2
			#define AT91C_ID_IRQ0   ((unsigned int) 30) // Advanced Interrupt Controller (IRQ0)
			#define AT91C_ID_IRQ1   ((unsigned int) 31) // Advanced Interrupt Controller (IRQ1)
			*/
		};
	}
}

#endif // XPCC_AT91__CORE_HPP
