/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LOG_STD_COLOUR_HPP
	#error "Don't include this file directly, use 'std_colour.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <xpcc::log::Colour TEXT, xpcc::log::Colour BACKGROUND, typename STYLE>
xpcc::log::StdColour<TEXT, BACKGROUND, STYLE>::StdColour( STYLE style ) :
	Style<STYLE> ( style )
{
}

template <xpcc::log::Colour TEXT, xpcc::log::Colour BACKGROUND, typename STYLE>
xpcc::log::StdColour<TEXT, BACKGROUND, STYLE>::StdColour(IODevice &device) :
	Style<STYLE> ( device )
{
}

template <xpcc::log::Colour TEXT, xpcc::log::Colour BACKGROUND, typename STYLE>
xpcc::log::StdColour<TEXT, BACKGROUND, STYLE>::~StdColour()
{
}

// ----------------------------------------------------------------------------

template <xpcc::log::Colour TEXT, xpcc::log::Colour BACKGROUND, typename STYLE>
void
xpcc::log::StdColour<TEXT, BACKGROUND, STYLE>::parseArg( int argc, char * argv )
{
	this->Style<STYLE>::parseArg( argc, argv );
}

// -----------------------------------------------------------------------------

template <xpcc::log::Colour TEXT, xpcc::log::Colour BACKGROUND, typename STYLE>
void
xpcc::log::StdColour<TEXT, BACKGROUND, STYLE>::write( char c )
{
	this->Style<STYLE>::write(this->getTextColour());
	this->Style<STYLE>::write(this->getBackgroundColour());
	this->Style<STYLE>::write(c);
}

template <xpcc::log::Colour TEXT, xpcc::log::Colour BACKGROUND, typename STYLE>
void
xpcc::log::StdColour<TEXT, BACKGROUND, STYLE>::write( const char* s )
{
	this->Style<STYLE>::write(this->getTextColour());
	this->Style<STYLE>::write(this->getBackgroundColour());
	this->Style<STYLE>::write(s);
}

// ----------------------------------------------------------------------------

template <xpcc::log::Colour TEXT, xpcc::log::Colour BACKGROUND, typename STYLE>
void
xpcc::log::StdColour<TEXT, BACKGROUND, STYLE>::flush()
{
	this->Style<STYLE>::write("\033[0m");
	this->Style<STYLE>::flush();
}

// ----------------------------------------------------------------------------
template <xpcc::log::Colour TEXT, xpcc::log::Colour BACKGROUND, typename STYLE>
const char*
xpcc::log::StdColour<TEXT, BACKGROUND, STYLE>::getTextColour()
{
	switch( TEXT )
	{
		case BLACK:
			return "\033[30m";
			break;
		case RED:
			return "\033[31m";
			break;
		case GREEN:
			return "\033[32m";
			break;
		case YELLOW:
			return "\033[33m";
			break;
		case BLUE:
			return "\033[34m";
			break;
		case MAGENTA:
			return "\033[35m";
			break;
		case TURQUOISE:
			return "\033[36m";
			break;
		case WHITE:
			return "\033[37m";
			break;
		default:
			return "";
	}
}

// ----------------------------------------------------------------------------

template <xpcc::log::Colour TEXT, xpcc::log::Colour BACKGROUND, typename STYLE>
const char*
xpcc::log::StdColour<TEXT, BACKGROUND, STYLE>::getBackgroundColour()
{
	switch( BACKGROUND )
	{
		case BLACK:
			return "\033[40m";
			break;
		case RED:
			return "\033[41m";
			break;
		case GREEN:
			return "\033[42m";
			break;
		case YELLOW:
			return "\033[43m";
			break;
		case BLUE:
			return "\033[44m";
			break;
		case MAGENTA:
			return "\033[45m";
			break;
		case TURQUOISE:
			return "\033[46m";
			break;
		case WHITE:
			return "\033[47m";
			break;
		default:
			return "";
	}
}
