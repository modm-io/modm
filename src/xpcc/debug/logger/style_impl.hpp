/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LOG_STYLE_HPP
	#error "Don't include this file directly, use 'style.hpp' instead!"
#endif

// -----------------------------------------------------------------------------

template < typename STYLE >
xpcc::log::Style<STYLE>::Style( STYLE style ) :
	style( style ),
	device ( 0 )
{
}

// -----------------------------------------------------------------------------

template < typename STYLE >
xpcc::log::Style<STYLE>::Style( IODevice &outputDevice) :
	device ( &outputDevice )
{
}

// -----------------------------------------------------------------------------

template < typename STYLE >
xpcc::log::Style<STYLE>::~Style()
{
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
xpcc::log::Style<STYLE>::parseArg( int /*argc*/, char * /*argv*/ )
{
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
xpcc::log::Style<STYLE>::write( char c )
{
	if ( tmp::SameType<STYLE, DefaultStyle >::value ) {
		this->device->write( c );
	}
	else {
		this->style.write( c );
	}
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
xpcc::log::Style<STYLE>::write( const char* s )
{
	if ( tmp::SameType<STYLE, DefaultStyle>::value ) {
		this->device->write( s );
	}
	else {
		this->style.write( s );
	}
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
xpcc::log::Style<STYLE>::flush()
{
	if ( tmp::SameType<STYLE, DefaultStyle >::value ) {
		this->device->flush();
	}
	else {
		this->style.flush();
	}
}
