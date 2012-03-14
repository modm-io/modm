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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LOG__STD_COLOUR_HPP
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
