/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LOG_PREFIX_HPP
	#error "Don't include this file directly, use 'prefix.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T, typename STYLE>
modm::log::Prefix<T, STYLE>::Prefix( const T& str, STYLE style ) :
	Style<STYLE> ( style ),
	flushed ( true )
{
	std::memcpy( &this->value, &str, sizeof(T) );
}

template <typename T, typename STYLE>
modm::log::Prefix<T, STYLE>::Prefix( const T& str, IODevice &device ) :
	Style<STYLE> ( device ),
	flushed ( true )
{
	std::memcpy( &this->value, &str, sizeof(T) );
}

// ----------------------------------------------------------------------------
template <typename T, typename STYLE>
void
modm::log::Prefix<T, STYLE>::write( char c )
{
	if( this->flushed ) {
		this->flushed = false;
		this->Style<STYLE>::write( this->value );
	}
	this->Style<STYLE>::write( c );
}

template <typename T, typename STYLE>
void
modm::log::Prefix<T, STYLE>::write( const char* s )
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
modm::log::Prefix<T, STYLE>::flush()
{
	this->flushed = true;
	this->Style<STYLE>::flush();
}

