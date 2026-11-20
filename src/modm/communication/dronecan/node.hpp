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
#include <modm/ext/canard/parameter.hpp>
#include <uavcan.protocol.NodeStatus.h>
#include <uavcan.protocol.GetNodeInfo.h>
#include <uavcan.protocol.RestartNode.h>
#include <uavcan.protocol.param.GetSet.h>
#include <uavcan.protocol.param.ExecuteOpcode.h>
#include <uavcan.protocol.dynamic_node_id.Allocation.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <span>
#include <string_view>

namespace modm::dronecan
{

/// Identity of a node for the uavcan.protocol.GetNodeInfo service
struct NodeInfo
{
	/// Reversed internet domain name, e.g. "io.modm.example", at most 80
	/// characters of a-z, 0-9, dot, dash and underscore.
	std::string_view name;
	/// Unique ID of the device, e.g. its UID padded with zeros, which is
	/// also used for the dynamic node-ID allocation.
	std::array<uint8_t, 16> unique_id;
	uint8_t software_major{};
	uint8_t software_minor{};
	/// Optional revision of the version control system, e.g. a Git hash
	uint32_t vcs_commit{};
	uint8_t hardware_major{};
	uint8_t hardware_minor{};
};

/// Sets a parameter from a GetSet request: an integer, real or boolean value
/// as the type of the parameter, or an integer also for real and boolean.
/// @return false if the type does not match or the value is out of range
inline bool
setParameter(canard::Parameter& parameter, const uavcan_protocol_param_Value& value)
{
	using Type = canard::Parameter::Type;
	switch (value.union_tag)
	{
		case UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE:
			if (parameter.type() == Type::Boolean)
			{
				if (value.integer_value != 0 and value.integer_value != 1) return false;
				return parameter.set(value.integer_value == 1);
			}
			return parameter.set(value.integer_value);
		case UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE:
			return parameter.set(value.real_value);
		case UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE:
			return parameter.set(value.boolean_value != 0);
		default:
			return false;
	}
}

/// A parameter value as GetSet value
inline uavcan_protocol_param_Value
parameterValue(const canard::Parameter& parameter, canard::Parameter::Data data)
{
	uavcan_protocol_param_Value value{};
	switch (parameter.type())
	{
		case canard::Parameter::Type::Integer:
			value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
			value.integer_value = data.integer;
			break;
		case canard::Parameter::Type::Real:
			value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
			value.real_value = data.real;
			break;
		case canard::Parameter::Type::Boolean:
			value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE;
			value.boolean_value = data.boolean;
			break;
	}
	return value;
}

/// Fills the value, default, range and name of a GetSet response.
inline void
getParameter(const canard::Parameter& parameter, uavcan_protocol_param_GetSetResponse& response)
{
	using Type = canard::Parameter::Type;
	response = {};
	response.value = parameterValue(parameter, parameter.value());
	response.default_value = parameterValue(parameter, parameter.defaultValue());
	if (parameter.type() == Type::Integer)
	{
		response.min_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE;
		response.min_value.integer_value = parameter.min().integer;
		response.max_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE;
		response.max_value.integer_value = parameter.max().integer;
	}
	else if (parameter.type() == Type::Real)
	{
		response.min_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE;
		response.min_value.real_value = parameter.min().real;
		response.max_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE;
		response.max_value.real_value = parameter.max().real;
	}
	const size_t length = std::min(parameter.name().size(), sizeof(response.name.data));
	std::memcpy(response.name.data, parameter.name().data(), length);
	response.name.len = length;
}

/// Configuration of modm::dronecan::Node
struct NodeConfig
{
	/// Node-ID 1-125, or 126-127 for maintenance tools like a GUI tool or a
	/// test fixture, or 0 to request one with dynamic node-ID allocation.
	uint8_t node_id = 0;
	/// Preferred node-ID of the dynamic node-ID allocation, 0 for any.
	uint8_t preferred_node_id = 0;
	/// Parameters of the GetSet service in a stable order
	std::span<canard::Parameter* const> parameters{};
	/// Saves the parameters with the ExecuteOpcode service, or nullptr to keep
	/// them only in RAM. Load them with canard::loadParameters() before.
	canard::ParameterStorage* storage = nullptr;
	/// Called by the RestartNode service after the response was sent, e.g.
	/// NVIC_SystemReset(), or nullptr to refuse restarts.
	void (*restart)() = nullptr;
	/// Configuration of the libcanard node, `keep_node_id` is always set.
	canard::NodeConfig node{};
};

/**
 * A DroneCAN node with the standard services of the uavcan.protocol namespace.
 *
 * - publishes uavcan.protocol.NodeStatus once per second,
 * - answers uavcan.protocol.GetNodeInfo and RestartNode,
 * - gets its node-ID by dynamic node-ID allocation (uavcan.protocol.dynamic_node_id.Allocation),
 * - and gives access to its parameters with uavcan.protocol.param.GetSet and
 *   ExecuteOpcode, which saves them to a storage or erases them.
 *
 * Use it like modm::canard::Node for the other data types of the application.
 * A node keeps its node-ID, even if another node uses it too.
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
		preferred_node_id_ = config.preferred_node_id;
		start_ = canard::now();

		uint64_t seed{};
		for (const uint8_t byte : info.unique_id) seed = (seed ^ byte) * 0x100000001b3ull;
		random_ = seed | 1;

		config.node.keep_node_id = true;
		if (config.node_id > CANARD_NODE_ID_MAX) return false;
		if (not Base::initialize(config.node_id ? config.node_id : CANARD_NODE_ID_ANONYMOUS, seed, config.node))
			return false;

		using GetNodeInfo = uavcan_protocol_GetNodeInfoRequest;
		using RestartNode = uavcan_protocol_RestartNodeRequest;
		using GetSet = uavcan_protocol_param_GetSetRequest;
		using ExecuteOpcode = uavcan_protocol_param_ExecuteOpcodeRequest;
		if (not subscribe<GetNodeInfo, store<&Node::info_request_, nullptr>>(info_request_sub_) or
			not subscribe<RestartNode, store<&Node::restart_request_, &Node::restart_magic_>>(restart_sub_) or
			not subscribe<GetSet, store<&Node::getset_request_, &Node::getset_>>(getset_sub_) or
			not subscribe<ExecuteOpcode, store<&Node::opcode_request_, &Node::opcode_>>(opcode_sub_))
			return false;
		if (config.node_id == 0)
		{
			if (not subscribe<uavcan_protocol_dynamic_node_id_Allocation, onAllocation>(allocation_sub_))
				return false;
			allocating_ = true;
			request_deadline_ = canard::now() + randomMs(RequestPeriodMin, RequestPeriodMax);
		}
		return true;
	}

	/// Receives and transmits frames, runs the services and publishes the
	/// node status. Call this every few milliseconds.
	void
	update()
	{
		Base::update();
		serve();
		Base::poll();
	}

	/// Runs the services and publishes the node status without receiving or
	/// transmitting, for applications that pass the frames with ingest() and
	/// transmit them with poll() themselves.
	void
	serve()
	{
		const canard_us_t now = canard::now();
		if (allocating_) updateAllocation(now);
		if (nodeId() == 0) return;

		if (info_request_.pending) respondNodeInfo();
		if (getset_request_.pending) respondGetSet();
		if (opcode_request_.pending) respondExecuteOpcode();
		if (restart_request_.pending) respondRestart(now);

		if (now >= status_deadline_)
		{
			status_deadline_ = now + 1'000'000;
			const uavcan_protocol_NodeStatus status = this->status();
			dronecan::publish(&canard, status, status_transfer_id_, canard_prio_low);
		}
		// restart once the transmit queue is empty, or after a second if it does not drain
		if (restart_deadline_ and now >= restart_deadline_ and
			(canard_pending_ifaces(&canard) == 0 or now >= restart_deadline_ + 1'000'000))
		{
			restart_deadline_ = 0;
			restart_();
		}
	}

	/// Node-ID, or 0 while the dynamic node-ID allocation is running
	uint8_t
	nodeId() const
	{
		return allocating_ ? 0 : canard.node_id;
	}

	void setHealth(uint8_t health) { health_ = health; }
	void setMode(uint8_t mode) { mode_ = mode; }
	void setVendorStatus(uint16_t code) { vendor_status_ = code; }

	/// Current uavcan.protocol.NodeStatus
	uavcan_protocol_NodeStatus
	status() const
	{
		uavcan_protocol_NodeStatus status{};
		status.uptime_sec = (canard::now() - start_) / 1'000'000;
		status.health = health_;
		status.mode = mode_;
		status.vendor_specific_status_code = vendor_status_;
		return status;
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
	// Dynamic node-ID allocation never assigns the node-IDs of maintenance tools
	static constexpr uint8_t MaxNodeId = 125;
	static constexpr uint32_t RequestPeriodMin = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MIN_REQUEST_PERIOD_MS;
	static constexpr uint32_t RequestPeriodMax = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_REQUEST_PERIOD_MS;
	static constexpr uint32_t FollowupDelayMax = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_FOLLOWUP_DELAY_MS;
	static constexpr uint8_t StageLength = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_LENGTH_OF_UNIQUE_ID_IN_REQUEST;

	struct Request
	{
		bool pending;
		uint8_t client;
		uint8_t transfer_id;
		canard_prio_t priority;
	};

	/// @return false if libcanard is out of memory or the application
	///         already subscribed to the type
	template< class T, auto callback >
	bool
	subscribe(canard_subscription_t& subscription)
	{
		return dronecan::subscribe<T, callback>(&canard, &subscription, this);
	}

	// The callbacks must not call libcanard, so they only store the requests
	// for serve(). A request that arrives before the previous one was
	// answered replaces it, the client repeats it after its timeout.
	template< auto request, auto value >
	static constexpr auto store = [](const Transfer& transfer, const auto& decoded)
	{
		Node& node = *static_cast<Node*>(transfer.context);
		if constexpr (value != nullptr) node.*value = decoded;
		node.*request = {true, transfer.source, transfer.transfer_id, transfer.priority};
	};

	void
	respondNodeInfo()
	{
		info_request_.pending = false;
		uavcan_protocol_GetNodeInfoResponse response{};
		response.status = status();
		response.software_version.major = info_.software_major;
		response.software_version.minor = info_.software_minor;
		if (info_.vcs_commit)
		{
			response.software_version.optional_field_flags =
					UAVCAN_PROTOCOL_SOFTWAREVERSION_OPTIONAL_FIELD_FLAG_VCS_COMMIT;
			response.software_version.vcs_commit = info_.vcs_commit;
		}
		response.hardware_version.major = info_.hardware_major;
		response.hardware_version.minor = info_.hardware_minor;
		std::ranges::copy(info_.unique_id, response.hardware_version.unique_id);
		response.name.len = std::min(info_.name.size(), sizeof(response.name.data));
		std::memcpy(response.name.data, info_.name.data(), response.name.len);
		respond(info_request_, response);
	}

	void
	respondGetSet()
	{
		getset_request_.pending = false;
		canard::Parameter* parameter{};
		if (getset_.name.len)
		{
			const std::string_view name(reinterpret_cast<const char*>(getset_.name.data), getset_.name.len);
			const auto it = std::ranges::find(parameters_, name, &canard::Parameter::name);
			if (it != parameters_.end()) parameter = *it;
		}
		else if (getset_.index < parameters_.size()) parameter = parameters_[getset_.index];

		// an unknown parameter is answered with an empty name and value
		uavcan_protocol_param_GetSetResponse response{};
		if (parameter)
		{
			// the response contains the value after the set, also if it was refused
			if (getset_.value.union_tag != UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY) setParameter(*parameter, getset_.value);
			getParameter(*parameter, response);
		}
		respond(getset_request_, response);
	}

	void
	respondExecuteOpcode()
	{
		opcode_request_.pending = false;
		uavcan_protocol_param_ExecuteOpcodeResponse response{};
		if (opcode_.opcode == UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_REQUEST_OPCODE_SAVE) response.ok = save();
		else if (opcode_.opcode == UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_REQUEST_OPCODE_ERASE) response.ok = erase();
		respond(opcode_request_, response);
	}

	void
	respondRestart(canard_us_t now)
	{
		restart_request_.pending = false;
		uavcan_protocol_RestartNodeResponse response{};
		response.ok = restart_ and restart_magic_.magic_number == UAVCAN_PROTOCOL_RESTARTNODE_REQUEST_MAGIC_NUMBER;
		// restart once the response is sent and had time to leave the driver
		if (respond(restart_request_, response) and response.ok) restart_deadline_ = now + 100'000;
	}

	template< class T >
	bool
	respond(const Request& request, const T& response)
	{
		return dronecan::respond(&canard, request.client, request.transfer_id, response, request.priority);
	}

	// Dynamic node-ID allocation client, rules A-E of uavcan.protocol.dynamic_node_id.Allocation
	static void
	onAllocation(const Transfer& transfer, const uavcan_protocol_dynamic_node_id_Allocation& allocation)
	{
		Node& node = *static_cast<Node*>(transfer.context);
		if (not node.allocating_) return;

		const canard_us_t now = canard::now();
		// Rule C: any allocation message restarts the request timer and
		// cancels a pending follow-up
		node.request_deadline_ = now + node.randomMs(RequestPeriodMin, RequestPeriodMax);
		node.followup_deadline_ = 0;
		if (transfer.source == CANARD_NODE_ID_ANONYMOUS or allocation.unique_id.len > 16 or
			not std::equal(allocation.unique_id.data, allocation.unique_id.data + allocation.unique_id.len,
						   node.info_.unique_id.begin()))
			return;
		// Rule E: the allocation is complete
		if (allocation.unique_id.len == 16)
		{
			if (allocation.node_id and allocation.node_id <= MaxNodeId)
				node.allocated_node_id_ = allocation.node_id;
			return;
		}
		// Rule D: send the next part of the unique ID after a random delay
		node.followup_offset_ = allocation.unique_id.len;
		node.followup_deadline_ = now + node.randomMs(0, FollowupDelayMax);
	}

	void
	updateAllocation(canard_us_t now)
	{
		if (allocated_node_id_)
		{
			allocating_ = false;
			canard_unsubscribe(&canard, &allocation_sub_);
			canard_set_node_id(&canard, allocated_node_id_);
			status_deadline_ = now;
			return;
		}
		// Rule B: send the first part of the unique ID periodically
		if (now >= request_deadline_)
		{
			request_deadline_ = now + randomMs(RequestPeriodMin, RequestPeriodMax);
			followup_deadline_ = 0;
			requestAllocation(0, true);
		}
		else if (followup_deadline_ and now >= followup_deadline_)
		{
			followup_deadline_ = 0;
			requestAllocation(followup_offset_, false);
		}
	}

	void
	requestAllocation(uint8_t offset, bool first)
	{
		uavcan_protocol_dynamic_node_id_Allocation request{};
		request.node_id = preferred_node_id_;
		request.first_part_of_unique_id = first;
		request.unique_id.len = std::min<uint8_t>(StageLength, 16 - offset);
		std::memcpy(request.unique_id.data, info_.unique_id.data() + offset, request.unique_id.len);
		dronecan::publishAnonymous(*this, request, allocation_transfer_id_, canard_prio_low);
	}

	/// Random microseconds between min and max milliseconds (xorshift64)
	canard_us_t
	randomMs(uint32_t min, uint32_t max)
	{
		random_ ^= random_ << 13;
		random_ ^= random_ >> 7;
		random_ ^= random_ << 17;
		return canard_us_t(min + random_ % (max - min + 1)) * 1000;
	}

	NodeInfo info_{};
	std::span<canard::Parameter* const> parameters_{};
	canard::ParameterStorage* storage_{};
	void (*restart_)(){};
	canard_us_t start_{};
	uint64_t random_{1};

	uint8_t health_{UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK};
	uint8_t mode_{UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL};
	uint16_t vendor_status_{};
	canard_us_t status_deadline_{};
	uint8_t status_transfer_id_{};

	canard_subscription_t info_request_sub_{}, restart_sub_{}, getset_sub_{}, opcode_sub_{};
	Request info_request_{}, restart_request_{}, getset_request_{}, opcode_request_{};
	uavcan_protocol_RestartNodeRequest restart_magic_{};
	uavcan_protocol_param_GetSetRequest getset_{};
	uavcan_protocol_param_ExecuteOpcodeRequest opcode_{};
	canard_us_t restart_deadline_{};

	canard_subscription_t allocation_sub_{};
	bool allocating_{};
	uint8_t preferred_node_id_{};
	uint8_t allocated_node_id_{};
	uint8_t followup_offset_{};
	uint8_t allocation_transfer_id_{};
	canard_us_t request_deadline_{};
	canard_us_t followup_deadline_{};
};

}	// namespace modm::dronecan
