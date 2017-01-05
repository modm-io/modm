/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2014, Daniel Krebs
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SDL_DISPLAY_HPP
#define MODM_SDL_DISPLAY_HPP

#include <modm/ui/display.hpp>
#include <modm/debug/logger.hpp>
#include <SDL/SDL.h>

namespace modm
{
	class SDLDisplay: public modm::GraphicDisplay
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

		void setPixelOnScreen(int16_t x, int16_t y);

		void clearWholeScreen();

		inline uint8_t
		colorToRed(modm::glcd::Color& color);

		inline uint8_t
		colorToGreen(modm::glcd::Color& color);

		inline uint8_t
		colorToBlue(modm::glcd::Color& color);


		const int16_t width;
		const int16_t height;

		uint16_t* memory;
		SDL_Surface* screen;

	};
}
#endif /* MODM_SDL_DISPLAY_HPP */
