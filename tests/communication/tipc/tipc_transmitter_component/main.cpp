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

#include <xpcc/communication/communication.hpp>
#include <xpcc/communication/postman/stl/postman.hpp>
#include <xpcc/communication/backend/tipc/tipc.hpp>

#include <xpcc/debug/logger/imp/std.hpp>
// set the Loglevel
#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DEBUG

int
main()
{
	xpcc::log::info << "########## XPCC TIPC COMPONENT Test TRANSMIT ##########" << xpcc::flush;

	// the hardware, that transfers the messages
	xpcc::TipcConnector tipc;

	// set the filters of the hardware
	tipc.addReceiverId(0x11);

	// set the 'list' of connections to the postman
	xpcc::DynamicPostman postman;

	// connect the 'hardware' to the postman
	xpcc::Communication com(&tipc, &postman);
	com.setCurrentComponent(0x11);

	xpcc::Header actionHeader( xpcc::Header::REQUEST, false, 0x10, 0x20, 0x01 );	// ACTION
	xpcc::Header eventHeader( xpcc::Header::REQUEST, false, 0x00, 0x20, 0x01 );	// ACTION

	int data(0);

	xpcc::IOStream stream(xpcc::log::device );

	while(1) {
		data++;

		com.update();

		xpcc::SmartPointer payload(&data);
		XPCC_LOG_INFO << XPCC_FILE_INFO << "data   =" << data << xpcc::endl;
		XPCC_LOG_INFO << XPCC_FILE_INFO << "payload=" << payload << xpcc::endl;


		com.callAction(0x10, 0x20, data);
		com.publishEvent(0x01, data);


		sleep(1);
	}


	xpcc::log::info << "########## XPCC TIPC RAW Test END ##########" << xpcc::flush;
}
