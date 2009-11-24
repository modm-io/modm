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
#include "logger.hpp"

#include <xpcc/io/backplane/std/std_iodevice.hpp>
#include <xpcc/debug/logger/style_wrapper.hpp>
#include <xpcc/debug/logger/style/std_colour.hpp>
#include <xpcc/debug/logger/style/prefix.hpp>

xpcc::StdIODevice stdDevice;

//xpcc::log::Logger xpcc::log::debug(xpcc::log::StdColour<xpcc::log::BLUE, xpcc::log::BLACK>( stdDevice ));
//xpcc::log::StdColour<xpcc::log::BLUE, xpcc::log::BLACK> style( xpcc::log::Prefix("DEBUG: ") );
namespace xl = xpcc::log;

xl::Prefix< char[7] > prefix("DEBUG ", stdDevice);
xl::StdColour<xl::BLUE, xl::NONE, xl::Prefix< char[7] > > style( prefix );
xl::StyleWrapper< xl::StdColour<xl::BLUE, xl::NONE, xl::Prefix< char[7] > > > wrapper( style );
xl::Logger xl::debug( wrapper );

xl::StyleWrapper< xl::Prefix< char[7], xl::StdColour<xl::YELLOW, xl::NONE > > > wrapperInfo (
		xl::Prefix< char[7], xl::StdColour<xl::YELLOW, xl::NONE > >( "INFO: ",
				xl::StdColour<xl::YELLOW, xl::NONE >( stdDevice ) ) );

xl::Logger xl::info(wrapperInfo);

xl::Logger xl::warning(stdDevice);
xl::Logger xpcc::log::error(stdDevice);

//xpcc::log::error.operator <<(22);

//xpcc::log::error << "Logger created";// << xpcc::flush;
