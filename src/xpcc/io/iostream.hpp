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

#ifndef XPCC__IOSTREAM_HPP
#define XPCC__IOSTREAM_HPP

#include <xpcc/architecture/platform.hpp>

#include "iodevice.hpp"
#include "iodevice_wrapper.hpp"

namespace xpcc
{
	/**
	 * \brief 	This Formats all primary typs into a string stream for
	 * 			output or it reads values from a input and converts them to
	 * 			a given type;
	 *
	 * \ingroup io
	 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
	 */
	class IOStream
	{
	public :
		/**
		 * \brief	Constructor
		 * 
		 * \param	device	device to write the stream to
		 * 
		 * \code
		 *	MyIODevice device;
		 *	IOStream stream( device );
		 * \endcode
		 */
		IOStream(IODevice& device);

		inline IOStream&
		write(char c)
		{
			this->device->write(c);
			return *this;
		}
		
		inline IOStream&
		flush()
		{
			this->device->flush();
			this->mode = ASCII;
			return *this;
		}
		
		/// set the output mode to HEX style for \c char and \c char*
		ALWAYS_INLINE IOStream&
		hex()
		{
			this->mode = HEX;
			return *this;
		}

		/// set the output mode to ASCII style for \c char and \c char*
		ALWAYS_INLINE IOStream&
		ascii()
		{
			this->mode = ASCII;
			return *this;
		}

		inline IOStream&
		operator << ( const unsigned char& v )
		{
			if( this->mode == ASCII ) {
				this->writeInteger( static_cast<uint16_t>( v ) );
			}
			else {
				this->writeHex( v );
			}
			return *this;
		}

		inline IOStream&
		operator << ( const char& v )
		{
			if( this->mode == ASCII ) {
				this->device->write(v);
			}
			else {
				this->writeHex( v );
			}
			return *this;
		}

		ALWAYS_INLINE IOStream&
		operator << ( const uint16_t& v )
		{
			this->writeInteger( v );
			return *this;
		}

		ALWAYS_INLINE IOStream&
		operator << ( const int16_t& v )
		{
			this->writeInteger( v );
			return *this;
		}

		ALWAYS_INLINE IOStream&
		operator << ( const uint32_t& v )
		{
			this->writeInteger(v);
			return *this;
		}

		ALWAYS_INLINE IOStream&
		operator << ( const int32_t& v )
		{
			this->writeInteger(v);
			return *this;
		}
		
#ifndef __AVR__
		ALWAYS_INLINE IOStream&
		operator << ( const uint64_t& v )
		{
			this->writeInteger(v);
			return *this;
		}
		
		ALWAYS_INLINE IOStream&
		operator << ( const int64_t& v )
		{
			this->writeInteger(v);
			return *this;
		}
#endif
		
		ALWAYS_INLINE IOStream&
		operator << ( const float& v )
		{
			this->writeFloat(v);
			return *this;
		}

		ALWAYS_INLINE IOStream&
		operator << ( const double& v )
		{
			this->writeFloat(v);
			return *this;
		}

		ALWAYS_INLINE IOStream&
		operator << ( const char* v )
		{
			if( this->mode == ASCII ) {
				this->device->write( v );
			}
			else {
				this->writeHex( v );
			}
			return *this;
		}

		ALWAYS_INLINE IOStream&
		operator << ( IOStream& (*function)(IOStream&) )
		{
			return function(*this);
		}

	protected :
		void
		writeInteger(int16_t value);
		
		void
		writeInteger(uint16_t value);
		
		void
		writeInteger(int32_t value);
		
		void
		writeInteger(uint32_t value);
		
		void
		writeInteger(int64_t value);
		
		void
		writeInteger(uint64_t value);
		
		void
		writeHex( const char*  s );

		void
		writeHex( char value );

		template<typename T>
		void
		writeFloat( const T& value );

	private :
		enum Mode
		{
			ASCII,
			HEX
		};

		IOStream(const IOStream& );

		IOStream&
		operator =(const IOStream& );
		
		IODevice* const	device;

		Mode mode;
	};
	
	/**
	 * \brief  Flushes the output stream.
	 *
	 * This manipulator simply calls the stream's flush() member function.
	 * \ingroup io
	*/
	inline IOStream&
	flush(IOStream& ios)
	{
		return ios.flush();
	}
	
	/**
	 * \brief  Write a newline and flush the stream.
	 *
	 * \ingroup io
	 */
	inline IOStream&
	endl(IOStream& ios)
	{
		return flush(ios.write('\n'));
	}

	/**
	 * \brief  set the output mode to HEX style for \b char and \b char*
	 *
	 * \ingroup io
	 */
	inline IOStream&
	hex(IOStream& ios)
	{
		return ios.hex();
	}

	/**
	 * \brief  set the output mode to ASCII style for \b char and \b char*
	 *
	 * \ingroup io
	 */
	inline IOStream&
	ascii(IOStream& ios)
	{
		return ios.ascii();
	}
};


#include "iostream_impl.hpp"

#endif // XPCC__IOSTREAM_HPP
