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
 * $Id: iostream.hpp 84 2009-10-16 19:15:12Z thundernail $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__IOSTREAM_HPP
#define XPCC__IOSTREAM_HPP

#include <stdint.h>
#include <xpcc/hal/flash/flash_pointer.hpp>

#include "iodevice.hpp"

namespace xpcc
{
	/**
	 * @class 	IOStream
	 * @brief 	This Formats all primary typs into a string stream for
	 * 			output or it reads values from a input and converts them to
	 * 			a given type;
	 *
	 * @ingroup io
	 * @version	$Id: iostream.hpp 84 2009-10-16 19:15:12Z thundernail $
	 * @author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
	 */
	class IOStream
	{
		public :

			//! Constructor
			//! \param	device	device to write the stream to
			//! \code
			//!		MyIODevice device;
			//!		IOStream stream( device );
			//! \endcode
			IOStream(IODevice& device);

			inline IOStream&
			put(char c)
			{
				this->device->put(c);
				return *this;
			}

			inline IOStream&
			flush()
			{
				this->device->flush();
				return *this;
			}

			template<typename T>
			inline IOStream&
			operator<< ( const T& c );

			inline IOStream&
			operator<< ( IOStream& (*function)(IOStream&) )
			{
				return function( *this );
			}

		protected :
			friend struct StringWriter;

			template <typename T>
			friend struct IntegerWriter;

			template <typename T>
			friend struct FloatWriter;
			
			IOStream&
			putInteger(int8_t value);
			
			IOStream&
			putInteger(uint8_t value);
			
			IOStream&
			putInteger(int16_t value);
			
			IOStream&
			putInteger(uint16_t value);
			
#ifdef __AVR__
			IOStream&
			putInteger(int32_t value);
#endif
			
			// default version which is used all the others didn't match
			template<typename T>
			IOStream&
			putInteger( T value );
			
			template<typename T>
			IOStream&
			putFloat( T value );

		private :
			IOStream(const IOStream& );

			IOStream&
			operator =(const IOStream& );

			IODevice* const	device;

	};

	/**
	 *  @brief  Write a newline and flush the stream.
	 *
	 *  This manipulator is often mistakenly used when a simple newline is
	 *  desired, leading to poor buffering performance.  See
	 *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt11ch25s02.html for more
	 *  on this subject.
	 */
	inline IOStream&
	endl(IOStream& ios);

	/**
	 *  @brief  Flushes the output stream.
	 *
	 *  This manipulator simply calls the stream's @c flush() member function.
	*/
	inline IOStream&
	flush(IOStream& ios);

	struct StringWriter {
		inline void
		operator()(IOStream& os, const char* s) const
		{
			os.device->put( s );
		}

		inline void
		operator()(IOStream& os, char c) const
		{
			os.device->put( c );
		}
		
		inline void
		operator()(IOStream& os, FlashPointer<char> str) const
		{
			char c;
			while ((c = *str++)) {
				os.device->put(c);
			}
		}
	};

	template <typename T>
	struct IntegerWriter {
		inline void operator()(IOStream& os, const T& v) const
		{
			os.putInteger (v);
		}
	};

	template <typename T>
	struct FloatWriter {
		inline void operator()(IOStream& os, const T& v) const
		{
			os.putFloat (v);
		}
	};

};

#include "iostream_impl.hpp"

#endif // XPCC__IOSTREAM_HPP
