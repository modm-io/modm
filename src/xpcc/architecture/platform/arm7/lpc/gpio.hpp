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

#ifndef XPCC_LPC__GPIO_HPP
#define XPCC_LPC__GPIO_HPP

#include <xpcc/architecture/peripheral/gpio.hpp>
#include "../lpc.hpp"

namespace xpcc
{
	namespace gpio
	{
		/**
		 * \ingroup	lpc
		 */
		enum Configuration
		{
			NORMAL,		///< standard operation (floating input, external signal needed!)
		};
	}
}

/**
 * \brief	Create a input/output pin type
 * 
 * TODO No internal pullup, reentrant only on set/reset, not on output/input
 * 
 * All I/O functions are reentrant and therefore may be used from within
 * an interrupt and the normal program. To achieve this every function has to
 * operate on read/write-only registers, so the Output Data Status
 * Register (ODSR) had to be disabled for write access.
 * 
 * Operations take between 1 and 6 cycles depending how much information is
 * already present in the registers (PIO base register and Pin mask may be loaded).
 * 
 * Example:
 * \code
 * GPIO__OUTPUT(Led1, 0, 12);
 * GPIO__OUTPUT(Led2, 0, 13);
 * ...
 * 
 * Led1::setOutput();
 * Led2::setOutput();
 * 
 * Led1::toggle();
 * Led2::set();
 * \endcode
 * 
 * This generates:
 * \verbatim
mvn     r3, #0xb00      ; Initialize registers
mov     r2, #0x40000
mov     r1, #0x20000

str     r2, [r3, #-255] ; Led1.setOutput();
str     r2, [r3, #-239]

str     r1, [r3, #-255] ; Led2.setOutput();
str     r1, [r3, #-239]

ldr     r0, [r3, #-199] ; Led1::toggle()
tst     r0, #0x40000
strne   r2, [r3, #-207]
streq   r2, [r3, #-203]

str     r1, [r3, #-207] ; Led2::set()
\endverbatim
 * 
 * Here you can see that the set() functions takes only one operation because
 * r1 and r3 are already initialized. If we call the same function out of context
 * it needs three operations:
 * 
 * \verbatim
mov     r2, #0x20000    ; Initialize registers
mvn     r3, #0xb00
str     r2, [r3, #-207] ; Led2::set()
\verbatim
 * 
 * Notes:
 * - In most LPC2000 devices, P0.14 is used for entering ISP mode (when being
 *   held low during reset). In the LPC23xx and LPC24xx, however, P2.10 is used
 *   to enter ISP mode.
 * - All LPC2000 microcontroller port 0 pins are true bi-directional GPIOs.
 *   The only minor exception is that the two I2C pins have an open drain
 *   output structure. With external pull-up resistors, they can also be used
 *   as true GPIOs.
 * - There are no programmable pull-up resistors. But some pins have internal
 *   pull-ups. These pull-ups are always active and can not be disabled.
 *   Set those pins high or keep them floating before entering power down mode.
 *   If set low, they will consume around 50 μA (typical) on each pin.
 *   Port 0 does not have any internal pull-ups. These pins can be set low.
 * - The GPIO pins can source or sink 4mA maximum. 
 * - The toggling speed on the standard GPIO pins is system_clock/14.
 *   If 72MHz operation is used, then the toggling speed is around 5MHz.
 *   The toggling speed on the Fast GPIO pins is system clock/4. If 72MHz
 *   operation is used, then the toggling speed is around 18MHz.
 * - In LPC23xx new accelerated GPIO functions were introduced.
 *   Only Ports 0 and 1 are accessible by the legacy group of registers.
 *   See UM10211, p170
 * 
 * External Interrupts:
 * - In some devices (like the LPC23xx and LPC24xx series), all pins on
 *   ports 0 and 2 plus EXTINTx pins can be used for external interrupts.
 *   However, in other devices, only dedicated EXTINTx pins could be used as
 *   external interrupts. Please note that only the dedicated EXTINTx pins
 *   can wake the chip from power down mode.
 * 
 * \hideinitializer
 * \ingroup	lpc
 */

#ifdef __ARM_LPC23_24__
#include "gpio/gpio_lpc23_24xx.hpp"
#else
#include "gpio/gpio_lpc21xx.hpp"
#endif

#endif // XPCC_LPC__GPIO_HPP
