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

#ifndef XPCC__CHARACTER_DISPLAY_HPP
#define XPCC__CHARACTER_DISPLAY_HPP

#include <stdint.h>
#include <xpcc/io/iostream.hpp>

namespace xpcc
{
	/**
	 * \brief	Base class for alpha-numeric LCDs (liquid crystal display)
	 * 
	 * <h3>About this implementation:</h3> 
	 * 
	 * In contrast to other drivers which use static methods,
	 * this class is directly derived from IODevice with all the virtual
	 * methods because it will be most likely used to create a IOStream
	 * which then handles the output to the display.
	 * 
	 * Therefore creating this class with static methods and using a wrapper
	 * class to create a IODevice seems unnecessary complicated.
	 * 
	 * \author	Fabian Greif
	 * \ingroup	lcd
	 */
	class CharacterDisplay : public IOStream
	{
	public:
		// TODO: we need to find a suitable subset for all LCDs here!
		enum Command
		{
			CLEAR_DISPLAY,
			CURSOR_HOME,
			CURSOR_ON,
			CURSOR_OFF,
			CURSOR_BLINK,
		};
		
	public:
		/// Constructor
		CharacterDisplay();
		
		/// Initialize the display
		virtual void
		initialize() = 0;
		
		/**
		 * \brief	Write a character
		 * 
		 * This method provides an automatic wrap-round if the output reaches
		 * the end of the current line or a newline character is detected.
		 * 
		 * Use writeRaw() if this behavior is not wanted.
		 */
		void
		write(char c);
		
		/**
		 * \brief	Write a raw character at cursor position
		 * 
		 * Unlike write() no further processing will occur.
		 * 
		 * \see	write()
		 */
		virtual void
		writeRaw(char c) = 0;
		
		virtual void
		command(Command command) = 0;
		
		/**
		 * \brief	Set cursor to specified position
		 * 
		 * \param	line	vertical position
		 * \param	column	horizontal position
		 */
		virtual void
		setPosition(uint8_t line, uint8_t column) = 0;
		
	protected:
		// Interface class for the IOStream
		class Writer : public IODevice
		{
		public:
			Writer(CharacterDisplay *parent) :
				parent(parent)
			{
			}
			
			/// Draw a single character
			virtual void
			write(char c);
			
			using IODevice::write;
			
			/// unused
			virtual void
			flush();
			
			/// unused, returns always \c false
			virtual bool
			read(char& c);
			
		private:
			CharacterDisplay *parent;
		};
		
	protected:
		Writer writer;
		
		uint8_t column;
		uint8_t line;
	};
}

#endif // XPCC__CHARACTER_DISPLAY_HPP
