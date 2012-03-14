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
 * \ingroup		arm7
 * \defgroup	at91		AT91SAM7xxx
 * 
 * <h2>Features</h2>
 * - ARM7TDMI-core
 * - up to 55 MHz from RAM
 * - up to 30 (20 for Rev.C) MHz from Flash without Wait-states
 * - DMA channels for every peripheral
 * 
 * <h3>Comparison</h3>
 * 
 * The flash memory of the AT91SAM7 is slower than the one of the LPC2000.
 * Therefore, it often speeds up the execution, to use Thumb code (16-bit) instead of
 * ARM code (32-bit), since the latter has a longer instruction words and therefore
 * needs more time to load the instructions.
 * 
 * With 0 wait state @ 30MHz:
 * - ARM 27 MIPS
 * - Thumb 21 MIPS
 * 
 * With 1 wait state @ 50MHz:
 * - ARM 24 MIPS
 * - Thumb 38 MIPS (Flash Accelerator)
 * 
 * To achieve maximum performance for time-critical functions compile them
 * in ARM mode and load them into the (faster) RAM.
 * 
 * <h2>Memory</h2>
 * 
 * Internal High-speed Flash
 * - 512 Kbytes (AT91SAM7S512)
 * - 256 Kbytes (AT91SAM7S256)
 * - 128 Kbytes (AT91SAM7S128)
 * - 64 Kbytes (AT91SAM7S64)
 * - 32 Kbytes (AT91SAM7S32/321)
 * - Single Cycle Access at Up to 30 (20 for Rev.C) MHz in Worst Case Conditions
 * 
 * Internal High-speed SRAM, Single-cycle Access at Maximum Speed
 * - 64 Kbytes (AT91SAM7S512/256)
 * - 32 Kbytes (AT91SAM7S128)
 * - 16 Kbytes (AT91SAM7S64)
 * - 8 Kbytes (AT91SAM7S32/321)
 * 
 * Other devices of the AT91SAM7-family:
 * - AT91SAM7SE
 *   - with an external memory interface
 * - AT91SAM7X
 *   - Ethernet and CAN interfaces
 * - AT91SAM7A
 *   - optimized for embedded control, less RAM than AT91SAM7S but multiple CAN interfaces
 * 
 * Devices not supported:
 * - AT91SAM7S16/161
 * 
 * <h3>Internal Memory mapping:</h3>
 * 
 * \verbatim
0x0000 0000 - 0x000f ffff => Flash before Remap or SRAM after Remap (1 MByte)
0x0010 0000 - 0x001f ffff => Internal Flash (1 MByte)
0x0020 0000 - 0x002f ffff => Internal SRAM (1 MByte)
0x0030 0000 - 0xefff ffff => Undefined Areas, Abort (253 MByte)
0xf000 0000 - 0xffff efff => Peripherals (UART, SPI, ADC, Timer etc.)
0xffff f000 - 0xffff ffff => System Controller
\endverbatim
 * 
 * See page 20 of the Datasheet
 * 
 * <h2>Clock</h2>
 * 
 * Clock Generator
 * - RC Oscillator ranges between 22 kHz and 42 kHz => SLCK (Slow Clock), always active
 * - Main Oscillator frequency ranges between 3 and 20 MHz => MAINCK
 * - Main Oscillator can be bypassed (for an external clock source)
 * - PLL output ranges between 80 and 220 MHz => PLLCK
 * 
 * See page 30-31 of AT91SAM7S Series Summary.
 * 
 * \see	http://www.at91.com/component/resource/list/Products/3-SAM7.html
 * \see	http://www.mikrocontroller.net/articles/AT91SAM
 */

namespace xpcc
{
	/**
	 * \ingroup	at91
	 * \brief	AT91SAM7xxx family
	 */
	namespace at91
	{
	}
}

#include "at91/device.h"

// peripheral drivers
#include "at91/core.hpp"
#include "at91/aic.hpp"
#include "at91/adc.hpp"
#include "at91/debug.hpp"
#include "at91/gpio.hpp"
#include "at91/pio.hpp"
#include "at91/pit.hpp"
#include "at91/pwm.hpp"
#include "at91/spi.hpp"
#include "at91/ssc.hpp"
#include "at91/timer.hpp"
#include "at91/twi.hpp"
#include "at91/uart.hpp"
#include "at91/usb.hpp"
#include "at91/watchdog.hpp"
