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

#include <xpcc/communication/xpcc/communication.hpp>
#include <xpcc/communication/xpcc/postman/stl/postman.hpp>
#include <xpcc/communication/xpcc/backend/tipc/tipc.hpp>

#include <xpcc/debug/logger/imp/std.hpp>
// set the Loglevel
#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DEBUG

class Receiver : public xpcc::AbstractComponent
{
	public:
		void
		actionCallback( const int* payload )
		{
			XPCC_LOG_INFO << XPCC_FILE_INFO << "has ACTION";
			XPCC_LOG_INFO << " from: TODO";
			XPCC_LOG_INFO << " value:" << *payload;
			XPCC_LOG_INFO << xpcc::flush;
		}

		void
		eventCallback( const int* payload )
		{
			XPCC_LOG_INFO << XPCC_FILE_INFO << "has EVENT";
			XPCC_LOG_INFO << " from: TODO";
			XPCC_LOG_INFO << " value:" << *payload;
			XPCC_LOG_INFO << xpcc::flush;
		}
	
	private:
		xpcc::TipcConnector tipc;
};

int
main()
{
	xpcc::log::info << "########## XPCC TIPC COMPONENT Test Receiver ##########" << xpcc::flush;
	
	// the component, that receives the messages
	Receiver receiver;

	// the hardware, that transfers the messages
	xpcc::TipcConnector tipc;

	// set the filters of the hardware
	tipc.addReceiverId(0x10);
	tipc.addEventId(0x01);


	// the connection between messageID and callback-methods
	// EVENT = 0x01;
	xpcc::StlPostman::EventMap eventMap;
	eventMap.insert( xpcc::StlPostman::EventMap::value_type( 0x01, xpcc::Callback(&receiver, &Receiver::eventCallback) ) );

	// ACTION = 0x10;
	xpcc::StlPostman::CallbackMap receiverCallbackMap;
	receiverCallbackMap[0x20] = xpcc::Callback(&receiver, &Receiver::actionCallback);

	xpcc::StlPostman::RequestMap componentMap;
	componentMap[0x10] = receiverCallbackMap;

	// set the 'list' of connections to the postman
	xpcc::StlPostman postman( eventMap, componentMap );

	// connect the 'hardware' to the postman
	xpcc::Communication com(&tipc, &postman);



	while(1) {
		// run the communication
		com.update();

		usleep(100000);
	}

	xpcc::log::info << "########## XPCC TIPC COMPONENT Test Receiver END ##########" << xpcc::flush;
}
