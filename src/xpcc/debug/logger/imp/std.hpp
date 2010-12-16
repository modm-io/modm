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

// the default style creates globals objects, so it can only one time be included
#ifdef XPCC_LOG__STYLE_DEFAULT_HPP
	#error "Second inclusion of a default log style"
#else
#define XPCC_LOG__STYLE_DEFAULT_HPP

#ifdef __AVR__
	// this version is to use on PC only
	#error "You can't use this implementation on an AVRs"
#endif

#ifndef __DOXYGEN__

#include "../logger.hpp"
#include "../style_wrapper.hpp"
#include "../style/prefix.hpp"
#include "../style/std_colour.hpp"

#include <xpcc/driver/pc/terminal.hpp>

namespace xpcc
{
	namespace log
	{
		xpcc::pc::Terminal device;

		template < typename T, Colour TEXT, Colour BACKGROUND >
		class Wrapper : public StyleWrapper< Prefix< T, StdColour<TEXT, BACKGROUND > > >
		{
			public:
				Wrapper(const T& str, ::xpcc::IODevice& device) :
					StyleWrapper< Prefix< T, StdColour<TEXT, BACKGROUND > > > (
							Prefix< T, StdColour<TEXT, BACKGROUND > >(
									str,
									StdColour<TEXT, BACKGROUND >( device ) ) )
				{
				}
		};

		Wrapper< char[10], BLUE, NONE > debugWrapper("Debug:   ", device);
		Logger debug( debugWrapper );

		Wrapper< char[10], GREEN, NONE > debugInfo("Info:    ", device);
		Logger info( debugInfo );

		Wrapper< char[10], YELLOW, NONE > warningInfo("Warning: ", device);
		Logger warning( warningInfo );

		Wrapper< char[10], RED, NONE > errorInfo("Error:   ", device);
		Logger error( errorInfo );
	}
}

#endif	// !__DOXYGEN__
#endif	// XPCC_LOG__STYLE_DEFAULT_HPP
