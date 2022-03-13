/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef FAKE_BACKEND_HPP
#define FAKE_BACKEND_HPP

#include <modm/communication/xpcc/backend/backend_interface.hpp>
#include <modm/container/linked_list.hpp>

#include "message.hpp"

/// @ingroup modm_test_test_communication_xpcc
class FakeBackend : public xpcc::BackendInterface
{
public:
	virtual void
	update();

	virtual void
	sendPacket(const xpcc::Header &header,
			modm::SmartPointer payload = modm::SmartPointer());


	virtual bool
	isPacketAvailable() const;

	virtual const xpcc::Header&
	getPacketHeader() const;

	virtual const modm::SmartPointer
	getPacketPayload() const;

	virtual void
	dropPacket();

public:
	/// Messages send by the dispatcher via sendPacket
	modm::LinkedList<Message> messagesSend;

	/// Messages which should be received. isPacketAvailable(), getPacketHeader(),
	/// getPacketPayload() and dropPacket() operate on this list.
	modm::LinkedList<Message> messagesToReceive;
};

#endif
