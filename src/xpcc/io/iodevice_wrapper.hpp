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

#ifndef XPCC__IODEVICE_WRAPPER_HPP
#define XPCC__IODEVICE_WRAPPER_HPP

#include <stdint.h>

#include "iodevice.hpp"

namespace xpcc
{
	/**
	 * \brief		Wrapper to use any peripheral device that supports static
	 * 				put() and get() as an IODevice
	 * 
	 * \tparam		T	Peripheral which should be wrapped
	 * 
	 * Example:
	 * \code
	 * // configure a UART
	 * xpcc::BufferedUart0 uart(9600);
	 * 
	 * // wrap it into an IODevice
	 * xpcc::IODeviceWrapper<xpcc::BufferedUart0> device(uart);
	 * 
	 * // use this device to print a message
	 * device.write("Hello");
	 * 
	 * // or create a IOStream and use the stream to print something
	 * xpcc::IOStream stream(device);
	 * stream << " World!";
	 * \endcode
	 * 
	 * \ingroup		io
	 */
	template<typename T>
	class IODeviceWrapper : public IODevice
	{
	public:
		/**
		 * \brief	Constructor
		 *
		 * \param	device	configured object
		 */
		IODeviceWrapper(const T& device)
		{
			// get rid of the warning about an unused parameter
			(void) device;
		}
		
		virtual void
		write(char c)
		{
			T::write(c);
		}
		
		virtual void
		write(const char *s)
		{
			char c;
			while ((c = *s++)) {
				T::write(static_cast<uint8_t>(c));
			}
		}
		
		virtual void
		flush()
		{
		}
		
		virtual bool
		read(char& c)
		{
			// FIXME
			uint8_t t;
			if (T::read(t)) {
				c = t;
				return true;
			}
			else {
				return false;
			}
		}
	};
}

#endif // XPCC__IODEVICE_WRAPPER_HPP
