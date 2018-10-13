/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
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

#ifndef MODM_SCROLLABLE_TEXT_HPP
#define MODM_SCROLLABLE_TEXT_HPP

#include <stdint.h>

namespace modm
{

	 /**
	 * @brief The ScrollableText class scrolls the text if the
	 *        required space of the string is less than the
	 *        available.
	 *
	 *\author Thorsten Lajewski
	 *\ingroup modm_ui_menu
	 *\warning This class only works in combination with modm::font::FixedWidth5x8
	 *\todo A better implementation is needed.
	 */
	class ScrollableText
	{
	public:
		/**
		 * @param text string which should be displayed on the Graphic Display
		 * @param space number of letters, which can be displayed at once
		 */
		ScrollableText(const char* text, uint16_t space);

		ScrollableText(const ScrollableText& text);

		ScrollableText& operator=(const ScrollableText& text);

		~ScrollableText();

		/**
		 *determines if the text can be displayed without scrolling
		 */
		inline bool
		needsScrolling()
		{
			return length > space;
		}

		/**
		 * @brief getText return the part of the text which should be displayed next
		 *
		 * If the text needs scrolling and the scrolling is not paused, each call to this
		 * function returns another part of the text which is displaced by one letter.
		 *
		 * @return part of the text
		 */
		const char*
		getText();

		/**
		 * @brief toogle the scrolling
		 */
		inline void
		toogle()
		{
			this->paused = !this->paused;
		}

		/**
		 * @brief start scrolling the text
		 */
		inline void
		scroll()
		{
			this->paused = false;
		}

		/**
		 * @brief pause scrolling the text
		 */
		inline void
		pause()
		{
			this->paused = true;
		}

		/**
		 * @brief setToStart getText returns the beginning of the text next time.
		 */
		void
		setToStart();

		/**
		 * @brief isPaused determines if the text is currently scrolling.
		 * @return
		 */
		inline bool
		isPaused()
		{
			return this->paused;
		}

	private:

		bool paused;
		const char* text;
		uint16_t startPosition;
		uint16_t length;
		uint16_t space;
		char* print;

	};
}
#endif // MODM_SCROLLABLE_TEXT_HPP
