// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__XMEGA_SPI_HPP
#define XPCC__XMEGA_SPI_HPP

#include <avr/io.h>
#include <stdint.h>

namespace xpcc
{


//SPIC.CTRL = SPI_CLK2X_bm // double speed
//			| SPI_ENABLE_bm
//				| SPI_DORD_bm  // lsb first
//				| SPI_MASTER_bm  // master mode
//				| SPI_MODE_x_gc  // timing / transfair mode slect
//				| SPI_PRESCALER_DIVxx_gc  // no effect in slave mode
//			;
//	SPIC.INTCTRL = SPI_INTLVL_OFF_gc;  // interrupt level configuration
//	SPIC.STATUS | SPI_IF_bm | SPI_WRCOL_bm interrupt flags

/**
 * \ingroup	architecture
 * \brief	Create a spi module
 */
#define	CREATE_SPI_MODULE(name, moduleBase) \
	struct name { \
		ALWAYS_INLINE static SPI_t &getModuleBase() { return moduleBase; } \
		ALWAYS_INLINE static void setEnable(bool enable) { moduleBase.CTRL = (moduleBase.CTRL & SPI_ENABLE_bm) | (enable?SPI_ENABLE_bm:0); } \
		ALWAYS_INLINE static void setMaster(bool master) { moduleBase.CTRL = (moduleBase.CTRL & SPI_MASTER_bm) | (master?SPI_MASTER_bm:0); } \
		ALWAYS_INLINE static void setPrescaler(SPI_PRESCALER_t prescaler) { moduleBase.CTRL = (moduleBase.CTRL & SPI_PRESCALER_gm) | prescaler; } \
		ALWAYS_INLINE static void setDoubleSpeed(bool doubleSpeed) { moduleBase.CTRL = (moduleBase.CTRL & SPI_CLK2X_bm) | (doubleSpeed?SPI_CLK2X_bm:0); } \
		ALWAYS_INLINE static void setDataOrder(bool LSBFirst) { moduleBase.CTRL = (moduleBase.CTRL & SPI_DORD_bm) | (LSBFirst?SPI_DORD_bm:0); } \
		ALWAYS_INLINE static void setMode(SPI_MODE_t mode) { moduleBase.CTRL = (moduleBase.CTRL & SPI_MODE_gm) | mode; } \
		ALWAYS_INLINE static void enableInterrupt(SPI_INTLVL_t level) { moduleBase.INTCTRL = (moduleBase.CTRL & SPI_INTLVL_gm) | level; } \
		ALWAYS_INLINE static bool isInterrupt() { bool flag = moduleBase.STATUS & SPI_IF_bm; moduleBase.STATUS |= SPI_IF_bm; return flag; } \
		\
		static uint8_t \
		put(uint8_t data) { \
			moduleBase.DATA = data; \
			while (! (moduleBase.STATUS & SPI_IF_bm)); \
			return moduleBase.DATA; \
		} \
		\
		static void \
		initialize(SPI_MODE_t mode, SPI_PRESCALER_t prescaler) { \
			moduleBase.CTRL = SPI_ENABLE_bm \
							| SPI_CLK2X_bm \
							| SPI_MASTER_bm \
							| mode \
							| prescaler \
							; \
		} \
		\
		static void \
		initialize(SPI_MODE_t mode) { \
			moduleBase.CTRL = SPI_ENABLE_bm \
							| mode \
							; \
		} \
	};

}

#endif // XPCC__XMEGA_SPI_HPP
