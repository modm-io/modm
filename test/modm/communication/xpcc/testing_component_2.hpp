/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, 2017-2018, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	TESTING_COMPONENT_2_HPP
#define	TESTING_COMPONENT_2_HPP

#include <modm/communication/xpcc.hpp>

#include "timeline.hpp"

class TestingComponent2 : public xpcc::AbstractComponent
{
public:
	TestingComponent2(xpcc::Dispatcher &communication, Timeline *timeline);

	virtual ~TestingComponent2()
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

	// Event id: 0x20
	void
	eventNoParameter(const xpcc::Header& header);


	// id: 0x30
	void
	responseNoParameter(const xpcc::Header& header);

	// id: 0x31
	void
	responseUint16(const xpcc::Header& header, const uint16_t *parameter);

	// id: 0x32
	void
	responseCallAction(const xpcc::Header& header);

private:
	Timeline *timeline;
};

#endif
