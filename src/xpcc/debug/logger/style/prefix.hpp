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
#ifndef XPCC_LOG_STYLE_PREFIX__HPP
#define XPCC_LOG_STYLE_PREFIX__HPP

#include <cstring>
#include "../style.hpp"
#include <xpcc/utils/typet.hpp>

namespace xpcc {
	namespace log {
		/**
		 * @class 	Prefix
		 * @brief 	Add a prefix to the logmessage
		 *
		 * @ingroup logger
		 * @version	$Id$
		 * @author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
		 */
		template <typename T, typename STYLE = DefaultStyle>
		class Prefix : public Style<STYLE> {
			public:
				Prefix(const T& str, STYLE style);

				Prefix(const T& str, IODevice &device);

				/// Write one char to the sink.
				void
				put( char c );

				/// Write a string that terminates with '\0' to the sink.
				void
				put( const char* s );

				/// The message is complete and can be written/send/displayed.
				void
				flush();

			private:
				bool	flushed;
				T		value;

		};
	}
}

template <typename T, typename STYLE>
xpcc::log::Prefix<T, STYLE>::Prefix( const T& str, STYLE style ) :
	Style<STYLE> ( style ),
	flushed ( true )
{
	memcpy( &this->value, &str, sizeof(T) );
}

template <typename T, typename STYLE>
xpcc::log::Prefix<T, STYLE>::Prefix( const T& str, IODevice &device ) :
	Style<STYLE> ( device ),
	flushed ( true )
{
	memcpy( &this->value, &str, sizeof(T) );
}

template <typename T, typename STYLE>
void
xpcc::log::Prefix<T, STYLE>::put( char c )
{
	if( this->flushed ) {
		this->flushed = false;
		this->Style<STYLE>::put( this->value );
	}
	this->Style<STYLE>::put( c );
}

template <typename T, typename STYLE>
void
xpcc::log::Prefix<T, STYLE>::put( const char* s )
{
	if( this->flushed ) {
		this->flushed = false;
		this->Style<STYLE>::put( this->value );
	}
	this->Style<STYLE>::put( s );
}


template <typename T, typename STYLE>
void
xpcc::log::Prefix<T, STYLE>::flush()
{
	this->flushed = true;
	this->Style<STYLE>::flush();
}

// -----------------------------------------------------------------------------

#endif // XPCC_LOG_STYLE_PREFIX__HPP
