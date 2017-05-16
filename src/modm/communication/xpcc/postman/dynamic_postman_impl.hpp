/*
 * Copyright (c) 2015, Kevin Läufer
 * Copyright (c) 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_DYNAMIC_POSTMAN_HPP
#	error	"Don't include this file directly, use 'dynamic_postman.h' instead"
#endif

// ----------------------------------------------------------------------------
template< class C >
bool
xpcc::DynamicPostman::registerEventListener(const uint8_t eventId,
		C *componentObject,
		void (C::*memberFunction)(const Header&))
{
	using namespace std::placeholders;

	eventMap.insert(
			std::pair<uint8_t, EventListener>(
					eventId,
					EventListener(static_cast<EventCallbackSimple>(
							std::bind(
									memberFunction,
									componentObject,
									_1)
					))
			)
	);

	return true;
}

template< class C, typename P >
bool
xpcc::DynamicPostman::registerEventListener(const uint8_t eventId,
		C *componentObject,
		void (C::*memberFunction)(const Header&, const P&))
{
	using namespace std::placeholders;
	typedef void (C::*Function)(const Header&, const uint8_t&);

	eventMap.insert(
			std::pair<uint8_t, EventListener>(
					eventId,
					EventListener(
							std::bind(
									reinterpret_cast<Function>(memberFunction),
									componentObject,
									_1, _2)
					)
			)
	);

	return true;
}

template< class C >
bool
xpcc::DynamicPostman::registerActionHandler(const uint8_t componentId,
		const uint8_t actionId,
		C *componentObject,
		void (C::*memberFunction)(const ResponseHandle&))
{
	using namespace std::placeholders;

	actionMap[componentId][actionId] = ActionHandler(static_cast<ActionCallbackSimple>(
			std::bind(
					memberFunction,
					componentObject,
					_1)
	));

	return true;
}

template< class C, typename P >
bool
xpcc::DynamicPostman::registerActionHandler(const uint8_t componentId,
		const uint8_t actionId,
		C *componentObject,
		void (C::*memberFunction)(const ResponseHandle&, const P&))
{
	using namespace std::placeholders;
	typedef void (C::*Function)(const ResponseHandle&, const uint8_t&);

	actionMap[componentId][actionId] = ActionHandler(
			std::bind(
					reinterpret_cast<Function>(memberFunction),
					componentObject,
					_1, _2)
	);

	return true;
}
