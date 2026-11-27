/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/ext/dronecan/dronecan.hpp>
#include <modm/architecture/interface/fiber.hpp>
#include <chrono>

namespace modm::dronecan
{

/**
 * Client of a DroneCAN service, e.g. `Client<uavcan_protocol_param_GetSetRequest>`,
 * with one request at a time.
 *
 * A node can only subscribe once to the responses of a service, so there is
 * one client per service and node. The client matches a response by the
 * server node-ID and the transfer-ID of the last request.
 */
template< Request T >
class Client
{
public:
	using Response = typename Type<T>::Response;

	Client() = default;
	Client(const Client&) = delete;
	Client& operator=(const Client&) = delete;
	~Client() { if (canard_) canard_unsubscribe(canard_, &subscription_); }

	/// Subscribes to the responses on a node, e.g. `client.initialize(&node.canard)`.
	/// @return false if libcanard is out of memory, the responses are already
	///         subscribed or the client is already initialized
	bool
	initialize(canard_t* canard)
	{
		if (canard_ or subscribe<Response>(canard, &subscription_, &vtable) != &subscription_) return false;
		subscription_.user_context = this;
		canard_ = canard;
		return true;
	}

	/// Sends a request to a server. The response of a previous request is
	/// ignored from now on.
	/// @return false if the request could not be queued
	bool
	request(uint8_t server, const T& request, canard_prio_t priority = canard_prio_nominal)
	{
		done_ = false;
		server_ = server;
		pending_ = transfer_id_;
		if (dronecan::request(canard_, server, request, transfer_id_, priority)) return true;
		pending_ = NoTransfer;
		return false;
	}

	/// The response to the last request has arrived
	bool
	done() const
	{
		return done_;
	}

	/// The response to the last request, valid once done()
	const Response&
	response() const
	{
		return response_;
	}

	/// Sends a request and yields the fiber until the response arrives.
	/// Another fiber must update the node meanwhile. Without fibers, call
	/// request() and check done() while updating the node instead.
	/// @return the response, or nullptr if the request could not be queued or
	///         no response arrived in time
	const Response*
	call(uint8_t server, const T& request, std::chrono::milliseconds timeout = std::chrono::seconds(1),
		 canard_prio_t priority = canard_prio_nominal)
	{
		if (not this->request(server, request, priority)) return nullptr;
		if (not modm::this_fiber::poll_for(timeout, [this] { return done_; })) return nullptr;
		return &response_;
	}

private:
	static constexpr uint8_t NoTransfer = 0xff;

	// decodes directly into the response, which can be large, instead of the stack
	static void
	onResponse(canard_subscription_t* sub, canard_us_t, canard_prio_t, uint_least8_t source,
			   uint_least8_t transfer_id, canard_payload_t payload)
	{
		Client& client = *static_cast<Client*>(sub->user_context);
		if (not client.done_ and source == client.server_ and transfer_id == client.pending_)
			client.done_ = decode(payload, client.response_);
		canard::release(sub, payload);
	}
	static constexpr canard_subscription_vtable_t vtable{onResponse};

	canard_t* canard_{};
	canard_subscription_t subscription_{};
	Response response_{};
	uint8_t transfer_id_{};
	uint8_t pending_{NoTransfer};
	uint8_t server_{};
	bool done_{};
};

}	// namespace modm::dronecan
