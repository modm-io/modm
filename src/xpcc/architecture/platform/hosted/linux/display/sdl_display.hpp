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
#ifndef XPCC__SDL_DISPLAY_HPP
#define XPCC__SDL_DISPLAY_HPP

#include <xpcc/driver/ui/display/graphic_display.hpp>
#include <SDL/SDL.h>

namespace xpcc
{

     /**
     * @brief The SDLDisplay class provides a visualisaiton of
     *        the GraphicDisplay on the PC. It can be used to
     *        test the code without the need of any hardware.
     *
     * \author Thorsten Lajewski
     *
     * \warning SDL_Init( SDL_INIT_EVERYTHING ); needs to be  called, before
     *          any instance of the SDLDisplay can be used
     */
	class SDLDisplay: public xpcc::GraphicDisplay
	{
	public:
		SDLDisplay(int16_t width, int16_t height);

		~SDLDisplay();

		virtual inline uint16_t
		getWidth() const
		{
			return this->width;
		}

		virtual inline uint16_t
		getHeight() const
		{
			return this->height;
		}

		inline virtual void clear()
		{
			this->clearWholeScreen();
		}

		virtual void
		update();

	protected:

		virtual void
		setPixel(int16_t x, int16_t y);

		virtual void
		clearPixel(int16_t x, int16_t y);

		virtual bool
		getPixel(int16_t x, int16_t y);

 	private:

		void setPixelOnScreen(int16_t x, int16_t y, bool value);

		void clearWholeScreen();

		const int16_t width;
		const int16_t height;

		// true means black, false means backgroundcolor
		bool* memory;
		SDL_Surface* screen;

	};
}
#endif /* XPCC__SDL_DISPLAY_HPP */
