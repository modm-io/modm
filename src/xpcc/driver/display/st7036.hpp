/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ST7036_HPP
#define MODM_ST7036_HPP

#include <modm/ui/display/character_display.hpp>

namespace modm
{
	/**
	 * \brief	Driver for ST7036 based LC-displays
	 *
	 * \todo	make this class adaptable to other line counts!
	 * \todo	documentation
	 *
	 * Available defines:
	 * - ST7036_VOLTAGE = 3|5
	 *
	 * \see		Lcd
	 *
	 * \author	Fabian Greif
	 * \ingroup	driver_display
	 */
	template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
	class St7036 : public CharacterDisplay
	{
	public:
		/// Constructor
		St7036();

		/**
		 * \brief	Initialize the display
		 *
		 * The display needs some time to initialize after startup. You have
		 * to wait at least 50 ms until calling this method.
		 */
		virtual void
		initialize();

		virtual void
		writeRaw(char c);

		virtual void
		execute(Command command);

		virtual void
		setCursor(uint8_t column, uint8_t line);

		// TODO
		//void
		//setContrast();

	protected:
		void
		writeCommand(uint8_t command);
	};
}

#include "st7036_impl.hpp"

#endif // MODM_ST7036_HPP
