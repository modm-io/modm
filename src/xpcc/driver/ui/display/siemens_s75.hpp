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

#ifndef XPCC__SIEMENS_S75_HPP
#define XPCC__SIEMENS_S75_HPP

#include <xpcc/architecture/driver/delay.hpp>
#include <xpcc/driver/connectivity/memory_interface/bitbang_memory_interface.hpp>

#include "buffered_graphic_display.hpp"

namespace xpcc
{
	/**
	 * \brief	Driver for Siemens S75/CX75/C81/M75 displays
	 *
	 * The controller in displays manufactured by Alps is a
	 * SOLOMON SYSTECH SSD1286 which can drive color
	 * displays of up to 132 x 176 pixels in 18 bit colour.
	 *
	 * The portrait mode is a bit more 'native' for this display because
	 * xpcc::BufferedGraphicDisplay requests that the vertical resolution is
	 * dividable by 8.
	 *
	 * In portrait mode the connector is at top.
	 * In landscapeLeft  mode the connector is at the left border.
	 * In landscapeRight mode the connector is at the right border.
	 *
	 * \ingroup	lcd
	 */

	enum class Orientation : uint8_t
	{
		Portrait, 				//< Connector top
		LandscapeRight,			//< Connector right
		LandscapeLeft,			//< Connector left
		PortraitUpsideDown,		//< Connector bottom
	};

	template <
		typename MEMORY,
		typename RESET,
		uint8_t  WIDTH,
		uint8_t  HEIGHT,
		xpcc::Orientation ORIENTATION>
	class SiemensS75Common :
			public BufferedGraphicDisplay<WIDTH, HEIGHT>
	{
	public:
		void
		update(void);

		void
		initialize(void);

	protected:
		ALWAYS_INLINE void
		lcdCls(const uint16_t colour);

		ALWAYS_INLINE void
		lcdSettings();
	};

	template <typename MEMORY, typename RESET>
	class SiemensS75Portrait :
		public SiemensS75Common<MEMORY, RESET, 136, 176, xpcc::Orientation::Portrait>
	{
	};

	template <typename MEMORY, typename RESET>
	class SiemensS75PortraitUpsideDown:
		public SiemensS75Common<MEMORY, RESET, 136, 176, xpcc::Orientation::PortraitUpsideDown>
	{
	};

	/**
	 * The display in landscape mode does not match the required
	 * alignment of BufferedGraphicDisplay which requests that
	 * the vertical resolution can be divided by 8.
	 *
	 */
	template <typename MEMORY, typename RESET>
	class SiemensS75LandscapeLeft :
		public SiemensS75Common<MEMORY, RESET, 176, 136, xpcc::Orientation::LandscapeLeft>
	{
	};

	template <typename MEMORY, typename RESET>
	class SiemensS75LandscapeRight :
		public SiemensS75Common<MEMORY, RESET, 176, 136, xpcc::Orientation::LandscapeRight>
	{
	};
}

#include "siemens_s75_impl.hpp"

#endif // XPCC__SIEMENS_S75_HPP
