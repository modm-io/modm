/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, 2015, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef FAKE_POSTMAN_HPP
#define FAKE_POSTMAN_HPP

#include <modm/communication/xpcc/postman/postman.hpp>
#include <modm/container/linked_list.hpp>

#include "testing_component_1.hpp"
#include "testing_component_2.hpp"
#include "message.hpp"

/**
 * 
 * 
 * \see	DispatcherTest
 */
class FakePostman : public xpcc::Postman
{
public:
	virtual ~FakePostman()
		{};

	virtual DeliverInfo
	deliverPacket(const xpcc::Header& header,
			const modm::SmartPointer& payload);
	
	virtual bool
	isComponentAvailable(uint8_t component) const;
	
public:
	modm::LinkedList<Message> messagesToDeliver;
	
	TestingComponent1 *component1;	///< must be created external!
	TestingComponent2 *component2;	///< must be created external!
};

#endif	// FAKE_POSTMAN_HPP
