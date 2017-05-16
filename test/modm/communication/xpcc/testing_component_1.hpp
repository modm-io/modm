/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	TESTING_COMPONENT_1_HPP
#define	TESTING_COMPONENT_1_HPP

#include <modm/communication/communication.hpp>

#include "timeline.hpp"

class TestingComponent1 : public xpcc::AbstractComponent
{
public:
	TestingComponent1(xpcc::Dispatcher &communication, Timeline *timeline);

	virtual ~TestingComponent1()
		{};

	void
	update();

	// expose protected methods
	using xpcc::AbstractComponent::callAction;
	using xpcc::AbstractComponent::publishEvent;

	// Action id: 0x10
	void
	actionNoParameter(const xpcc::ResponseHandle& handle);

	// Action id: 0x11
	void
	actionUint16(const xpcc::ResponseHandle& handle,
			const uint16_t *parameter);

	// Action id: 0x12
	// This method will generate an response
	void
	actionDirectResponse(const xpcc::ResponseHandle& handle);

	// Action id: 0x13
	// Generates an response the next time update gets called
	void
	actionDelayedResponse(const xpcc::ResponseHandle& handle);

	// Action id: 0x14
	// This method will call action 0x11 of component 2 when executed
	void
	actionUint16CallAction(const xpcc::ResponseHandle& handle,
			const uint16_t *parameter);



	// Event id: 0x20
	void
	eventNoParameter(const xpcc::Header& header);

	// Event id: 0x21
	void
	eventUint32(const xpcc::Header& header, const uint32_t *parameter);

private:
	Timeline *timeline;

	bool isDelayedResponseActive;
	xpcc::ResponseHandle delayedResponseHandle;
};

#endif
