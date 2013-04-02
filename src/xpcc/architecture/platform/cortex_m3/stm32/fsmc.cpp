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

#include "fsmc.hpp"

#if defined (STM32F10X_HD) || defined (STM32F10X_XL) || \
	defined (STM32F10X_HD_VL) || \
	defined(STM32F2XX) || defined(STM32F4XX)

// ----------------------------------------------------------------------------
/*
 * - Byte select lanes (NBL[1:0]) are not used during read access (they are
 *   kept low). Read access is always 16 bit!
 * - Output signal change on the rising edge of HCLK
 * - In synchronous mode the output data changes on the falling edge of CLK
 */
void
xpcc::stm32::Fsmc::initialize()
{
	//RCC->AHB3RSTR |= RCC_AHB3RSTR_FSMCRST;
	
	enable();
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::fsmc::NorSram::resetRegion(Region region)
{
	if (region == CHIP_SELECT_1)
	{
		// FSMC_Bank1_NORSRAM1
		FSMC_Bank1->BTCR[region*2] = 0x000030DB;   
	}
	else
	{
		// FSMC_Bank1_NORSRAM2,  FSMC_Bank1_NORSRAM3 or FSMC_Bank1_NORSRAM4
		FSMC_Bank1->BTCR[region*2] = 0x000030D2;
	}
	
	FSMC_Bank1->BTCR[region*2 + 1] = 0x0FFFFFFF;
	FSMC_Bank1E->BWTR[region] = 0x0FFFFFFF;
}

void
xpcc::stm32::fsmc::NorSram::configureSynchronousRegion(Region region,
		BusType multiplex, MemoryType memoryType, SynchronousTiming timing)
{
	// NOR/PSRAM chip-select control register (BCR)
	FSMC_Bank1->BTCR[region*2] =
			FSMC_BCR1_CBURSTRW |
			FSMC_BCR1_EXTMOD |	// Enable extended mode, this allows different timings for read and write
			FSMC_BCR1_WREN |	// enable write
			FSMC_BCR1_BURSTEN | // synchronous access
			(1 << 7) |			// FWPRLVL (unknown, should be kept at reset level (=1)) 
			memoryType |
			multiplex;
	
	if (memoryType == NOR) {
		// Flash access enable
		FSMC_Bank1->BTCR[region*2] |= FSMC_BCR1_FACCEN;
	}
	
	if (memoryType == PSRAM) {
		// see RM0090 page 1259:
		// PSRAMs (CRAMs) have a variable latency due to internal refresh.
		// Therefore these memories issue the NWAIT signal during the whole
		// latency phase to prolong the latency as needed.
		// With PSRAMs (CRAMs) the filed DATLAT must be set to 0, so that
		// the FSMC exits its latency phase soon and starts sampling NWAIT
		// from memory, then starts to read or write when the memory is ready.
		// 
		// This method can be used also with the latest generation of
		// synchronous Flash memories that issue the NWAIT signal, unlike
		// older Flash memories (check the datasheet of the specific Flash
		// memory being used).
		timing.dataLatency = 0;
		
		// TODO NWait
	}
	
	// SRAM/NOR-Flash timing register (BTR) (for read access)
	FSMC_Bank1->BTCR[region*2 + 1] =
			timing.dataLatency << 24 |
			timing.clockDivideRatio << 20 |
			timing.busTurnAround << 16;
	
	// SRAM/NOR-Flash write timing registers (BWTR)
	// TODO read write timing different?
	FSMC_Bank1E->BWTR[region] =
			timing.dataLatency << 24 |
			timing.clockDivideRatio << 20 |
			timing.busTurnAround << 16;
}

void
xpcc::stm32::fsmc::NorSram::configureAsynchronousRegion(Region region,
		BusType busType, MemoryType memoryType, AccessMode accessMode,
		AsynchronousTiming timing)
{
	// NOR/PSRAM chip-select control register (BCR)
	FSMC_Bank1->BTCR[region*2] =
	//		FSMC_BCR1_EXTMOD |	// Enable extended mode, this allows different timings for read and write
			FSMC_BCR1_WREN |	// enable write
			(1 << 7) |			// FWPRLVL (unknown, should be kept at reset level (=1)) 
			memoryType |
			busType;
	
	// SRAM/NOR-Flash timing register (BTR) (for read access)
	FSMC_Bank1->BTCR[region*2 + 1] = 
			accessMode |
			timing.busTurnAround << 16 |
			timing.readDataPhase << 8 |
			timing.readAddressHold << 4 |
			timing.readAddressSetup;
	
	// SRAM/NOR-Flash write timing registers (BWTR)
	FSMC_Bank1E->BWTR[region] = 
			accessMode |
			timing.busTurnAround << 16 |
			timing.writeDataPhase << 8 |
			timing.writeAddressHold << 4 |
			timing.writeAddressSetup;
}

#endif
