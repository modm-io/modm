/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TIPC_RECEIVER_SOCKET_H_
#define XPCC_TIPC_RECEIVER_SOCKET_H_

#include "header.hpp"

#include <stdint.h>

namespace xpcc {
	namespace tipc {
		/**
		 * \class		ReceiverSocket
		 * \brief		Provide the handling of the socket interface from TIPC.
		 * 
		 * \ingroup		tipc
		 * \author		Carsten Schmitt < >
		 */
		class ReceiverSocket {
			public:	
				ReceiverSocket();
				~ReceiverSocket();
		
				void 
				registerOnPacket(	unsigned int typeId,
									unsigned int lowerInstance,
									unsigned int upperInstance);
		
				/**
				 * \param transmitterPortId the id of the tipc port, that transmitted the message is returned
				 * \param tipcHeader the tipc header is returned
				 */
				bool 
				receiveHeader(
						uint32_t & transmitterPortId,
						tipc::Header & tipcHeader );
				
				bool 
				receivePayload(
						uint8_t* payloadPointer,
						size_t payloadLength);
				
				bool 
				popPayload();
		
			private:
				const int socketDescriptor_;
		};
	}
}

#endif /* XPCC_TIPC_RECEIVER_SOCKET_H_ */
