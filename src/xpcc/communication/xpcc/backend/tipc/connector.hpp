/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, 2014, Martin Rosekeit
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__TIPC_HPP
#define XPCC__TIPC_HPP

#include "header.hpp"
#include "receiver.hpp"
#include "transmitter.hpp"

#include <xpcc/container/smart_pointer.hpp>

#include "../backend_interface.hpp"

namespace xpcc
{
	/**
	 * \brief	Class that connects the communication to the TIPC
	 *
	 * Messages that are received by the same connector, that has transmitted
	 * them, will be ignored.
	 *
	 * \see 	tipc
	 *
	 * \ingroup	backend
	 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
	 */
	class TipcConnector : public BackendInterface
	{
	public :
		TipcConnector();

		~TipcConnector();

		/**
		 * \brief	Change the domain id during runtime
		 */
		void
		setDomainId(unsigned int domainId);

		/**
		 * \brief	Add a new event to receive
		 *
		 * Call this method for every event you want to receive.
		 *
		 * \param	id	Identifier of the event.
		 */
		inline void
		addEventId(uint8_t id)
		{
			this->receiver.addEventId(id);
		}

		/**
		 * \brief	Add a new receiver
		 *
		 * You need to call this method for every component implemented in
		 * this module.
		 *
		 * \param	id	Identifier of the receiving component.
		 */
		inline void
		addReceiverId(uint8_t id)
		{
			this->receiver.addReceiverId(id);
		}

		/// Check if a new packet was received by the backend
		virtual bool
		isPacketAvailable() const;

		/**
		 * \brief	Access the packet header
		 *
		 * Only valid if isPacketAvailable() returns \c true.
		 */
		virtual const Header&
		getPacketHeader() const;

		/**
		 * \brief	Access the packet payload
		 *
		 * Only valid if isPacketAvailable() returns \c true.
		 */
		virtual const SmartPointer
		getPacketPayload() const;

		/**
		 * \brief	Delete the current packet
		 *
		 * Only valid if isPacketAvailable() returns \c true.
		 */
		virtual void
		dropPacket();

		/**
		 * \brief	Update method
		 *
		 * Does nothing here as TIPC is implemented with threads.
		 */
		virtual void
		update();

		/**
		 * Send a Message.
		 */
		virtual void
		sendPacket(const Header &header,
				   SmartPointer payload = SmartPointer());

	private:
		tipc::Transmitter transmitter;
		tipc::Receiver receiver;
	};
};

#endif	// XPCC__TIPC_HPP
