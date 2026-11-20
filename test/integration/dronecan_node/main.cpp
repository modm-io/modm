/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// Tests modm::dronecan::Node with persistent parameters in ZMS over three
// boots. A second node (127) in the same firmware plays the DroneCAN GUI tool
// with a node-ID allocation server, both on one CAN controller in loopback.
//
// 1. The node gets node-ID 125 by dynamic allocation. The tool checks the node
//    status, the node info, lists and sets the parameters, also out of range and
//    with the wrong type, saves them and restarts the node.
// 2. The node starts with the stored node-ID 126 and parameters. It keeps its
//    node-ID when another node uses it. The tool erases the parameters and
//    restarts the node.
// 3. The node gets node-ID 125 by dynamic allocation again, with the defaults.

#include <modm/board.hpp>
#include <modm/communication/dronecan.hpp>
#include <modm/ext/canard/parameter_zms.hpp>
#include <modm/processing/timer.hpp>
#include <algorithm>
#include <cstring>

#include "../integration_test.hpp"

using namespace modm::literals;
using namespace std::chrono_literals;
namespace dronecan = modm::dronecan;
using Allocation = uavcan_protocol_dynamic_node_id_Allocation;

#if defined(FDCAN1)
using Can = Fdcan1;
#elif __has_include(<modm/platform/can/can_1.hpp>)
using Can = Can1;
#else
using Can = modm::platform::Can;
#endif

extern "C" uint32_t __main_stack_bottom[], __main_stack_top[];

// compiles all members, also call(), which needs fibers and is not used here
template class modm::dronecan::Client<uavcan_protocol_param_GetSetRequest>;

namespace
{

constexpr uint8_t ToolId = 127;
constexpr std::array<uint8_t, 16> UniqueId{0x6d, 0x6f, 0x64, 0x6d, 0x2e, 0x69, 0x6f, 0x2e,
										   0x6e, 0x6f, 0x64, 0x65, 0x74, 0x65, 0x73, 0x74};
constexpr char Name[] = "io.modm.test.node";

// survives the restarts of the node
struct State
{
	uint32_t magic;
	uint32_t boot;
	bool failed;
};
modm_section(".noinit") State state;
constexpr uint32_t Magic = 0xd20'eca1;

modm::canard::Parameter node_id{"uavcan.node_id", 0, 0, 127};
modm::canard::Parameter rate{"test.rate", 10, 1, 100};
modm::canard::Parameter gain{"test.gain", 0.5f, -10.f, 10.f};
modm::canard::Parameter enable{"test.enable", true};
modm::canard::Parameter* const parameters[]{&node_id, &rate, &gain, &enable};

zms_fs fs;
dronecan::Node<Can> node;
modm::canard::Node<Can> tool;
bool passed{true};

#define CHECK(cond, ...) \
	do { if (not (cond)) { passed = false; \
		MODM_LOG_ERROR.printf("ERROR %s:%d: " #cond " ", __FILE__, __LINE__); \
		MODM_LOG_ERROR.printf(__VA_ARGS__); MODM_LOG_ERROR << modm::endl; } } while(0)

// ----------------------------------------------------------------------------
// Messages to the tool, stored by the typed subscriptions
template< class T >
struct Received
{
	bool received;
	uint8_t source;
	T value;
	canard_subscription_t subscription;

	static void
	store(const dronecan::Transfer& transfer, const T& value)
	{
		auto& self = *static_cast<Received*>(transfer.context);
		self.received = true;
		self.source = transfer.source;
		self.value = value;
	}

	bool
	subscribe()
	{
		return dronecan::subscribe<T, store>(&tool.canard, &subscription, this);
	}
};
Received<uavcan_protocol_NodeStatus> status;
Received<Allocation> allocation;

// Clients of the services of the node
dronecan::Client<uavcan_protocol_GetNodeInfoRequest> info;
dronecan::Client<uavcan_protocol_param_GetSetRequest> getset;
dronecan::Client<uavcan_protocol_param_ExecuteOpcodeRequest> opcode;
dronecan::Client<uavcan_protocol_RestartNodeRequest> restart;

// ----------------------------------------------------------------------------
// Allocation server of the tool, see the DroneCAN example
uint8_t server_unique_id[16];
uint8_t server_unique_id_length{};
uint8_t server_transfer_id{};

void
serveAllocation()
{
	allocation.received = false;
	if (allocation.source != CANARD_NODE_ID_ANONYMOUS) return;
	const Allocation& request = allocation.value;
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
		response.node_id = request.node_id ? request.node_id : 125;
		server_unique_id_length = 0;
	}
	CHECK(dronecan::publish(&tool.canard, response, server_transfer_id), "allocation response");
}

// ----------------------------------------------------------------------------
void
pump()
{
	modm::can::Message message;
	while (Can::getMessage(message))
	{
		node.ingest(message);
		tool.ingest(message);
	}
	node.serve();
	node.poll();
	tool.poll();
	if (allocation.received) serveAllocation();
}

template< class F >
bool
waitUntil(F&& condition, std::chrono::milliseconds timeout)
{
	modm::Timeout timer{timeout};
	while (not condition())
	{
		if (timer.isExpired()) return false;
		pump();
	}
	return true;
}

/// Sends a request to the node and waits for the response.
/// Not inlined, so that the request does not add to the stack of the callers.
template< class Client, class Request >
[[gnu::noinline]] bool
call(Client& client, const Request& request)
{
	if (not client.request(node.nodeId(), request)) return false;
	return waitUntil([&] { return client.done(); }, 500ms);
}

// A small value for getSet(), since a Value has 136 bytes, and every temporary
// one would take its own space on the stack of the caller.
struct Set
{
	uint8_t tag{UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY};
	int64_t integer{};
	float real{};
};
Set integer(int64_t value) { return {UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE, value}; }
Set real(float value) { return {UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE, 0, value}; }
Set boolean(bool value) { return {UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE, value}; }

/// GetSet by name, or by index if the name is empty.
/// Not inlined, so that its request does not add to the stack of the callers.
[[gnu::noinline]] const uavcan_protocol_param_GetSetResponse&
getSet(std::string_view name, Set value = {}, uint16_t index = 0)
{
	uavcan_protocol_param_GetSetRequest request{};
	request.index = index;
	request.value.union_tag = value.tag;
	if (value.tag == UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE) request.value.integer_value = value.integer;
	if (value.tag == UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE) request.value.real_value = value.real;
	if (value.tag == UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE) request.value.boolean_value = value.integer;
	request.name.len = name.size();
	std::memcpy(request.name.data, name.data(), name.size());
	CHECK(call(getset, request), "GetSet %.*s", int(name.size()), name.data());
	return getset.response();
}

std::string_view
nameOf(const uavcan_protocol_param_GetSetResponse& response)
{
	return {reinterpret_cast<const char*>(response.name.data), response.name.len};
}

bool
executeOpcode(uint8_t code)
{
	uavcan_protocol_param_ExecuteOpcodeRequest request{};
	request.opcode = code;
	CHECK(call(opcode, request), "ExecuteOpcode %u", code);
	return opcode.response().ok;
}

size_t stackUsed();

/// Requests the restart, which must reset the device
void
restartNode()
{
	uavcan_protocol_RestartNodeRequest request{};
	request.magic_number = 0x1234;
	CHECK(call(restart, request) and not restart.response().ok, "restart with wrong magic");
	request.magic_number = UAVCAN_PROTOCOL_RESTARTNODE_REQUEST_MAGIC_NUMBER;
	CHECK(call(restart, request) and restart.response().ok, "restart");
	MODM_LOG_INFO << "Restarting, stack " << stackUsed() << " bytes" << modm::endl;
	waitUntil([] { return false; }, 1s);
	CHECK(false, "no restart");
}

void
waitForNode(uint8_t expected)
{
	CHECK(waitUntil([] { return node.nodeId() != 0; }, 3s), "no node-ID");
	CHECK(node.nodeId() == expected, "node-ID %u", node.nodeId());
	status.received = false;
	CHECK(waitUntil([] { return status.received and status.source == node.nodeId(); }, 1500ms), "no status");
	MODM_LOG_INFO.printf("Node-ID %u, uptime %lus\n", node.nodeId(), status.value.uptime_sec);
}

void
checkDefaults()
{
	CHECK(getSet("uavcan.node_id").value.integer_value == 0, "node_id default");
	CHECK(getSet("test.rate").value.integer_value == 10, "rate default");
	CHECK(getSet("test.gain").value.real_value == 0.5f, "gain default");
	CHECK(getSet("test.enable").value.boolean_value == 1, "enable default");
	CHECK(rate.integer() == 10 and gain.real() == 0.5f and enable.boolean(), "defaults in the node");
}

// ----------------------------------------------------------------------------
void
firstBoot()
{
	waitForNode(125);
	CHECK(status.value.health == UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK and
		  status.value.mode == UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL, "status");

	CHECK(call(info, uavcan_protocol_GetNodeInfoRequest{}), "GetNodeInfo");
	const auto& i = info.response();
	CHECK(i.name.len == sizeof(Name) - 1 and std::memcmp(i.name.data, Name, sizeof(Name) - 1) == 0, "name");
	CHECK(std::ranges::equal(i.hardware_version.unique_id, UniqueId), "unique ID");
	CHECK(i.software_version.major == 2 and i.software_version.minor == 3, "version");

	// list by index
	for (uint16_t index = 0; index < std::size(parameters); index++)
	{
		const auto& response = getSet({}, {}, index);
		CHECK(nameOf(response) == parameters[index]->name(), "name of index %u", index);
	}
	const auto& end = getSet({}, {}, std::size(parameters));
	CHECK(end.name.len == 0 and end.value.union_tag == UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY, "index past the end");
	const auto& unknown = getSet("test.unknown", integer(1));
	CHECK(unknown.name.len == 0 and unknown.value.union_tag == UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY, "unknown name");

	// ranges and defaults
	const auto& r = getSet("test.rate");
	CHECK(r.value.union_tag == UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE and r.value.integer_value == 10 and
		  r.default_value.integer_value == 10 and r.min_value.integer_value == 1 and
		  r.max_value.integer_value == 100, "rate");
	const auto& g = getSet("test.gain");
	CHECK(g.value.union_tag == UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE and g.min_value.real_value == -10.f and
		  g.max_value.real_value == 10.f, "gain");
	const auto& e = getSet("test.enable");
	CHECK(e.value.union_tag == UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE and
		  e.min_value.union_tag == UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_EMPTY, "enable");

	// sets, also refused ones, which answer with the current value
	CHECK(getSet("test.rate", integer(50)).value.integer_value == 50, "set rate");
	CHECK(getSet("test.rate", integer(101)).value.integer_value == 50, "rate above range");
	CHECK(getSet("test.rate", real(20.f)).value.integer_value == 50, "rate with real");
	CHECK(getSet("test.gain", real(-2.5f)).value.real_value == -2.5f, "set gain");
	CHECK(getSet("test.gain", integer(3)).value.real_value == 3.f, "set gain with integer");
	CHECK(getSet("test.gain", real(10.5f)).value.real_value == 3.f, "gain above range");
	CHECK(getSet("test.enable", boolean(false)).value.boolean_value == 0, "set enable");
	CHECK(getSet("test.enable", integer(2)).value.boolean_value == 0, "enable with 2");
	CHECK(getSet("uavcan.node_id", integer(126)).value.integer_value == 126, "set node_id");
	CHECK(rate.integer() == 50 and gain.real() == 3.f and not enable.boolean(), "values in the node");
	// the node-ID only changes after the restart
	CHECK(node.nodeId() == 125, "node-ID changed");

	CHECK(executeOpcode(UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_REQUEST_OPCODE_SAVE), "save");
	CHECK(not executeOpcode(7), "unknown opcode");
	restartNode();
}

void
secondBoot()
{
	waitForNode(126);
	CHECK(status.value.uptime_sec <= 2, "uptime %lu after restart", status.value.uptime_sec);
	CHECK(getSet("test.rate").value.integer_value == 50, "stored rate");
	CHECK(getSet("test.gain").value.real_value == 3.f, "stored gain");
	CHECK(getSet("test.enable").value.boolean_value == 0, "stored enable");
	CHECK(getSet("uavcan.node_id").value.integer_value == 126, "stored node_id");

	// another node with node-ID 126 sends a NodeStatus
	uint8_t buffer[UAVCAN_PROTOCOL_NODESTATUS_MAX_SIZE];
	const uint32_t size = uavcan_protocol_NodeStatus_encode(&status.value, buffer);
	modm::can::Message message((uint32_t(canard_prio_low) << 26) | (uint32_t(UAVCAN_PROTOCOL_NODESTATUS_ID) << 8) | 126,
							   size + 1);
	message.setExtended(true);
	std::memcpy(message.data, buffer, size);
	message.data[size] = 0xC0;
	CHECK(Can::sendMessage(message), "send collision");
	CHECK(waitUntil([] { return node.collisions() == 1; }, 100ms), "collision %u", node.collisions());
	waitUntil([] { return false; }, 100ms);
	CHECK(node.nodeId() == 126 and node.canard.node_id == 126, "node-ID changed by collision");

	CHECK(executeOpcode(UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_REQUEST_OPCODE_ERASE), "erase");
	checkDefaults();
	restartNode();
}

void
thirdBoot()
{
	waitForNode(125);
	checkDefaults();
	uint64_t data;
	modm::canard::ZmsStorage storage{fs};
	for (const auto* parameter : parameters)
		CHECK(not storage.read(parameter->key(), data), "%.*s still stored",
			  int(parameter->name().size()), parameter->name().data());
}

/// Fills the unused main stack, so that stackUsed() finds the peak usage
void
paintStack()
{
	uint32_t* sp;
	asm volatile ("mov %0, sp" : "=r" (sp));
	for (uint32_t* word = __main_stack_bottom; word < sp - 16; word++) *word = 0xa5a5'a5a5;
}

size_t
stackUsed()
{
	uint32_t* word = __main_stack_bottom;
	while (word < __main_stack_top and *word == 0xa5a5'a5a5) word++;
	return (__main_stack_top - word) * 4;
}

}	// namespace

int
main()
{
	paintStack();
	Board::initialize();
	if (state.magic != Magic) state = {Magic, 0, false};
	passed = not state.failed;
	MODM_LOG_INFO << "\n\nDroneCAN node test, boot " << (state.boot + 1) << modm::endl;

	if (not modm::zms::setup(fs, Flash::Size - 4 * Flash::MaxPageSize, 4, Flash::MaxPageSize) or
		zms_mount(&fs) != 0)
	{
		MODM_LOG_ERROR << "ZMS setup failed" << modm::endl;
		return finishTest(false);
	}
	// start with an empty storage
	if (state.boot == 0) CHECK(zms_clear(&fs) == 0 and zms_mount(&fs) == 0, "clear");
	modm::canard::ZmsStorage storage{fs};
	modm::canard::loadParameters(parameters, storage);

#ifdef FDCAN1
	Can::initialize<Board::SystemClock, 1_Mbps>(9, Can::Mode::TestInternalLoopback);
#else
	// bxCAN needs a recessive CANRX pin to leave the initialization mode
	Can::connect<GpioA11::Rx, GpioA12::Tx>(Gpio::InputType::PullUp);
	CHECK((Can::initialize<Board::SystemClock, 1_Mbps>(9, Can::Mode::ListenOnlyLoopBack)), "CAN init");
#endif
	Can::setExtendedFilter(0, Can::FilterConfig::Fifo0,
			modm::can::ExtendedIdentifier(0), modm::can::ExtendedMask(0));

	// Both nodes share one controller, so libcanard must not configure the
	// filters, and each node must ignore the frames it sent itself.
	constexpr modm::canard::NodeConfig config{.filters = false, .loopback = true, .keep_node_id = true};
	CHECK(tool.initialize(ToolId, 0x7001, config), "tool");
	CHECK(status.subscribe() and allocation.subscribe(), "tool subscriptions");
	CHECK(info.initialize(&tool.canard) and getset.initialize(&tool.canard) and
		  opcode.initialize(&tool.canard) and restart.initialize(&tool.canard), "tool clients");
	CHECK(not getset.initialize(&tool.canard), "client initialized twice");
	dronecan::Client<uavcan_protocol_param_GetSetRequest> second;
	CHECK(not second.initialize(&tool.canard), "second client of a service");

	CHECK(node.initialize({.name = Name, .unique_id = UniqueId, .software_major = 2, .software_minor = 3},
						  {.node_id = uint8_t(node_id.integer()), .parameters = parameters, .storage = &storage,
						   .restart = [] { state.boot++; state.failed = not passed; NVIC_SystemReset(); }, .node = config}), "node");

	switch (state.boot)
	{
		case 0: firstBoot(); break;
		case 1: secondBoot(); break;
		case 2: thirdBoot(); break;
		default: CHECK(false, "boot %lu", state.boot);
	}
	state.magic = 0;
	MODM_LOG_INFO.printf("collisions %u, tx expired %lu, stack %u bytes\n", node.collisions(),
						 uint32_t(node.canard.err.tx_expiration + tool.canard.err.tx_expiration), stackUsed());
	return finishTest(passed);
}
