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

#ifndef XPCC_ATXMEGA__DMAC_HPP
#define	XPCC_ATXMEGA__DMAC_HPP

#include <stdint.h>
#include <avr/io.h>
#include "dma.hpp"

#if defined(DMA) || defined(__DOXYGEN__)

namespace xpcc
{
	namespace xmega
	{
		/**
		 * \brief		DMA Controller
		 *
		 * You first must enable this module to be able to use any DMA channel!
		 *
		 * \author		Niklas Hauser
		 * \ingroup		xmega_dma
		 */
		class DmaController
		{
		public:
			/**
			 * Resets and enables the DMA Controller.
			 *
			 * \param priorityMode	Priority mode
			 * \param bufferMode	Double buffering mode
			 */
			inline static void
			initialize(dma::ChannelPriority priorityMode=dma::PRIORITY_RR0123,
					   dma::DoubleBuffer bufferMode=dma::DOUBLE_BUFFER_DISABLED)
			{
				reset();
				// configure DMA controller
				DMA_CTRL = DMA_ENABLE_bm | bufferMode | priorityMode;
			}
			
			inline static void
			enable(bool enable=true)
			{
				DMA_CTRL = (DMA_CTRL & ~DMA_ENABLE_bm) | (enable ? DMA_ENABLE_bm : 0);
			}
			
			/// performs a software reset of the module and clears all registers
			inline static void
			reset()
			{
				DMA_CTRL = 0;
				DMA_CTRL = DMA_RESET_bm;
				while (DMA_CTRL & DMA_RESET_bm) ;
			}
			
			inline static void
			setDoubleBufferMode(dma::DoubleBuffer mode=dma::DOUBLE_BUFFER_DISABLED)
			{
				DMA_CTRL = (DMA_CTRL & ~DMA_DBUFMODE_gm) | mode;
			}
			
			inline static void
			setChannelPriorityMode(dma::ChannelPriority mode=dma::PRIORITY_RR0123)
			{
				DMA_CTRL = (DMA_CTRL & DMA_PRIMODE_gm) | mode;
			}
			
			/// \param clear set to true to read and clear the flags
			inline static uint8_t
			getInterruptStatus(bool clear=false)
			{
				uint8_t flags = DMA_INTFLAGS;
				if (clear) DMA_INTFLAGS = flags;
				return flags;
			}
			
			/// these flags cannot be cleared by software, only by hardware
			inline static uint8_t
			getStatus()
			{
				return DMA_STATUS;
			}
			
			inline static DMA_t&
			getModuleRegister()
			{
				return DMA;
			}
		};
	}
}

#endif	// DMA
#endif	// XPCC_ATXMEGA__DMAC_HPP
