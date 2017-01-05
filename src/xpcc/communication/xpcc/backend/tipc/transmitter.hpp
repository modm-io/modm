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

#ifndef XPCC_TIPC_TRANSMITTER_HPP
#define XPCC_TIPC_TRANSMITTER_HPP

// STD exceptions...
//#include <stdexcept>

#include <bitset>

#include "transmitter_socket.hpp"
#include <modm/container/smart_pointer.hpp>

namespace xpcc
{
	namespace tipc
	{
		/**
		 * \class		Transmitter
		 * \brief		Transmit packets over the TIPC.
		 *
		 * \todo		exception handling : now it writs only log-messages
		 *
		 * \ingroup		tipc
		 * \author		Carsten Schmitt < >
		 */
		class Transmitter
		{
		public:
			Transmitter( );

			~Transmitter();

			void
			setDomainId( unsigned int id );

			void
			transmitRequest( uint8_t destination, const modm::SmartPointer& payload );

			void
			transmitEvent( uint8_t event, const modm::SmartPointer& payload );

			uint32_t
			getPortId();

		private:
			TransmitterSocket 	tipcTransmitterSocket_;
			unsigned int 	domainId_;
		};
	}
}
#endif // XPCC_TIPC_TRANSMITTER_HPP
