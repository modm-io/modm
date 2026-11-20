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

// Two DroneCAN nodes on one CAN controller in loopback mode, no transceiver
// needed. The server (node-ID 42) allocates node-IDs dynamically, broadcasts
// uavcan.protocol.NodeStatus and answers uavcan.protocol.GetNodeInfo requests,
// whose response is a multi-frame transfer. The client starts without node-ID
// and requests one with anonymous uavcan.protocol.dynamic_node_id.Allocation
// messages, then receives the node status and requests the node info.

#include <modm/board.hpp>
#include <modm/ext/dronecan/dronecan.hpp>
#include <modm/processing/timer.hpp>
#include <uavcan.protocol.NodeStatus.h>
#include <uavcan.protocol.GetNodeInfo.h>
#include <uavcan.protocol.dynamic_node_id.Allocation.h>
#include <algorithm>
#include <cstring>

using namespace modm::literals;
using namespace std::chrono_literals;
namespace dronecan = modm::dronecan;
using Allocation = uavcan_protocol_dynamic_node_id_Allocation;
using NodeStatus = uavcan_protocol_NodeStatus;
using GetNodeInfoRequest = uavcan_protocol_GetNodeInfoRequest;
using GetNodeInfoResponse = uavcan_protocol_GetNodeInfoResponse;

#if defined(FDCAN1)
using Can = Fdcan1;
#elif __has_include(<modm/platform/can/can_1.hpp>)
using Can = Can1;
#else
using Can = modm::platform::Can;
#endif

constexpr uint8_t ServerId = 42;
constexpr uint8_t AllocatedId = 125;
// Normally the unique ID of the device
constexpr uint8_t UniqueId[16]{0x6d, 0x6f, 0x64, 0x6d, 0x2e, 0x69, 0x6f, 0x2e,
							   0x64, 0x72, 0x6f, 0x6e, 0x65, 0x63, 0x61, 0x6e};

modm::canard::Node<Can> server;
modm::canard::Node<Can> client;
uint32_t statuses{}, infos{}, errors{};

// Callbacks must not call libcanard, so they copy what the main loop handles
struct Received
{
	bool pending;
	uint8_t source;
	uint8_t transfer_id;
};
Received info_request{}, allocation_request{}, allocation_response{};
Allocation server_allocation{}, client_allocation{};

static void
onNodeStatus(canard_subscription_t* sub, canard_us_t, canard_prio_t, uint_least8_t source,
			 uint_least8_t, canard_payload_t payload)
{
	NodeStatus status;
	if (dronecan::decode(payload, status))
	{
		statuses++;
		MODM_LOG_INFO.printf("NodeStatus from %u: uptime %lu s, health %u\n",
							 source, status.uptime_sec, status.health);
	}
	else errors++;
	modm::canard::release(sub, payload);
}

static void
onGetNodeInfoRequest(canard_subscription_t* sub, canard_us_t, canard_prio_t, uint_least8_t source,
					 uint_least8_t transfer_id, canard_payload_t payload)
{
	info_request = {true, source, transfer_id};
	modm::canard::release(sub, payload);
}

static void
onGetNodeInfoResponse(canard_subscription_t* sub, canard_us_t, canard_prio_t, uint_least8_t source,
					  uint_least8_t, canard_payload_t payload)
{
	GetNodeInfoResponse info;
	if (dronecan::decode(payload, info))
	{
		infos++;
		MODM_LOG_INFO.printf("GetNodeInfo from %u: %.*s v%u.%u (%u bytes)\n", source,
							 info.name.len, (const char*)info.name.data,
							 info.software_version.major, info.software_version.minor, payload.view.size);
	}
	else errors++;
	modm::canard::release(sub, payload);
}

// The server only accepts allocation requests from anonymous nodes and the
// client only accepts responses from servers
static void
onServerAllocation(canard_subscription_t* sub, canard_us_t, canard_prio_t, uint_least8_t source,
				   uint_least8_t, canard_payload_t payload)
{
	if (source == CANARD_NODE_ID_ANONYMOUS and dronecan::decode(payload, server_allocation))
		allocation_request = {true, source, 0};
	modm::canard::release(sub, payload);
}

static void
onClientAllocation(canard_subscription_t* sub, canard_us_t, canard_prio_t, uint_least8_t source,
				   uint_least8_t, canard_payload_t payload)
{
	if (source != CANARD_NODE_ID_ANONYMOUS and dronecan::decode(payload, client_allocation))
		allocation_response = {true, source, 0};
	modm::canard::release(sub, payload);
}

constexpr canard_subscription_vtable_t nodeStatusVtable{onNodeStatus};
constexpr canard_subscription_vtable_t requestVtable{onGetNodeInfoRequest};
constexpr canard_subscription_vtable_t responseVtable{onGetNodeInfoResponse};
constexpr canard_subscription_vtable_t serverAllocationVtable{onServerAllocation};
constexpr canard_subscription_vtable_t clientAllocationVtable{onClientAllocation};
canard_subscription_t nodeStatusSubscription, requestSubscription, responseSubscription,
					  serverAllocationSubscription, clientAllocationSubscription;

static NodeStatus
nodeStatus(uint32_t uptime)
{
	NodeStatus status{};
	status.uptime_sec = uptime;
	status.health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK;
	status.mode = UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL;
	return status;
}

// ----------------------------------------------------------------------------
// Dynamic node-ID allocation server, which collects the unique ID of a client
// in stages of up to 6 bytes and repeats the received part to the client.
uint8_t server_unique_id[16];
uint8_t server_unique_id_length{};
uint8_t server_transfer_id{};

static void
serveAllocation()
{
	const Allocation& request = server_allocation;
	if (request.first_part_of_unique_id) server_unique_id_length = 0;
	else if (server_unique_id_length == 0) return;
	const uint8_t length = std::min<uint8_t>(request.unique_id.len, 16 - server_unique_id_length);
	std::memcpy(server_unique_id + server_unique_id_length, request.unique_id.data, length);
	server_unique_id_length += length;

	Allocation response{};
	response.unique_id.len = server_unique_id_length;
	std::memcpy(response.unique_id.data, server_unique_id, server_unique_id_length);
	if (server_unique_id_length == 16)
	{
		// A real server looks up the unique ID in its allocation table
		response.node_id = AllocatedId;
		server_unique_id_length = 0;
	}
	if (not dronecan::publish(&server.canard, response, server_transfer_id)) errors++;
}

// ----------------------------------------------------------------------------
// Dynamic node-ID allocation client
modm::Timeout allocation_timer{1s};
uint8_t client_unique_id_offset{};
uint8_t client_transfer_id{};
bool allocated{};

static void
requestAllocation()
{
	Allocation request{};
	request.first_part_of_unique_id = (client_unique_id_offset == 0);
	request.unique_id.len = std::min(UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_MAX_LENGTH_OF_UNIQUE_ID_IN_REQUEST,
									 16 - client_unique_id_offset);
	std::memcpy(request.unique_id.data, UniqueId + client_unique_id_offset, request.unique_id.len);
	if (not dronecan::publishAnonymous(client, request, client_transfer_id, canard_prio_low)) errors++;
}

static void
updateAllocation()
{
	if (allocation_response.pending)
	{
		allocation_response.pending = false;
		const Allocation& response = client_allocation;
		// a response for a different unique ID restarts the allocation
		if (std::memcmp(response.unique_id.data, UniqueId, response.unique_id.len))
		{
			client_unique_id_offset = 0;
			return;
		}
		if (response.node_id and response.unique_id.len == 16)
		{
			allocated = canard_set_node_id(&client.canard, response.node_id);
			MODM_LOG_INFO.printf("Allocated node-ID %u from %u\n", response.node_id, allocation_response.source);
			return;
		}
		// send the next stage after a random delay below 400ms
		client_unique_id_offset = response.unique_id.len;
		allocation_timer.restart(std::chrono::milliseconds(modm::canard::now() % 400));
	}
	if (allocation_timer.execute())
	{
		requestAllocation();
		// restart if the server does not answer within 600-1000ms
		allocation_timer.restart(std::chrono::milliseconds(600 + modm::canard::now() % 400));
		client_unique_id_offset = 0;
	}
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "DroneCAN loopback example" << modm::endl;

#ifdef FDCAN1
	Can::initialize<Board::SystemClock, 500_kbps>(9, Can::Mode::TestInternalLoopback);
#else
	// bxCAN needs a recessive CANRX pin to leave the initialization mode
	Can::connect<GpioA11::Rx, GpioA12::Tx>(Gpio::InputType::PullUp);
	if (not Can::initialize<Board::SystemClock, 500_kbps>(9, Can::Mode::ListenOnlyLoopBack))
		errors++;
#endif
	Can::setExtendedFilter(0, Can::FilterConfig::Fifo0,
			modm::can::ExtendedIdentifier(0), modm::can::ExtendedMask(0));

	// Both nodes share one controller, so libcanard must not configure the
	// filters, and each node must ignore the frames it sent itself.
	constexpr modm::canard::NodeConfig config{.filters = false, .loopback = true};
	if (not server.initialize(ServerId, 0x4242'4242'4242'4242, config)) errors++;
	// The client keeps the random node-ID of libcanard until allocation, but
	// does not send anything with it.
	if (not client.initialize(CANARD_NODE_ID_ANONYMOUS, 0x1234'5678'9abc'def0, config)) errors++;

	dronecan::subscribe<Allocation>(&server.canard, &serverAllocationSubscription, &serverAllocationVtable);
	dronecan::subscribe<GetNodeInfoRequest>(&server.canard, &requestSubscription, &requestVtable);
	dronecan::subscribe<Allocation>(&client.canard, &clientAllocationSubscription, &clientAllocationVtable);
	dronecan::subscribe<NodeStatus>(&client.canard, &nodeStatusSubscription, &nodeStatusVtable);
	dronecan::subscribe<GetNodeInfoResponse>(&client.canard, &responseSubscription, &responseVtable);

	modm::PeriodicTimer timer{1s};
	uint32_t uptime{};
	uint8_t status_transfer_id{}, request_transfer_id{};
	while (true)
	{
		// Read the controller once and pass all frames to both nodes
		modm::can::Message message;
		while (Can::getMessage(message))
		{
			server.ingest(message);
			client.ingest(message);
		}
		server.poll();
		client.poll();

		if (allocation_request.pending)
		{
			allocation_request.pending = false;
			serveAllocation();
		}
		if (not allocated) updateAllocation();

		if (info_request.pending)
		{
			info_request.pending = false;
			GetNodeInfoResponse info{};
			info.status = nodeStatus(uptime);
			info.software_version.major = 1;
			info.software_version.minor = 2;
			constexpr char name[] = "io.modm.example.dronecan";
			info.name.len = sizeof(name) - 1;
			std::memcpy(info.name.data, name, info.name.len);
			if (not dronecan::respond(&server.canard, info_request.source, info_request.transfer_id, info))
				errors++;
		}

		if (timer.execute())
		{
			MODM_LOG_INFO.printf("node-ID %u, status %lu info %lu errors %lu\n",
								 allocated ? client.canard.node_id : 0, statuses, infos,
								 errors + uint32_t(server.canard.err.tx_expiration +
								 client.canard.err.rx_transfer + server.canard.err.rx_transfer));

			if (not dronecan::publish(&server.canard, nodeStatus(uptime), status_transfer_id)) errors++;
			// The client needs a node-ID to send requests
			if (allocated and (uptime % 3 == 2))
			{
				if (not dronecan::request(&client.canard, ServerId, GetNodeInfoRequest{}, request_transfer_id))
					errors++;
			}
			uptime++;
		}
	}
}
