// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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

#ifndef XPCC__PARALLEL_TFT_HPP
#define XPCC__PARALLEL_TFT_HPP

#include <xpcc/architecture/driver/delay.hpp>
#include <xpcc/driver/ui/display/graphic_display.hpp>

namespace xpcc
{
	/**
	 * 
	 * @author	Fabian Greif
	 */
	struct TftMemoryBus
	{
		TftMemoryBus(volatile uint16_t * index,
				volatile uint16_t * data) :
			ptrIndex(index), ptrData(data)
		{
		}
		
		ALWAYS_INLINE void
		writeIndex(uint16_t index)
		{
			*ptrIndex = index;
		}
		
		ALWAYS_INLINE void
		writeData(uint16_t data)
		{
			*ptrData = data;
		}
		
		ALWAYS_INLINE uint16_t
		readData()
		{
			return *ptrData;
		}
		
		ALWAYS_INLINE void
		writeRegister(uint16_t reg, uint16_t value)
		{
			writeIndex(reg);
			writeData(value);
		}
		
		ALWAYS_INLINE uint16_t
		readRegister(uint16_t reg)
		{
			writeIndex(reg);
			return readData();
		}
		
	private:
		volatile uint16_t * const ptrIndex;
		volatile uint16_t * const ptrData;
	};
	
	/**
	 * TFT display connected to a 16 bit parallel bus
	 * 
	 * Supports (among others) the following displays:
	 * - WaveShare 3,2" TFT (Model B), Controller SSD1289
	 * 
	 * @author	Fabian Greif
	 */
	template <typename INTERFACE>
	class ParallelTft : public GraphicDisplay
	{
	public:
		ParallelTft(INTERFACE& interface);
		
		void
		initialize();
		
		// see GraphicDisplay::getWidth()
		virtual uint16_t
		getWidth() const;
	
		// see GraphicDisplay::getHeight()
		virtual uint16_t
		getHeight() const;
	
		// set GraphicDisplay::clear()
		virtual void
		clear();
		
		/**
		 * Not used here as all operations are performed directly
		 * on the display.
		 */
		virtual void
		update()
		{
		}
		
	private:
		enum class Device
		{
			ILI9320,	// device code = 0x9320
			ILI9325,	// device code = 0x9325
			ILI9328,	// device code = 0x9328
			ILI9331,	// device code = 0x9331
			SSD1298,	// device code = 0x8999
			SSD1289,	// device code = 0x8989
			ST7781,		// device code = 0x7783
			LGDP4531,	// device code = 0x4531
			SPFD5408B,	// device code = 0x5408
			R61505U,	// device code = 0x1505 or 0x0505	   
			//HX8347D,	// device code = 0x0047
			//HX8347A,	// device code = 0x0047	
			LGDP4535,	// device code = 0x4535  
			//SSD2119,	// 3.5 LCD, device code = 0x9919
		};
		
		virtual void
		setPixel(int16_t x, int16_t y);
		
		virtual void
		clearPixel(int16_t x, int16_t y);
		
		virtual bool
		getPixel(int16_t x, int16_t y);
		
		void
		writeCursor(uint16_t x, uint16_t y);
		
		void
		writeRegister(uint16_t reg, uint16_t value);
		
		INTERFACE& interface;
		Device deviceCode;
	};
}

#include "parallel_tft_impl.hpp"

#endif // XPCC__PARALLEL_TFT_HPP
