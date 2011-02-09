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


#ifndef XPCC__XMEGA_SPI_F_HPP
#define XPCC__XMEGA_SPI_F_HPP

#include <avr/io.h>
#include <stdint.h>

#include <xpcc/architecture/platform.hpp>

namespace xpcc
{
	/**
	 * \brief		SPI Master for Port F
	 * 
	 * 
	 * \ingroup		atxmega_spi
	 */
	class SpiMasterF
	{
	public:
		static void
		initialize(SPI_PRESCALER_t prescaler, SPI_MODE_t mode = SPI_MODE_0_gc);
		
		static uint8_t
		write(uint8_t data);
		
		ALWAYS_INLINE static SPI_t&
		getModuleBase()
		{
			return SPIF;
		}
		
		ALWAYS_INLINE static void
		setEnable(bool enable)
		{
			SPIF_CTRL = (SPIF_CTRL & ~SPI_ENABLE_bm) | (enable ? SPI_ENABLE_bm : 0);
		}
		
		ALWAYS_INLINE static void
		setMaster(bool master)
		{
			SPIF_CTRL = (SPIF_CTRL & ~SPI_MASTER_bm) | (master ? SPI_MASTER_bm : 0);
		}
		
		ALWAYS_INLINE static void
		setPrescaler(SPI_PRESCALER_t prescaler)
		{
			SPIF_CTRL = (SPIF_CTRL & ~SPI_PRESCALER_gm) | prescaler;
		}
		
		ALWAYS_INLINE static void
		setDoubleSpeed(bool doubleSpeed)
		{
			SPIF_CTRL = (SPIF_CTRL & ~SPI_CLK2X_bm) | (doubleSpeed ? SPI_CLK2X_bm : 0);
		}
		
		ALWAYS_INLINE static void
		setDataOrder(bool LsbFirst)
		{
			SPIF_CTRL = (SPIF_CTRL & ~SPI_DORD_bm) | (LsbFirst ? SPI_DORD_bm : 0);
		}
		
		ALWAYS_INLINE static void
		setMode(SPI_MODE_t mode)
		{
			SPIF_CTRL = (SPIF_CTRL & ~SPI_MODE_gm) | mode;
		}
	};
}

#endif // XPCC__XMEGA_SPI_F_HPP
