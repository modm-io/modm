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

#ifndef XPCC_LOG__PREFIX_HPP
	#error "Don't include this file directly, use 'prefix.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T, typename STYLE>
xpcc::log::Prefix<T, STYLE>::Prefix( const T& str, STYLE style ) :
	Style<STYLE> ( style ),
	flushed ( true )
{
	std::memcpy( &this->value, &str, sizeof(T) );
}

template <typename T, typename STYLE>
xpcc::log::Prefix<T, STYLE>::Prefix( const T& str, IODevice &device ) :
	Style<STYLE> ( device ),
	flushed ( true )
{
	std::memcpy( &this->value, &str, sizeof(T) );
}

// ----------------------------------------------------------------------------
template <typename T, typename STYLE>
void
xpcc::log::Prefix<T, STYLE>::write( char c )
{
	if( this->flushed ) {
		this->flushed = false;
		this->Style<STYLE>::write( this->value );
	}
	this->Style<STYLE>::write( c );
}

template <typename T, typename STYLE>
void
xpcc::log::Prefix<T, STYLE>::write( const char* s )
{
	if( this->flushed ) {
		this->flushed = false;
		this->Style<STYLE>::write( this->value );
	}
	this->Style<STYLE>::write( s );
}

// ----------------------------------------------------------------------------
template <typename T, typename STYLE>
void
xpcc::log::Prefix<T, STYLE>::flush()
{
	this->flushed = true;
	this->Style<STYLE>::flush();
}

#endif // XPCC_LOG__PREFIX_HPP
