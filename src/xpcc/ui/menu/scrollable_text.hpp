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

#ifndef XPCC__SCROLLABLE_TEXT_HPP
#define XPCC__SCROLLABLE_TEXT_HPP

#include <stdint.h>

namespace xpcc
{

	 /**
	 * @brief The ScrollableText class scrolls the text if the
	 *        required space of the string is less than the
	 *        available.
	 *
	 *\author Thorsten Lajewski
	 *\ingroup display_menu
	 *\warning This class only works in combination with xpcc::font::FixedWidth5x8
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
#endif // XPCC__SCROLLABLE_TEXT_HPP
