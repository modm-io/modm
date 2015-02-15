// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
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
 * @ingroup	xpcc_comm
 * @author	Niklas Hauser
 */
class DynamicPostman : public Postman
{
public:
	DynamicPostman();

	DeliverInfo
	deliverPacket(const Header &header, const SmartPointer& payload) override;

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

		void operator()(const Header& header, const SmartPointer& payload) const;
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

		void operator()(const ResponseHandle& response, const SmartPointer& payload) const;
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
