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

#ifndef XPCC_STM32__FSMC_HPP
#define XPCC_STM32__FSMC_HPP

#include <stdint.h>

namespace xpcc
{
	namespace stm32
	{
		/**
		 * Flexible static memory controller.
		 * 
		 * Memory Map:
		 * 0x6000 0000 - 0x6fff ffff : Bank 1 4 NOR or [P]SRAM devices
		 * 0x7000 0000 - 0x7fff ffff : Bank 2 NAND Flash device
		 * 0x8000 0000 - 0x8fff ffff : Bank 3 NAND Flash device
		 * 0x9000 0000 - 0x9fff ffff : Bank 4 PC Card device
		 * 
		 * NOR/PSRAM/SRAM Flash signals:
		 * CLK       - Clock (for synchronous burst)
		 * A[25:0]   - Address Bus
		 * AD[15:0]  - 16-bit multiplexed, bidirectional address/data bus or data bus
		 * NE[x]     - Chip select, x=1..4
		 * NOE       - Output enable
		 * NWE       - Write enable
		 * NL(=NADV) - Latch enable (this signal is called address valid,
		 *             NADV, by some NOR Flash devices)
		 * NWAIT     - NOR Flash wait input signal to the FSMC
		 * NBL1      - Upper byte enable (memory signal name: NUB)
		 * NBL0      - Lowed byte enable (memory signal name: NLB)
		 * 
		 * 
		 * \author	Fabian Greif
		 */
		class Fsmc
		{
		public:
			
		};
	}
}

#endif	// XPCC_STM32__FSMC_HPP
