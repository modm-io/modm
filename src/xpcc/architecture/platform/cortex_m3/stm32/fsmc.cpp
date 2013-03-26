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
		FSMC_Bank1->BTCR[region] = 0x000030DB;   
	}
	else
	{
		// FSMC_Bank1_NORSRAM2,  FSMC_Bank1_NORSRAM3 or FSMC_Bank1_NORSRAM4
		FSMC_Bank1->BTCR[region] = 0x000030D2;
	}
	
	FSMC_Bank1->BTCR[region + 1] = 0x0FFFFFFF;
	FSMC_Bank1E->BWTR[region] = 0x0FFFFFFF;
}

void
xpcc::stm32::fsmc::NorSram::configureSynchronousRegion(Region region,
		Muliplex multiplex, SynchronousTiming timing)
{
	// Bank1 NOR/SRAM control register configuration
	FSMC_Bank1->BTCR[region] = FSMC_BCR1_EXTMOD;
	
	
/*  FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank] = 
            (uint32_t)FSMC_NORSRAMInitStruct->FSMC_DataAddressMux |
            FSMC_NORSRAMInitStruct->FSMC_MemoryType |
            FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth |
            FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode |
            FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait |
            FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity |
            FSMC_NORSRAMInitStruct->FSMC_WrapMode |
            FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive |
            FSMC_NORSRAMInitStruct->FSMC_WriteOperation |
            FSMC_NORSRAMInitStruct->FSMC_WaitSignal |
            FSMC_NORSRAMInitStruct->FSMC_ExtendedMode |
            FSMC_NORSRAMInitStruct->FSMC_WriteBurst;
  if(FSMC_NORSRAMInitStruct->FSMC_MemoryType == FSMC_MemoryType_NOR)
  {
    FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank] |= (uint32_t)BCR_FACCEN_SET;
  }*/
}

void
xpcc::stm32::fsmc::NorSram::configureAsynchronousRegion(Region region,
		Muliplex multiplex, AsynchronousTiming timing)
{
	
}
