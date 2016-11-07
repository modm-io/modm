/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
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

#include <xpcc/communication/xpcc/backend/backend_interface.hpp>
#include <xpcc/container/linked_list.hpp>

#include "message.hpp"

class FakeBackend : public xpcc::BackendInterface
{
public:
	virtual void
	update();
	
	virtual void
	sendPacket(const xpcc::Header &header,
			xpcc::SmartPointer payload = xpcc::SmartPointer());
	
	
	virtual bool
	isPacketAvailable() const;
	
	virtual const xpcc::Header&
	getPacketHeader() const;
	
	virtual const xpcc::SmartPointer
	getPacketPayload() const;
	
	virtual void
	dropPacket();

public:
	/// Messages send by the dispatcher via sendPacket
	xpcc::LinkedList<Message> messagesSend;
	
	/// Messages which should be received. isPacketAvailable(), getPacketHeader(),
	/// getPacketPayload() and dropPacket() operate on this list.
	xpcc::LinkedList<Message> messagesToReceive;
};

#endif
