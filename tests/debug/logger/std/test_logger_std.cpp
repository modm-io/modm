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

#include <xpcc/io/backplane/std/std_iodevice.hpp>
#include <xpcc/debug/logger/logger.hpp>
#include <xpcc/debug/logger/style_wrapper.hpp>
#include <xpcc/debug/logger/style/std_colour.hpp>
#include <xpcc/debug/logger/style/prefix.hpp>

xpcc::StdIODevice device;

xpcc::log::Prefix< char[8] > prefix("DEBUG: ", device);
xpcc::log::StdColour<xpcc::log::BLUE, xpcc::log::NONE, xpcc::log::Prefix< char[8] > > style( prefix );
xpcc::log::StyleWrapper< xpcc::log::StdColour<xpcc::log::BLUE, xpcc::log::NONE, xpcc::log::Prefix< char[8] > > > wrapperDebug( style );
xpcc::log::Logger xpcc::log::debug( wrapperDebug );

xpcc::log::StyleWrapper< xpcc::log::Prefix< char[7], xpcc::log::StdColour<xpcc::log::YELLOW, xpcc::log::NONE > > > wrapperInfo (
		xpcc::log::Prefix< char[7], xpcc::log::StdColour<xpcc::log::YELLOW, xpcc::log::NONE > >( "INFO: ",
				xpcc::log::StdColour<xpcc::log::YELLOW, xpcc::log::NONE >( device ) ) );
xpcc::log::Logger xpcc::log::info(wrapperInfo);

xpcc::log::Logger xpcc::log::warning(device);
xpcc::log::Logger xpcc::log::error(device);

int
main()
{
	xpcc::log::info << "XPCC Logger Test" << xpcc::flush;

//	xpcc::log::setDevice( device );
	//xpcc::log::setDevice( new xpcc::log::DeviceStd );
	//xpcc::log::setDevice( device );

//	xpcc::log::setFilter(xpcc::log::WARNING);

	xpcc::log::debug << "Logmessage" << xpcc::flush;
	xpcc::log::info << "zur info: 100=" << 100 << xpcc::flush;
	xpcc::log::warning << "100!=" << 1001 << xpcc::flush;



	xpcc::log::info << "ENDE" << xpcc::flush;
}
