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

#include "iodevice.hpp"

//#include <iostream>

namespace xpcc
{
	/**
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
		/// @brief	Constructor
		/// 
		/// @param	device	device to write the stream to
		/// 
		/// @code
		///	MyIODevice device;
		///	IOStream stream( device );
		/// @endcode
		IOStream(IODevice* device);

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
			this->mode = ASCII;
			return *this;
		}
		
		/// set the output mode to HEX style for \t char and \t char*
		inline IOStream&
		hex()
		{
			this->mode = HEX;
			return *this;
		}

		/// set the output mode to ASCII style for \t char and \t char*
		inline IOStream&
		ascii()
		{
			this->mode = ASCII;
			return *this;
		}

		inline IOStream&
		operator << ( const unsigned char& v )
		{
			if( this->mode == ASCII ) {
				this->putInteger( static_cast<unsigned short>( v ) );
			}
			else {
				this->putHex( v );
			}
			return *this;
		}

		inline IOStream&
		operator << ( const char& v )
		{
			if( this->mode == ASCII ) {
				this->device->put(v);
			}
			else {
				this->putHex( v );
			}
			return *this;
		}

		inline IOStream&
		operator << ( const unsigned short& v )
		{
			this->putInteger( v );
			return *this;
		}

		inline IOStream&
		operator << ( const short& v )
		{
			this->putInteger( v );
			return *this;
		}

		inline IOStream&
		operator << ( const unsigned int& v )
		{
			this->putInteger(v);
			return *this;
		}

		inline IOStream&
		operator << ( const int& v )
		{
			this->putInteger(v);
			return *this;
		}

		inline IOStream&
		operator << ( const float& v )
		{
			this->putFloat(v);
			return *this;
		}

		inline IOStream&
		operator << ( const double& v )
		{
			this->putFloat(v);
			return *this;
		}

		inline IOStream&
		operator << ( const char* v )
		{
			if( this->mode == ASCII ) {
				this->device->put( v );
			}
			else {
				this->putHex( v );
			}
			return *this;
		}

		inline IOStream&
		operator << ( IOStream& (*function)(IOStream&) )
		{
			return function( *this );
		}

	protected :
		void
		putInteger(int16_t value);
		
		void
		putInteger(uint16_t value);
		
#ifdef __AVR__
		// uses the optimized non standard function 'ltoa()' which is
		// not always available. For the general case snprintf() is
		// used.
		void
		putInteger(int32_t value);
		
		void
		putInteger(uint32_t value);
#endif
		
		// default version which is used when all the others don't match
		template<typename T>
		void
		putInteger( T value );

		void
		putHex( const char*  s );

		void
		putHex( char value );

		template<typename T>
		void
		putFloat( const T& value );

	private :
		typedef enum {
			ASCII,
			HEX
		} Mode;

		IOStream(const IOStream& );

		IOStream&
		operator =(const IOStream& );
		
		IODevice* const	device;

		Mode mode;
	};
	
	/**
	 * @brief  Flushes the output stream.
	 *
	 * This manipulator simply calls the stream's flush() member function.
	 * @ingroup io
	*/
	inline IOStream&
	flush(IOStream& ios)
	{
		return ios.flush();
	}
	
	/**
	 * @brief  Write a newline and flush the stream.
	 *
	 * @ingroup io
	 */
	inline IOStream&
	endl(IOStream& ios)
	{
		return flush(ios.put('\n'));
	}

	/**
	 * @brief  set the output mode to HEX style for \t char and \t char*
	 *
	 * @ingroup io
	 */
	inline IOStream&
	hex(IOStream& ios)
	{
		return ios.hex();
	}

	/**
	 * @brief  set the output mode to ASCII style for \t char and \t char*
	 *
	 * @ingroup io
	 */
	inline IOStream&
	ascii(IOStream& ios)
	{
		return ios.ascii();
	}
};


#include "iostream_impl.hpp"

#endif // XPCC__IOSTREAM_HPP
