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

#ifndef XPCC__LCD_HPP
#define XPCC__LCD_HPP

#include <stdint.h>
#include <xpcc/communication/io/iodevice.hpp>

namespace xpcc
{
	/**
	 * \brief	Base class for alphanumeric LCDs (liquid crystal display)
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
	 * \ingroup	driver
	 */
	class Lcd : public IODevice
	{
	public:
		// TODO: we need to find a suitable subset for all LCDs here!
		typedef enum {
			CLEAR_DISPLAY,
			CURSOR_HOME,
			CURSOR_ON,
			CURSOR_OFF,
			CURSOR_BLINK,
		} Command;
		
	public:
		/// Constructor
		Lcd();
		
		/// Initialize the display
		virtual void
		initialize() = 0;
		
		/**
		 * \brief	Write a character
		 * 
		 * This method provides an automatic wrap-round if the output reaches
		 * the end of the current line or a newline character is detected.
		 * 
		 * Use putRaw() if this behavior is not wanted.
		 */
		virtual void
		put(char c);
		
		// import the other versions of put() from IODevice
		using IODevice::put;
		
		/**
		 * \brief	Write a raw character at cursor position
		 * 
		 * Unlike put() no further processing will occur.
		 * 
		 * \see	put()
		 */
		virtual void
		putRaw(char c) = 0;
		
		virtual void
		flush();
		
		// TODO we need a better name here!
		//virtual void
		//command(Command command) = 0;
		
		/**
		 * \brief	Set cursor to specified position
		 * 
		 * \param	line	vertical position
		 * \param	column	horizontal position
		 */
		virtual void
		setPosition(uint8_t line, uint8_t column) = 0;
		
		/**
		 * \brief	Read a character
		 *
		 * Because a LCD is a read-only device this method will always return
		 * \c false.
		 */
		virtual bool
		get(char&)
		{
			return false;
		}
		
	protected:
		uint8_t column;
		uint8_t line;
	};
}

#endif // XPCC__LCD_HPP
