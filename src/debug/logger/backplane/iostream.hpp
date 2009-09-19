// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

#ifndef XPCC_IOSTREAM_HPP_
#define XPCC_IOSTREAM_HPP_


#include "arithmetic_traits.hpp"
#include "iodevice.hpp"

namespace xpcc {
	class IOStream {
		public :

			IOStream();

			~IOStream();

			//! stream-operator to read static char-arrays
			//! \code
			//!		stream << "INFO";
			//! \endcode
			template<typename T>
			inline Stream&
			operator<< ( const T& c );

			inline IOStream&
			operator<< ( uint16_t i )
			{
				this->addInteger( i );
				return *this;
			}

			IOStream&
			operator<< ( int16_t );

			IOStream&
			operator<< ( uint32_t );

			IOStream&
			operator<< ( int32_t );

			IOStream&
			operator<< ( uint64_t );

			IOStream&
			operator<< ( int64_t );

		protected :
			void
			addString( const char*, uint8_t size );

			template<typename T>
			void
			addInteger( T value )
			{
				int8_t digit( arithmetic_traits<T>::dec_digits );
//				static const T compare[ arithmetic_traits<T>::dec_digits ] = {
//						1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
//				};

				if( value < 0 ) {
					this->device->put('-');
					val = -value;
				}

				uint8_t zero = 1;
				uint8_t c;
				T compare;

				do {
					compare = 10 * digit;
					c = '0';
					while( value > compare ) {
						value -= compare;
						c++;
						zero = 0;
					}
					if (zero == 0) {
						stream->put(d);
					}
					digit--;
				} while (digit);


			}

			bool
			allocate();

		public :
			friend std::ostream&
			operator<<(std::ostream& os, const IOStream& c);

			char* 	container;
			uint8_t	size;
			uint8_t	pos;

			IODevice* device;

	};

	std::ostream&
	operator<<(std::ostream& os, const xpcc::IOStream& c);
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

template<typename T>
inline xpcc::Stream&
xpcc::Stream::operator<< ( const T& c )
{
	this->addString( (const char*)&c, sizeof(T) );

	return *this;
}

#endif /* XPCC_IOSTREAM_HPP_ */
