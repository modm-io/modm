/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, 2014, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TIPC_TRANSMITTER_SOCKET_H_
#define XPCC_TIPC_TRANSMITTER_SOCKET_H_
 
#include <cstring>
#include <stdint.h>
#include "header.hpp"

namespace xpcc {
	namespace tipc {
		/**
		 * \class		TransmitterSocket
		 * \brief		Proviede the handling of the socket interface from TIPC.
		 * 
		 * \ingroup		tipc
		 * \author		Carsten Schmitt < >
		 */
		class TransmitterSocket {
			public:
				TransmitterSocket();
				~TransmitterSocket();
		
				void 
				transmitPayload(	unsigned int typeId,
									unsigned int instanceId,
									const uint8_t* packet,
									size_t length,
									unsigned int domainId = Header::DOMAIN_ID_UNDEFINED );

				/*
				 * \brief Returns the ref part of the tipc port id.
				 *
				 * 1.3.2 Port Identifier \n
				 * Each port in a TIPC network has a unique "port identifier" or "port ID",
				 * which is typically denoted as <Z.C.N:ref>.  The port ID is assigned
				 * automatically by TIPC when the port is created, and consists of the 32-bit
				 * network address of the port's node and a 32-bit reference value.  The
				 * reference value is guaranteed to be unique on a per-node basis and will not
				 * be reused for a long time once the port ceases to exist.
				 */
				uint32_t
				getPortId();
		
			private:
				const int socketDescriptor_;
		};
	}
}
#endif // TIPC_TRANSMITTER_SOCKET_H_
