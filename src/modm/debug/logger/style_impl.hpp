/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LOG_STYLE_HPP
	#error "Don't include this file directly, use 'style.hpp' instead!"
#endif

#include <type_traits>

// -----------------------------------------------------------------------------

template < typename STYLE >
modm::log::Style<STYLE>::Style( STYLE style ) :
	style( style ),
	device ( 0 )
{
}

// -----------------------------------------------------------------------------

template < typename STYLE >
modm::log::Style<STYLE>::Style( IODevice &outputDevice) :
	device ( &outputDevice )
{
}

// -----------------------------------------------------------------------------

template < typename STYLE >
modm::log::Style<STYLE>::~Style()
{
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
modm::log::Style<STYLE>::parseArg( int /*argc*/, char * /*argv*/ )
{
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
modm::log::Style<STYLE>::write( char c )
{
	if ( std::is_same_v<STYLE, DefaultStyle > ) {
		this->device->write( c );
	}
	else {
		this->style.write( c );
	}
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
modm::log::Style<STYLE>::write( const char* s )
{
	if ( std::is_same_v<STYLE, DefaultStyle> ) {
		this->device->write( s );
	}
	else {
		this->style.write( s );
	}
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
modm::log::Style<STYLE>::flush()
{
	if ( std::is_same_v<STYLE, DefaultStyle > ) {
		this->device->flush();
	}
	else {
		this->style.flush();
	}
}
