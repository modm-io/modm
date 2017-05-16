/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_VIRTUAL_GRAPHIC_DISPLAY
#define MODM_VIRTUAL_GRAPHIC_DISPLAY

#include <modm/ui/display/graphic_display.hpp>

namespace modm
{
	/// @ingroup	graphics
	class VirtualGraphicDisplay:
			public modm::GraphicDisplay
	{
	public:
		VirtualGraphicDisplay(modm::GraphicDisplay* display,
				modm::glcd::Point leftUpper, modm::glcd::Point rightLower);

		void
		setDisplay(modm::GraphicDisplay* display);

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

		virtual void
		clear();

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
		modm::GraphicDisplay* display;
		modm::glcd::Point leftUpper;
		modm::glcd::Point rightLower;
		const uint16_t width;
		const uint16_t height;
	};


}

#endif //MODM_VIRTUAL_GRAPHIC_DISPLAY
