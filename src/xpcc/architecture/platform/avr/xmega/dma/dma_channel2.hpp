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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATXMEGA__DMA_CHANNEL2_HPP
#define XPCC_ATXMEGA__DMA_CHANNEL2_HPP

#include <avr/io.h>
#include <stdint.h>
#include <xpcc/architecture/platform/avr/xmega.hpp>
#include <xpcc/architecture/driver/atomic/lock.hpp>
#include "../dma.hpp"

#if defined(DMA_CH2_CTRLA) || defined(__DOXYGEN__)

namespace xpcc
{
	namespace xmega
	{
		/**
		 * \brief		DMA Channel 2
		 * 
		 * The channel gets disabled after a transaction is completed, so you
		 * might have to manually enable it again.
		 * You should read the datasheet chapter on the DMA Controller, and
		 * also <a href="http://blog.world3.net/2011/11/using-the-xmega-dma-controller/">
		 * this blogpost</a>.
		 * 
		 * \author		Niklas Hauser
		 * \ingroup		xmega_dma
		 */
		class DmaChannel2
		{
		public:
			/// Resets and enables the module, sets Repeat count to 1.
			static void
			initialize(dma::BurstLength burstlength=dma::BURST_1BYTE,
					   dma::TriggerSource triggerSource=dma::TRIGGER_SOFTWARE,
					   dma::Reload sourceReload = dma::RELOAD_NONE,
					   dma::AddressMode sourceAddressMode = dma::ADDRESS_FIXED,
					   dma::Reload destinationReload = dma::RELOAD_NONE,
					   dma::AddressMode destinationAddressMode = dma::ADDRESS_FIXED)
			{
				reset();
				// configure DMA channel
				DMA_CH2_CTRLA = DMA_CH_ENABLE_bm | burstlength;
				DMA_CH2_ADDRCTRL = (sourceReload << 6) | (sourceAddressMode << 4) | (destinationReload << 2) | destinationAddressMode;
				DMA_CH2_TRIGSRC = triggerSource;
				DMA_CH2_REPCNT = 1;
				DMA_CH2_TRFCNT = 1;
			}
			
			
			// MARK: CTRLA
			inline static void
			enable(bool enable=true)
			{
				DMA_CH2_CTRLA = (DMA_CH2_CTRLA & ~DMA_CH_ENABLE_bm) | (enable ? DMA_CH_ENABLE_bm : 0);
			}
			
			/// performs a software reset of the module and clears all registers
			static void
			reset()
			{
				DMA_CH2_CTRLA = 0;
				DMA_CH2_CTRLA = DMA_CH_RESET_bm;
				while (DMA_CH2_CTRLA & DMA_CH_RESET_bm) ;
			}
			
			inline static void
			enableRepeatMode(bool enable=true)
			{
				DMA_CH2_CTRLA = (DMA_CH2_CTRLA & ~DMA_CH_REPEAT_bm) | (enable ? DMA_CH_REPEAT_bm : 0) | DMA_CH_ENABLE_bm;
			}
			
			inline static void
			requestTransfer()
			{
				DMA_CH2_CTRLA |= DMA_CH_TRFREQ_bm;
			}
			
			/// Select the trigger to start a burst transfer instead of a block transfer
			inline static void
			setSingleShotMode(bool enable=true)
			{
				DMA_CH2_CTRLA = (DMA_CH2_CTRLA & ~DMA_CH_SINGLE_bm) | (enable ? DMA_CH_SINGLE_bm : 0);
			}
			
			inline static void
			setBurstLength(dma::BurstLength length=dma::BURST_1BYTE)
			{
				DMA_CH2_CTRLA = (DMA_CH2_CTRLA & ~DMA_CH_BURSTLEN_gm) | length;
			}
			
			// MARK: CTRLB
			inline static uint8_t
			getStatus()
			{
				return DMA_CH2_CTRLB & 0xf0;
			}
			
			inline static uint8_t
			getAndClearStatus()
			{
				uint8_t status = DMA_CH2_CTRLB & 0xf0;
				DMA_CH2_CTRLB |= status;
				return status;
			}
			
			inline static void
			setErrorInterruptLevel(InterruptLevel level=INTERRUPT_LEVEL_OFF)
			{
				DMA_CH2_CTRLB = (DMA_CH2_CTRLB & ~DMA_CH_ERRINTLVL_gm) | (level << 2);
			}
			
			inline static void
			setTransactionCompleteInterruptLevel(InterruptLevel level=INTERRUPT_LEVEL_OFF)
			{
				DMA_CH2_CTRLB = (DMA_CH2_CTRLB & ~DMA_CH_TRNINTLVL_gm) | (level << 0);
			}
			
			// MARK: ADDRCTRL
			inline static void
			setSourceAddressReload(dma::Reload reload=dma::RELOAD_NONE)
			{
				DMA_CH2_ADDRCTRL = (DMA_CH2_ADDRCTRL & ~DMA_CH_SRCRELOAD_gm) | (reload << 6);
			}
			
			inline static void
			setSourceAddressMode(dma::AddressMode mode=dma::ADDRESS_FIXED)
			{
				DMA_CH2_ADDRCTRL = (DMA_CH2_ADDRCTRL & ~DMA_CH_SRCDIR_gm) | (mode << 4);
			}
			
			inline static void
			setDestinationAddressReload(dma::Reload reload=dma::RELOAD_NONE)
			{
				DMA_CH2_ADDRCTRL = (DMA_CH2_ADDRCTRL & ~DMA_CH_DESTRELOAD_gm) | (reload << 2);
			}
			
			inline static void
			setDestinationAddressMode(dma::AddressMode mode=dma::ADDRESS_FIXED)
			{
				DMA_CH2_ADDRCTRL = (DMA_CH2_ADDRCTRL & ~DMA_CH_DESTDIR_gm) | (mode << 0);
			}
			
			// MARK: TRIGSRC
			inline static void
			setTriggerSource(dma::TriggerSource source=dma::TRIGGER_SOFTWARE)
			{
				DMA_CH2_TRIGSRC = source;
			}
			
			// MARK: TRFCNT
			/// set how many bytes are transferred on one block
			inline static void
			setBlockTransferCount(uint16_t count)
			{
				xpcc::atomic::Lock lock;
				DMA_CH2_TRFCNT = count;
			}
			
			// MARK: REPCNT
			/// set how many blocks will be transferred, set to 0 for continuous transfer
			inline static void
			setRepeatCount(uint8_t count=1)
			{
				DMA_CH2_REPCNT = count;
			}
			
			// MARK: SRCADDR
			inline static void
			setSource(register8_t* address)
			{
				xpcc::atomic::Lock lock;
				DMA_CH2_SRCADDR0 = static_cast<uint8_t>(reinterpret_cast<uint16_t>(address));
				DMA_CH2_SRCADDR1 = static_cast<uint8_t>(reinterpret_cast<uint16_t>(address) >> 8);
				DMA_CH2_SRCADDR2 = 0;
			}
			
			inline static void
			setSource(register16_t* address)
			{
				xpcc::atomic::Lock lock;
				DMA_CH2_SRCADDR0 = static_cast<uint8_t>(reinterpret_cast<uint16_t>(address));
				DMA_CH2_SRCADDR1 = static_cast<uint8_t>(reinterpret_cast<uint16_t>(address) >> 8);
				DMA_CH2_SRCADDR2 = 0;
			}
			
			inline static void
			setSource(register32_t* address)
			{
				xpcc::atomic::Lock lock;
				DMA_CH2_SRCADDR0 = static_cast<uint8_t>(reinterpret_cast<uint32_t>(address));
				DMA_CH2_SRCADDR1 = static_cast<uint8_t>(reinterpret_cast<uint32_t>(address) >> 8);
				DMA_CH2_SRCADDR2 = static_cast<uint8_t>(reinterpret_cast<uint32_t>(address) >> 16);
			}
			
			// MARK: DESTADDR
			inline static void
			setDestination(register8_t* address)
			{
				xpcc::atomic::Lock lock;
				DMA_CH2_DESTADDR0 = static_cast<uint8_t>(reinterpret_cast<uint16_t>(address));
				DMA_CH2_DESTADDR1 = static_cast<uint8_t>(reinterpret_cast<uint16_t>(address) >> 8);
				DMA_CH2_DESTADDR2 = 0;
			}
			
			inline static void
			setDestination(register16_t* address)
			{
				xpcc::atomic::Lock lock;
				DMA_CH2_DESTADDR0 = static_cast<uint8_t>(reinterpret_cast<uint16_t>(address));
				DMA_CH2_DESTADDR1 = static_cast<uint8_t>(reinterpret_cast<uint16_t>(address) >> 8);
				DMA_CH2_DESTADDR2 = 0;
			}
			
			inline static void
			setDestination(register32_t* address)
			{
				xpcc::atomic::Lock lock;
				DMA_CH2_DESTADDR0 = static_cast<uint8_t>(reinterpret_cast<uint32_t>(address));
				DMA_CH2_DESTADDR1 = static_cast<uint8_t>(reinterpret_cast<uint32_t>(address) >> 8);
				DMA_CH2_DESTADDR2 = static_cast<uint8_t>(reinterpret_cast<uint32_t>(address) >> 16);
			}
			
			inline static DMA_CH_t&
			getChannelRegister()
			{
				return DMA.CH2;
			}
		};
	}
}

#endif	// DMA_CH2_CTRLA
#endif	// XPCC_ATXMEGA__DMA_CHANNEL2_HPP
