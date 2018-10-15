/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2015, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_DYNAMIC_POSTMAN_HPP
#define	XPCC_DYNAMIC_POSTMAN_HPP

#include "postman.hpp"
#include "../response_callback.hpp"
#include "../backend/header.hpp"
#include "../response_handle.hpp"

#include <map>
#include <functional>

namespace xpcc
{

/**
 * The Dynamic Postman is a generic Postman, which allows components to
 * add Action Handlers and Event Listeners at runtime.
 * This class should be used in hosted targets only, as the static Postman generated
 * by XPCC is much more efficient.
 *
 * On hosted however, this class allows for much easier registering of callbacks.
 *
 * @ingroup	modm_communication_xpcc
 * @author	Niklas Hauser
 */
class DynamicPostman : public Postman
{
public:
	DynamicPostman();

	DeliverInfo
	deliverPacket(const Header &header, const modm::SmartPointer& payload) override;

	bool
	isComponentAvailable(uint8_t component) const override;

public:
	template< class C >
	bool
	registerEventListener(const uint8_t eventId, C *componentObject,
						  void (C::*memberFunction)(const Header&));

	template< class C, typename P >
	bool
	registerEventListener(const uint8_t eventId, C *componentObject,
						  void (C::*memberFunction)(const Header&, const P&));

	template< class C >
	bool
	registerActionHandler(const uint8_t componentId, const uint8_t actionId, C *componentObject,
						  void (C::*memberFunction)(const ResponseHandle&));

	template< class C, typename P >
	bool
	registerActionHandler(const uint8_t componentId, const uint8_t actionId, C *componentObject,
						  void (C::*memberFunction)(const ResponseHandle&, const P&));

private:
	typedef std::function<void (const Header&, const uint8_t&)> EventCallback;
	typedef std::function<void (const Header&)> EventCallbackSimple;

	typedef std::function<void (const ResponseHandle&, const uint8_t&)> ActionCallback;
	typedef std::function<void (const ResponseHandle&)> ActionCallbackSimple;

	class EventListener
	{
		EventCallback call;
		EventCallbackSimple callSimple;
		int8_t hasPayload;

	public:
		EventListener();
		EventListener(EventCallback call);
		EventListener(EventCallbackSimple call);

		void operator()(const Header& header, const modm::SmartPointer& payload) const;
	};

	class ActionHandler
	{
		ActionCallback call;
		ActionCallbackSimple callSimple;
		int8_t hasPayload;

	public:
		ActionHandler();
		ActionHandler(ActionCallback call);
		ActionHandler(ActionCallbackSimple call);

		void operator()(const ResponseHandle& response, const modm::SmartPointer& payload) const;
	};

	/// packetIdentifier -> callback
	typedef std::multimap<uint8_t, EventListener> EventMap;

	/// packetIdentifier -> callback
	typedef std::map<uint8_t, ActionHandler > CallbackMap;
	///< destination -> callbackMap
	typedef std::map<uint8_t, CallbackMap > ActionMap;

private:
	EventMap eventMap;
	ActionMap actionMap;
};

}	// namespace xpcc

#include "dynamic_postman_impl.hpp"

#endif	// XPCC_DYNAMIC_POSTMAN_HPP
