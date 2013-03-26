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

#include "device.h"

namespace xpcc
{
	namespace stm32
	{
		namespace fsmc
		{
			/**
			 * Configure NOR-Flash or (P)SRAM
			 * 
			 * NOR non-multiplexed
			 * - CLK
			 * - A[25:0]
			 * - D[15:0]
			 * - NE[x]
			 * - NOE
			 * - NWE
			 * - NL(=NADV)
			 * - NWAIT
			 * 
			 * NOR multiplexed
			 * - A[25:16]
			 * - AD[15:0]
			 * 
			 * (P)SRAM
			 * - NBL[1]		- Upper byte enable (NUB)
			 * - NBL[0]		- Lower byte enable (NLB)
			 * 
			 * Options:
			 * - multiplexed, non-multiplexed
			 * - asynchronous, synchronous
			 * 
			 * Mode 1 (SRAM/CRAM)
			 * - NOE directly low
			 * - NWE after address setup
			 * - Timings: address setup, data phase
			 * 
			 * Mode A (CRAM)
			 * - NOE, NWE low after address setup
			 * - Timings: address setup, data phase
			 * 
			 * Mode 2/B (NOR Flash)
			 * - NADV low during address setup
			 * - NOE directly low
			 * - Mode 2: NWE low after address setup
			 * - Mode B: NWE directly low
			 * 
			 * Mode C (NOR Flash)
			 * - NADV low during address setup
			 * - NOE, NWE low after address setup
			 * 
			 * Mode D
			 * - Mode C with additional address hold step (NADV, NOE, NWE high)
			 * - Timings: address setup, address hold, data phase
			 * 
			 * Mode muxed
			 * - Mode D with lower address bits on data lanes during
			 *   address setup step
			 * 
			 * Async wait: only data phase stage is prolonged, address setup
			 * and address hold are not affected.
			 * 
			 * Memory Type (MTYP):
			 * - 00 SRAM/ROM 					(asynchronous, non-multiplexed)
			 * - 01 PSRAM/Cellular RAM: CRAM	((a)synchronous read/write, (non-)multiplexed)
			 * - 10 NOR Flash/One NAND Flash	((a)synchronous read, (non-)multiplexed)
			 * 
			 * Multiplexed mode only valid for NOR and PSRAM
			 * 
			 * Synchronous => burst mode?
			 * NOR Flash and burst write?
			 */
			class NorSram
			{
			public:
				/**
				 * Corresponds to NE1..4.
				 */
				enum Region
				{
					CHIP_SELECT_1 = 0,
					CHIP_SELECT_2 = 1,
					CHIP_SELECT_3 = 2,
					CHIP_SELECT_4 = 3,
				};
				
				enum WaitPolarity
				{
					WAIT_HIGH_ACTIVE,
					WAIT_LOW_ACTIVE,
				};
				
				enum Muliplex
				{
					NO_MULTIPLEX,
					ADDRESS_DATA_MULIPLEX,
				};
				
				/**
				 * 
				 */
				struct AsynchronousTiming
				{
					uint8_t readAddressSetup;	///< 0..15
					uint8_t readAddressHold;	///< 1..15
					uint8_t readDataPhase;		///< 1..256
					
					uint8_t writeAddressSetup;	///< 0..15
					uint8_t writeAddressHold;	///< 1..15
					uint8_t writeDataPhase;		///< 1..256
					
					/**
					 * Bus turn around time
					 * 
					 * Time from NEx high to NEx low => time between two bus
					 * accesses (0..15 HCLK cycles).
					 * 
					 * TODO read/write?
					 */
					uint8_t busTurnAround;
				};
				
				struct SynchronousTiming
				{
					uint8_t busTurnAround;		///< 0..15
					/** Number of HCLK cycles for one CLK cycle, [1..16] */
					uint8_t clockDivideRatio;
					uint8_t dataLatency;		///< 2..17 CLK cycles
				};
				
			public:
				static void
				resetRegion(Region region);
				
				static inline void
				enableRegion(Region region, bool enable)
				{
					if (enable) {
						FSMC_Bank1->BTCR[region * 2] |= FSMC_BCR1_MBKEN;
					}
					else {
						FSMC_Bank1->BTCR[region * 2] &= ~FSMC_BCR1_MBKEN;
					}
					
				}
				
				static void
				configureSynchronousRegion(Region region,
						Muliplex multiplex,
						SynchronousTiming timing);
				
				static void
				configureAsynchronousRegion(Region region,
						Muliplex multiplex,
						AsynchronousTiming timing);
				
				static inline void
				enableAsynchronousWait(Region region,
						WaitPolarity polarity = WAIT_LOW_ACTIVE)
				{
					uint32_t btcr = FSMC_BCR1_ASYNCWAIT; 
					if (polarity != WAIT_LOW_ACTIVE) {
						btcr |= FSMC_BCR1_WAITPOL;
					}
					FSMC_Bank1->BTCR[region] = btcr;
				}
				
				/**
				 * Get mapped memory address of a region.
				 * 
				 * Each region is 64 MB in size.
				 */
				static inline void*
				getRegionPointer(Region region) {
					void *ptr;
					switch (region) {
						case CHIP_SELECT_1: ptr = (void *)(0x60000000); break;
						case CHIP_SELECT_2: ptr = (void *)(0x64000000); break;
						case CHIP_SELECT_3: ptr = (void *)(0x68000000); break;
						case CHIP_SELECT_4: ptr = (void *)(0x6C000000); break;
					}
					return ptr;
				}
			};
			
			class FsmcNand
			{
				// TODO
			};
			
			class FsmcPcCard
			{
				// TODO
			};
		}
		
		/**
		 * Flexible static memory controller.
		 * 
		 * Memory Map:
		 * 0x6000 0000 - 0x6fff ffff : Bank 1 4x NOR or [P]SRAM devices
		 * 0x7000 0000 - 0x7fff ffff : Bank 2 NAND Flash device
		 * 0x8000 0000 - 0x8fff ffff : Bank 3 NAND Flash device
		 * 0x9000 0000 - 0x9fff ffff : Bank 4 PC Card device
		 *   each bank is 4x64 = 256 MB wide
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
			/**
			 * Reset all NAND/NOR/SRAM/... settings
			 */
			static void
			initialize();
			
		private:
			static inline void
			enable()
			{
				RCC->AHB3ENR |= RCC_AHB3ENR_FSMCEN;
			}
			
			static inline void
			disable()
			{
				RCC->AHB3ENR &= ~RCC_AHB3ENR_FSMCEN;
			}
		};
	}
}

#endif	// XPCC_STM32__FSMC_HPP
