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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NOKIA6610_HPP
#define XPCC__NOKIA6610_HPP

#include <xpcc/architecture/driver/delay.hpp>

#include <xpcc/ui/display/buffered_graphic_display.hpp>

namespace xpcc
{
	/**
	 * \brief	Driver for Nokia6610 displays
	 * 
	 * Adapted from www.olimex.com
	 *
	 * bool GE12 = true generates driver for GE12 Philips PCF8833 132 x 132 x 3 driver
	 * bool GE12 = false generates driver for GE8 Epson S1D15G10
	 *
	 * GE12 not implemented yet
	 *
	 * \ingroup	lcd
	 */
	template <typename SPI, typename CS, typename Reset, bool GE12>
	class Nokia6610 : public BufferedGraphicDisplay<130, 128>
	{
	public:
		void
		initialize();

		/**
		 * \brief	Update the display with the content of the RAM buffer
		 */
		virtual void
		update();


		/**
		 * \brief	Change display contrast 0..127
		 */
		void
		setContrast(uint8_t contrast);

	private:
		ALWAYS_INLINE void
		lcdSettings(void);

		ALWAYS_INLINE void
		writeSpiCommand(uint16_t data);

		ALWAYS_INLINE void
		writeSpiData(uint16_t data);

		ALWAYS_INLINE void
		writeSpi9Bit(uint16_t data);

		ALWAYS_INLINE void
		writeSpi9BitFlush();

		void
		lcdWrite130x130bmp(void);

		// 9bit spi managing
		uint16_t temp;				//
		uint8_t countValidBits;		// always < 8
	};
}

#include "nokia6610_impl.hpp"

#endif // XPCC__NOKIA6610_HPP
