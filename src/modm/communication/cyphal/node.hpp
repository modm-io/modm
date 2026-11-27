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

#include <modm/ext/canard/canard.hpp>
#include <modm/ext/canard/parameter.hpp>
#include <uavcan/node/Heartbeat_1_0.h>
#include <uavcan/node/GetInfo_1_0.h>
#include <uavcan/node/ExecuteCommand_1_3.h>
#include <uavcan/_register/Access_1_0.h>
#include <uavcan/_register/List_1_0.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <span>
#include <string_view>

namespace modm::cyphal
{

/// Identity of a node for the uavcan.node.GetInfo service
struct NodeInfo
{
	/// Reversed internet domain name, e.g. "io.modm.example", at most 50
	/// characters of a-z, 0-9, dot and underscore.
	std::string_view name;
	/// Unique ID of the device, e.g. its UID padded with zeros
	std::array<uint8_t, 16> unique_id;
	uint8_t software_major{};
	uint8_t software_minor{};
	/// Optional revision of the version control system, e.g. a Git hash
	uint64_t vcs_revision{};
	uint8_t hardware_major{};
	uint8_t hardware_minor{};
};

/// Configuration of modm::cyphal::Node
struct NodeConfig
{
	/// Node-ID 0-127, or CANARD_NODE_ID_ANONYMOUS to let libcanard pick a free one.
	uint8_t node_id = CANARD_NODE_ID_ANONYMOUS;
	/// Parameters of the register services in a stable order
	std::span<canard::Parameter* const> parameters{};
	/// Saves the parameters with the ExecuteCommand service, or nullptr to keep
	/// them only in RAM. Load them with canard::loadParameters() before.
	canard::ParameterStorage* storage = nullptr;
	/// Called by the ExecuteCommand service with COMMAND_RESTART after the
	/// response was sent, e.g. NVIC_SystemReset(), or nullptr to refuse restarts.
	void (*restart)() = nullptr;
	/// Configuration of the libcanard node
	canard::NodeConfig node{};
};

/// Sets a parameter from a register value with one element: an integer, real
/// or bit value as the type of the parameter, or an integer also for real and
/// boolean parameters (0 or 1).
/// @return false if the type does not match or the value is out of range
inline bool
setRegister(canard::Parameter& parameter, const uavcan_register_Value_1_0& value)
{
	using Type = canard::Parameter::Type;
	const auto integral = [&](auto x, size_t count) -> bool
	{
		if (count != 1) return false;
		if (parameter.type() == Type::Boolean)
		{
			if (x != 0 and x != 1) return false;
			return parameter.set(x == 1);
		}
		return parameter.set(x);
	};
	const auto real = [&](double x, size_t count) -> bool
	{
		// the range check of the parameter happens before narrowing to float
		return count == 1 and parameter.type() == Type::Real and parameter.set(x);
	};
	const auto* v = &value;
	if (uavcan_register_Value_1_0_is_bit_(v))
		return v->bit.value.count == 1 and parameter.type() == Type::Boolean and
			   parameter.set(nunavutGetBit(v->bit.value.bitpacked, sizeof(v->bit.value.bitpacked), 0));
	if (uavcan_register_Value_1_0_is_integer64_(v)) return integral(v->integer64.value.elements[0], v->integer64.value.count);
	if (uavcan_register_Value_1_0_is_integer32_(v)) return integral(v->integer32.value.elements[0], v->integer32.value.count);
	if (uavcan_register_Value_1_0_is_integer16_(v)) return integral(v->integer16.value.elements[0], v->integer16.value.count);
	if (uavcan_register_Value_1_0_is_integer8_(v)) return integral(v->integer8.value.elements[0], v->integer8.value.count);
	if (uavcan_register_Value_1_0_is_natural64_(v)) return integral(v->natural64.value.elements[0], v->natural64.value.count);
	if (uavcan_register_Value_1_0_is_natural32_(v)) return integral(v->natural32.value.elements[0], v->natural32.value.count);
	if (uavcan_register_Value_1_0_is_natural16_(v)) return integral(v->natural16.value.elements[0], v->natural16.value.count);
	if (uavcan_register_Value_1_0_is_natural8_(v)) return integral(v->natural8.value.elements[0], v->natural8.value.count);
	if (uavcan_register_Value_1_0_is_real64_(v)) return real(v->real64.value.elements[0], v->real64.value.count);
	if (uavcan_register_Value_1_0_is_real32_(v)) return real(v->real32.value.elements[0], v->real32.value.count);
	if (uavcan_register_Value_1_0_is_real16_(v)) return real(v->real16.value.elements[0], v->real16.value.count);
	return false;
}

/// Writes the value of a parameter into a register value: integer64, real32
/// or bit with one element
inline void
getRegister(const canard::Parameter& parameter, uavcan_register_Value_1_0& value)
{
	switch (parameter.type())
	{
		case canard::Parameter::Type::Integer:
			uavcan_register_Value_1_0_select_integer64_(&value);
			value.integer64.value.elements[0] = parameter.integer();
			value.integer64.value.count = 1;
			break;
		case canard::Parameter::Type::Real:
			uavcan_register_Value_1_0_select_real32_(&value);
			value.real32.value.elements[0] = parameter.real();
			value.real32.value.count = 1;
			break;
		case canard::Parameter::Type::Boolean:
			uavcan_register_Value_1_0_select_bit_(&value);
			value.bit.value.bitpacked[0] = parameter.boolean();
			value.bit.value.count = 1;
			break;
	}
}

/**
 * A Cyphal node with the standard services of the uavcan.node and
 * uavcan.register namespaces.
 *
 * - publishes uavcan.node.Heartbeat once per second,
 * - answers uavcan.node.GetInfo,
 * - answers uavcan.node.ExecuteCommand with COMMAND_RESTART,
 *   COMMAND_STORE_PERSISTENT_STATES and COMMAND_FACTORY_RESET,
 * - and gives access to its parameters as registers with
 *   uavcan.register.Access and List.
 *
 * Use it like modm::canard::Node for the other data types of the application.
 * Without node-ID, libcanard picks a free one and moves to another one when
 * it detects a collision.
 */
template< class Can, size_t ArenaSize = 4096 >
class Node : public canard::Node<Can, ArenaSize>
{
	using Base = canard::Node<Can, ArenaSize>;

public:
	using Base::canard;
	using Config = NodeConfig;

	/// The name of `info` and the parameters must stay valid as long as the node.
	/// @return false if libcanard cannot be initialized or one of the services
	///         is already subscribed
	bool
	initialize(const NodeInfo& info, Config config = {})
	{
		info_ = info;
		parameters_ = config.parameters;
		storage_ = config.storage;
		restart_ = config.restart;
		start_ = canard::now();

		uint64_t seed{};
		for (const uint8_t byte : info.unique_id) seed = (seed ^ byte) * 0x100000001b3ull;
		if (not Base::initialize(config.node_id, seed, config.node)) return false;

		return subscribe<onGetInfo>(info_sub_, uavcan_node_GetInfo_1_0_FIXED_PORT_ID_,
					uavcan_node_GetInfo_Request_1_0_EXTENT_BYTES_) and
			   subscribe<onExecuteCommand>(command_sub_, uavcan_node_ExecuteCommand_1_3_FIXED_PORT_ID_,
					uavcan_node_ExecuteCommand_Request_1_3_EXTENT_BYTES_) and
			   subscribe<onAccess>(access_sub_, uavcan_register_Access_1_0_FIXED_PORT_ID_,
					uavcan_register_Access_Request_1_0_EXTENT_BYTES_) and
			   subscribe<onList>(list_sub_, uavcan_register_List_1_0_FIXED_PORT_ID_,
					uavcan_register_List_Request_1_0_EXTENT_BYTES_);
	}

	/// Receives and transmits frames, runs the services and publishes the
	/// heartbeat. Call this every few milliseconds.
	void
	update()
	{
		Base::update();
		serve();
		Base::poll();
	}

	/// Runs the services and publishes the heartbeat without receiving or
	/// transmitting, for applications that pass the frames with ingest() and
	/// transmit them with poll() themselves.
	void
	serve()
	{
		const canard_us_t now = canard::now();
		if (info_request_.pending) respondInfo();
		if (command_request_.pending) respondCommand(now);
		if (access_request_.pending) respondAccess();
		if (list_request_.pending) respondList();

		if (now >= heartbeat_deadline_)
		{
			heartbeat_deadline_ = now + 1'000'000;
			const uavcan_node_Heartbeat_1_0 message = heartbeat();
			uint8_t buffer[uavcan_node_Heartbeat_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
			size_t size = sizeof(buffer);
			if (uavcan_node_Heartbeat_1_0_serialize_(&message, buffer, &size) >= 0)
			{
				canard_publish_13b(&canard, now + 1'000'000, CANARD_IFACE_BITMAP_ALL, canard_prio_nominal,
								   uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_, heartbeat_transfer_id_,
								   {{size, buffer}, nullptr}, nullptr);
				heartbeat_transfer_id_ = (heartbeat_transfer_id_ + 1) % CANARD_TRANSFER_ID_MODULO;
			}
		}
		// restart once the transmit queue is empty, or after a second if it does not drain
		if (restart_deadline_ and now >= restart_deadline_ and
			(canard_pending_ifaces(&canard) == 0 or now >= restart_deadline_ + 1'000'000))
		{
			restart_deadline_ = 0;
			restart_();
		}
	}

	uint8_t
	nodeId() const
	{
		return canard.node_id;
	}

	void setHealth(uint8_t health) { health_ = health; }
	void setMode(uint8_t mode) { mode_ = mode; }
	void setVendorStatus(uint8_t code) { vendor_status_ = code; }

	/// Current uavcan.node.Heartbeat
	uavcan_node_Heartbeat_1_0
	heartbeat() const
	{
		uavcan_node_Heartbeat_1_0 heartbeat{};
		heartbeat.uptime = (canard::now() - start_) / 1'000'000;
		heartbeat.health.value = health_;
		heartbeat.mode.value = mode_;
		heartbeat.vendor_specific_status_code = vendor_status_;
		return heartbeat;
	}

	/// Writes the parameters with a non-default value to the storage and
	/// removes the others, so that new default values take effect.
	/// @return false without storage or if a write failed
	bool
	save()
	{
		return storage_ and canard::saveParameters(parameters_, *storage_);
	}

	/// Removes the parameters from the storage and resets them to their defaults.
	/// @return false if removing failed
	bool
	erase()
	{
		return canard::eraseParameters(parameters_, storage_);
	}

private:
	struct Request
	{
		bool pending;
		uint8_t client;
		uint8_t transfer_id;
		canard_prio_t priority;
	};

	using Callback = void(*)(canard_subscription_t*, canard_us_t, canard_prio_t,
							 uint_least8_t, uint_least8_t, canard_payload_t);

	template< Callback callback >
	bool
	subscribe(canard_subscription_t& subscription, uint16_t service_id, size_t extent)
	{
		static constexpr canard_subscription_vtable_t vtable{callback};
		if (canard_subscribe_request(&canard, &subscription, service_id, extent,
				CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_us, &vtable) != &subscription) return false;
		subscription.user_context = this;
		return true;
	}

	static Node&
	self(canard_subscription_t* subscription)
	{
		return *static_cast<Node*>(subscription->user_context);
	}

	// The callbacks must not call libcanard, so they only store the requests
	// for serve(). A request that arrives before the previous one was answered
	// is dropped, which only affects clients that send several requests at once.
	// The requests are decoded in place, since the register requests have more
	// than 500 bytes.
	template< class T >
	static void
	store(canard_subscription_t* sub, const canard_payload_t& payload, T& value, Request& request,
		  canard_prio_t priority, uint8_t client, uint8_t transfer_id,
		  int8_t (*deserialize)(T*, const uint8_t*, size_t*))
	{
		size_t size = payload.view.size;
		if (not request.pending and
			deserialize(&value, static_cast<const uint8_t*>(payload.view.data), &size) >= 0)
			request = {true, client, transfer_id, priority};
		canard::release(sub, payload);
	}

	static void
	onGetInfo(canard_subscription_t* sub, canard_us_t, canard_prio_t priority,
			  uint_least8_t client, uint_least8_t transfer_id, canard_payload_t payload)
	{
		self(sub).info_request_ = {true, uint8_t(client), uint8_t(transfer_id), priority};
		canard::release(sub, payload);
	}

	static void
	onExecuteCommand(canard_subscription_t* sub, canard_us_t, canard_prio_t priority,
					 uint_least8_t client, uint_least8_t transfer_id, canard_payload_t payload)
	{
		Node& node = self(sub);
		store(sub, payload, node.command_, node.command_request_, priority, client, transfer_id,
			  uavcan_node_ExecuteCommand_Request_1_3_deserialize_);
	}

	static void
	onAccess(canard_subscription_t* sub, canard_us_t, canard_prio_t priority,
			 uint_least8_t client, uint_least8_t transfer_id, canard_payload_t payload)
	{
		Node& node = self(sub);
		store(sub, payload, node.access_, node.access_request_, priority, client, transfer_id,
			  uavcan_register_Access_Request_1_0_deserialize_);
	}

	static void
	onList(canard_subscription_t* sub, canard_us_t, canard_prio_t priority,
		   uint_least8_t client, uint_least8_t transfer_id, canard_payload_t payload)
	{
		Node& node = self(sub);
		store(sub, payload, node.list_, node.list_request_, priority, client, transfer_id,
			  uavcan_register_List_Request_1_0_deserialize_);
	}

	template< class T >
	bool
	respond(Request& request, uint16_t service_id, const T& response,
			int8_t (*serialize)(const T*, uint8_t*, size_t*))
	{
		request.pending = false;
		size_t size = sizeof(buffer_);
		if (serialize(&response, buffer_, &size) < 0) return false;
		// libcanard copies the payload
		return canard_respond(&canard, canard::now() + 1'000'000, request.priority, service_id,
							  request.client, request.transfer_id, {{size, buffer_}, nullptr}, nullptr);
	}

	void
	respondInfo()
	{
		uavcan_node_GetInfo_Response_1_0 response{};
		response.protocol_version = {1, 0};
		response.hardware_version = {info_.hardware_major, info_.hardware_minor};
		response.software_version = {info_.software_major, info_.software_minor};
		response.software_vcs_revision_id = info_.vcs_revision;
		std::ranges::copy(info_.unique_id, response.unique_id);
		response.name.count = std::min(info_.name.size(), sizeof(response.name.elements));
		std::memcpy(response.name.elements, info_.name.data(), response.name.count);
		respond(info_request_,
				uavcan_node_GetInfo_1_0_FIXED_PORT_ID_, response, uavcan_node_GetInfo_Response_1_0_serialize_);
	}

	void
	respondCommand(canard_us_t now)
	{
		uavcan_node_ExecuteCommand_Response_1_3 response{};
		response.status = uavcan_node_ExecuteCommand_Response_1_3_STATUS_BAD_COMMAND;
		bool restart{};
		switch (command_.command)
		{
			case uavcan_node_ExecuteCommand_Request_1_3_COMMAND_RESTART:
				if (restart_)
				{
					response.status = uavcan_node_ExecuteCommand_Response_1_3_STATUS_SUCCESS;
					restart = true;
				}
				break;
			case uavcan_node_ExecuteCommand_Request_1_3_COMMAND_STORE_PERSISTENT_STATES:
				if (storage_) response.status = save() ? uavcan_node_ExecuteCommand_Response_1_3_STATUS_SUCCESS :
														 uavcan_node_ExecuteCommand_Response_1_3_STATUS_FAILURE;
				break;
			case uavcan_node_ExecuteCommand_Request_1_3_COMMAND_FACTORY_RESET:
				response.status = erase() ? uavcan_node_ExecuteCommand_Response_1_3_STATUS_SUCCESS :
											uavcan_node_ExecuteCommand_Response_1_3_STATUS_FAILURE;
				break;
		}
		// restart once the response is sent and had time to leave the driver
		if (respond(command_request_,
				uavcan_node_ExecuteCommand_1_3_FIXED_PORT_ID_, response,
				uavcan_node_ExecuteCommand_Response_1_3_serialize_) and restart)
			restart_deadline_ = now + 100'000;
	}

	void
	respondAccess()
	{
		const std::string_view name(reinterpret_cast<const char*>(access_.name.name.elements), access_.name.name.count);
		const auto it = std::ranges::find(parameters_, name, &canard::Parameter::name);
		// an unknown register is answered with an empty value
		uavcan_register_Access_Response_1_0 response{};
		uavcan_register_Value_1_0_select_empty_(&response.value);
		if (it != parameters_.end())
		{
			// the response contains the value after the write, also if it was refused
			if (not uavcan_register_Value_1_0_is_empty_(&access_.value)) setRegister(**it, access_.value);
			response._mutable = true;
			response.persistent = storage_ != nullptr;
			getRegister(**it, response.value);
		}
		respond(access_request_,
				uavcan_register_Access_1_0_FIXED_PORT_ID_, response, uavcan_register_Access_Response_1_0_serialize_);
	}

	void
	respondList()
	{
		// an index past the end is answered with an empty name
		uavcan_register_List_Response_1_0 response{};
		if (list_.index < parameters_.size())
		{
			const std::string_view name = parameters_[list_.index]->name();
			response.name.name.count = std::min(name.size(), sizeof(response.name.name.elements));
			std::memcpy(response.name.name.elements, name.data(), response.name.name.count);
		}
		respond(list_request_,
				uavcan_register_List_1_0_FIXED_PORT_ID_, response, uavcan_register_List_Response_1_0_serialize_);
	}

	NodeInfo info_{};
	std::span<canard::Parameter* const> parameters_{};
	canard::ParameterStorage* storage_{};
	void (*restart_)(){};
	canard_us_t start_{};

	uint8_t health_{uavcan_node_Health_1_0_NOMINAL};
	uint8_t mode_{uavcan_node_Mode_1_0_OPERATIONAL};
	uint8_t vendor_status_{};
	canard_us_t heartbeat_deadline_{};
	uint8_t heartbeat_transfer_id_{};

	canard_subscription_t info_sub_{}, command_sub_{}, access_sub_{}, list_sub_{};
	Request info_request_{}, command_request_{}, access_request_{}, list_request_{};
	uavcan_node_ExecuteCommand_Request_1_3 command_{};
	uavcan_register_Access_Request_1_0 access_{};
	uavcan_register_List_Request_1_0 list_{};
	canard_us_t restart_deadline_{};
	// encodes the responses, the largest is GetInfo
	uint8_t buffer_[std::max({uavcan_node_GetInfo_Response_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_,
							  uavcan_node_ExecuteCommand_Response_1_3_SERIALIZATION_BUFFER_SIZE_BYTES_,
							  uavcan_register_Access_Response_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_,
							  uavcan_register_List_Response_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_})];
};

}	// namespace modm::cyphal
