// coding: utf-8
/* Copyright (c) 2012-2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#include "fsmc.hpp"

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
		AsynchronousTiming timing, ExtendedMode extended)
{
	// dirty hack to make "Muxed Mode" (reference manual p. 1340) work
	uint8_t FACCEN = (	busType == ADDRESS_DATA_MULIPLEX_16BIT &&
						memoryType == NOR && accessMode == MODE_A)?
						(1 << 6) : 0;
	// NOR/PSRAM chip-select control register (BCR)
	FSMC_Bank1->BTCR[region*2] =
			static_cast<uint32_t>(extended) |	// Enable extended mode, this allows different timings for read and write
			FSMC_BCR1_WREN |	// enable write
			FACCEN |
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
