/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// Tests modm::cyphal::Node with persistent registers in ZMS over three boots.
// A second node (127) in the same firmware plays Yakut, both on one CAN
// controller in loopback.
//
// 1. The node picks a free node-ID. The tool checks the heartbeat, the node
//    info, lists, reads and writes the registers, also out of range, with
//    other types and with more elements, stores them and restarts the node.
// 2. The node starts with the stored node-ID 42 and registers. The tool
//    resets the node to the factory defaults and restarts it.
// 3. The node picks a free node-ID again and has the defaults.

#include <modm/board.hpp>
#include <modm/communication/cyphal.hpp>
#include <modm/ext/canard/parameter_zms.hpp>
#include <modm/processing/timer.hpp>
#include <algorithm>
#include <cstring>

#include "../integration_test.hpp"

using namespace modm::literals;
using namespace std::chrono_literals;
using Value = uavcan_register_Value_1_0;

#if defined(FDCAN1)
using Can = Fdcan1;
#elif __has_include(<modm/platform/can/can_1.hpp>)
using Can = Can1;
#else
using Can = modm::platform::Can;
#endif

extern "C" uint32_t __main_stack_bottom[], __main_stack_top[];

namespace
{

constexpr uint8_t ToolId = 127;
constexpr std::array<uint8_t, 16> UniqueId{0x6d, 0x6f, 0x64, 0x6d, 0x2e, 0x69, 0x6f, 0x2e,
										   0x63, 0x79, 0x70, 0x68, 0x61, 0x6c, 0x00, 0x01};
constexpr char Name[] = "io.modm.test.cyphal";

// survives the restarts of the node
struct State
{
	uint32_t magic;
	uint32_t boot;
	bool failed;
};
modm_section(".noinit") State state;
constexpr uint32_t Magic = 0xc1fa'1001;

modm::canard::Parameter node_id{"uavcan.node.id", 65535, 0, 65535};
modm::canard::Parameter rate{"test.rate", 10, 1, 100};
modm::canard::Parameter gain{"test.gain", 0.5f, -10.f, 10.f};
modm::canard::Parameter enable{"test.enable", true};
modm::canard::Parameter* const parameters[]{&node_id, &rate, &gain, &enable};

zms_fs fs;
modm::cyphal::Node<Can> node;
modm::canard::Node<Can> tool;
bool passed{true};

#define CHECK(cond, ...) \
	do { if (not (cond)) { passed = false; \
		MODM_LOG_ERROR.printf("ERROR %s:%d: " #cond " ", __FILE__, __LINE__); \
		MODM_LOG_ERROR.printf(__VA_ARGS__); MODM_LOG_ERROR << modm::endl; } } while(0)

// ----------------------------------------------------------------------------
// Replies to the tool, stored by the subscription callbacks
template< class T >
struct Reply
{
	bool received;
	uint8_t source;
	T value;
};
Reply<uavcan_node_Heartbeat_1_0> heartbeat;
Reply<uavcan_node_GetInfo_Response_1_0> info;
Reply<uavcan_node_ExecuteCommand_Response_1_3> command;
Reply<uavcan_register_Access_Response_1_0> access_reply;
Reply<uavcan_register_List_Response_1_0> list;

template< auto& reply, auto deserialize >
void
onReply(canard_subscription_t* sub, canard_us_t, canard_prio_t, uint_least8_t source,
		uint_least8_t, canard_payload_t payload)
{
	size_t size = payload.view.size;
	reply.received = deserialize(&reply.value, static_cast<const uint8_t*>(payload.view.data), &size) >= 0;
	reply.source = source;
	modm::canard::release(sub, payload);
}

template< auto& reply, auto deserialize >
void
subscribeResponse(uint16_t service_id, size_t extent)
{
	static canard_subscription_t subscription;
	static constexpr canard_subscription_vtable_t vtable{onReply<reply, deserialize>};
	CHECK(canard_subscribe_response(&tool.canard, &subscription, service_id, extent, &vtable) == &subscription,
		  "subscribe %u", service_id);
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

/// Sends a request to the node and waits for the response
template< auto& reply, size_t Size, class Request >
bool
call(uint16_t service_id, const Request& request, int8_t (*serialize)(const Request*, uint8_t*, size_t*))
{
	static uint8_t transfer_id{};
	// static, since the register requests have more than 500 bytes
	static uint8_t buffer[Size ? Size : 1];
	size_t size = Size;
	if (serialize(&request, buffer, &size) < 0) return false;
	reply.received = false;
	if (not canard_request(&tool.canard, modm::canard::now() + 1'000'000, canard_prio_nominal, service_id,
						   node.nodeId(), transfer_id, {{size, buffer}, nullptr}, nullptr))
		return false;
	transfer_id = (transfer_id + 1) % CANARD_TRANSFER_ID_MODULO;
	return waitUntil([] { return reply.received and reply.source == node.nodeId(); }, 500ms);
}

// A small value for access(), since a register value has about 270 bytes, and
// every temporary one would take its own space on the stack of the caller.
enum class Kind : uint8_t { Empty, Integer64, Natural8, Real32, Real64, Bit, Integer64x2 };
struct Write
{
	Kind kind{Kind::Empty};
	double value{};
};

/// Reads or writes a register
[[gnu::noinline]] const uavcan_register_Access_Response_1_0&
access(std::string_view name, Write write = {})
{
	uavcan_register_Access_Request_1_0 request{};
	request.name.name.count = name.size();
	std::memcpy(request.name.name.elements, name.data(), name.size());
	Value& v = request.value;
	switch (write.kind)
	{
		case Kind::Empty: uavcan_register_Value_1_0_select_empty_(&v); break;
		case Kind::Integer64:
			uavcan_register_Value_1_0_select_integer64_(&v);
			v.integer64.value.elements[0] = int64_t(write.value);
			v.integer64.value.count = 1;
			break;
		case Kind::Integer64x2:
			uavcan_register_Value_1_0_select_integer64_(&v);
			v.integer64.value.elements[0] = v.integer64.value.elements[1] = int64_t(write.value);
			v.integer64.value.count = 2;
			break;
		case Kind::Natural8:
			uavcan_register_Value_1_0_select_natural8_(&v);
			v.natural8.value.elements[0] = uint8_t(write.value);
			v.natural8.value.count = 1;
			break;
		case Kind::Real32:
			uavcan_register_Value_1_0_select_real32_(&v);
			v.real32.value.elements[0] = float(write.value);
			v.real32.value.count = 1;
			break;
		case Kind::Real64:
			uavcan_register_Value_1_0_select_real64_(&v);
			v.real64.value.elements[0] = write.value;
			v.real64.value.count = 1;
			break;
		case Kind::Bit:
			uavcan_register_Value_1_0_select_bit_(&v);
			v.bit.value.bitpacked[0] = write.value != 0;
			v.bit.value.count = 1;
			break;
	}
	access_reply.value = {};
	CHECK((call<access_reply, uavcan_register_Access_Request_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_>(
			uavcan_register_Access_1_0_FIXED_PORT_ID_, request, uavcan_register_Access_Request_1_0_serialize_)),
		  "Access %.*s", int(name.size()), name.data());
	return access_reply.value;
}

int64_t integerOf(const Value& v) { return uavcan_register_Value_1_0_is_integer64_(&v) and v.integer64.value.count == 1 ? v.integer64.value.elements[0] : -12345; }
float realOf(const Value& v) { return uavcan_register_Value_1_0_is_real32_(&v) and v.real32.value.count == 1 ? v.real32.value.elements[0] : -12345.f; }
int bitOf(const Value& v) { return uavcan_register_Value_1_0_is_bit_(&v) and v.bit.value.count == 1 ? (v.bit.value.bitpacked[0] & 1) : -1; }

std::string_view
listName(uint16_t index)
{
	const uavcan_register_List_Request_1_0 request{index};
	list.value = {};
	CHECK((call<list, uavcan_register_List_Request_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_>(
			uavcan_register_List_1_0_FIXED_PORT_ID_, request, uavcan_register_List_Request_1_0_serialize_)),
		  "List %u", index);
	return {reinterpret_cast<const char*>(list.value.name.name.elements), list.value.name.name.count};
}

uint8_t
executeCommand(uint16_t code)
{
	uavcan_node_ExecuteCommand_Request_1_3 request{};
	request.command = code;
	command.value.status = 0xff;
	CHECK((call<command, uavcan_node_ExecuteCommand_Request_1_3_SERIALIZATION_BUFFER_SIZE_BYTES_>(
			uavcan_node_ExecuteCommand_1_3_FIXED_PORT_ID_, request, uavcan_node_ExecuteCommand_Request_1_3_serialize_)),
		  "ExecuteCommand %u", code);
	return command.value.status;
}

size_t stackUsed();

/// Requests the restart, which must reset the device
void
restartNode()
{
	CHECK(executeCommand(uavcan_node_ExecuteCommand_Request_1_3_COMMAND_RESTART) ==
		  uavcan_node_ExecuteCommand_Response_1_3_STATUS_SUCCESS, "restart");
	MODM_LOG_INFO << "Restarting, stack " << stackUsed() << " bytes" << modm::endl;
	waitUntil([] { return false; }, 1s);
	CHECK(false, "no restart");
}

void
waitForNode()
{
	heartbeat.received = false;
	CHECK(waitUntil([] { return heartbeat.received and heartbeat.source == node.nodeId(); }, 1500ms), "no heartbeat");
	CHECK(node.nodeId() <= CANARD_NODE_ID_MAX and node.nodeId() != ToolId, "node-ID %u", node.nodeId());
	MODM_LOG_INFO.printf("Node-ID %u, uptime %lus\n", node.nodeId(), heartbeat.value.uptime);
}

void
checkDefaults()
{
	CHECK(integerOf(access("uavcan.node.id").value) == 65535, "node.id default");
	CHECK(integerOf(access("test.rate").value) == 10, "rate default");
	CHECK(realOf(access("test.gain").value) == 0.5f, "gain default");
	CHECK(bitOf(access("test.enable").value) == 1, "enable default");
	CHECK(rate.integer() == 10 and gain.real() == 0.5f and enable.boolean(), "defaults in the node");
}

// ----------------------------------------------------------------------------
void
firstBoot()
{
	waitForNode();
	CHECK(heartbeat.value.health.value == uavcan_node_Health_1_0_NOMINAL and
		  heartbeat.value.mode.value == uavcan_node_Mode_1_0_OPERATIONAL, "heartbeat");

	const uavcan_node_GetInfo_Request_1_0 info_request{};
	CHECK((call<info, uavcan_node_GetInfo_Request_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_>(
			uavcan_node_GetInfo_1_0_FIXED_PORT_ID_, info_request, uavcan_node_GetInfo_Request_1_0_serialize_)),
		  "GetInfo");
	CHECK(info.value.name.count == sizeof(Name) - 1 and
		  std::memcmp(info.value.name.elements, Name, sizeof(Name) - 1) == 0, "name");
	CHECK(std::ranges::equal(info.value.unique_id, UniqueId), "unique ID");
	CHECK(info.value.protocol_version.major == 1 and info.value.software_version.major == 2 and
		  info.value.software_version.minor == 3 and info.value.software_vcs_revision_id == 0xabcd, "versions");

	// list by index
	for (uint16_t index = 0; index < std::size(parameters); index++)
		CHECK(listName(index) == parameters[index]->name(), "name of index %u", index);
	CHECK(listName(std::size(parameters)).empty(), "index past the end");
	const auto& unknown = access("test.unknown", {Kind::Integer64, 1});
	CHECK(uavcan_register_Value_1_0_is_empty_(&unknown.value) and not unknown._mutable, "unknown name");

	// types and flags
	const auto& r = access("test.rate");
	CHECK(integerOf(r.value) == 10 and r._mutable and r.persistent, "rate");
	CHECK(realOf(access("test.gain").value) == 0.5f, "gain");
	CHECK(bitOf(access("test.enable").value) == 1, "enable");

	// writes, also refused ones, which answer with the current value
	CHECK(integerOf(access("test.rate", {Kind::Integer64, 50}).value) == 50, "write rate");
	CHECK(integerOf(access("test.rate", {Kind::Integer64, 101}).value) == 50, "rate above range");
	CHECK(integerOf(access("test.rate", {Kind::Natural8, 20}).value) == 20, "rate as natural8");
	CHECK(integerOf(access("test.rate", {Kind::Real32, 30}).value) == 20, "rate as real32");
	CHECK(integerOf(access("test.rate", {Kind::Integer64x2, 40}).value) == 20, "rate with two elements");
	CHECK(integerOf(access("test.rate", {Kind::Integer64, 50}).value) == 50, "write rate again");
	CHECK(realOf(access("test.gain", {Kind::Real32, -2.5}).value) == -2.5f, "write gain");
	CHECK(realOf(access("test.gain", {Kind::Integer64, 3}).value) == 3.f, "gain as integer64");
	CHECK(realOf(access("test.gain", {Kind::Real64, 10.5}).value) == 3.f, "gain above range");
	CHECK(bitOf(access("test.enable", {Kind::Bit, 0}).value) == 0, "write enable");
	CHECK(bitOf(access("test.enable", {Kind::Integer64, 2}).value) == 0, "enable with 2");
	CHECK(integerOf(access("uavcan.node.id", {Kind::Integer64, 42}).value) == 42, "write node.id");
	CHECK(rate.integer() == 50 and gain.real() == 3.f and not enable.boolean(), "values in the node");

	CHECK(executeCommand(uavcan_node_ExecuteCommand_Request_1_3_COMMAND_STORE_PERSISTENT_STATES) ==
		  uavcan_node_ExecuteCommand_Response_1_3_STATUS_SUCCESS, "store");
	CHECK(executeCommand(1) == uavcan_node_ExecuteCommand_Response_1_3_STATUS_BAD_COMMAND, "unknown command");
	restartNode();
}

void
secondBoot()
{
	waitForNode();
	CHECK(node.nodeId() == 42, "stored node-ID %u", node.nodeId());
	CHECK(heartbeat.value.uptime <= 2, "uptime %lu after restart", heartbeat.value.uptime);
	CHECK(integerOf(access("test.rate").value) == 50, "stored rate");
	CHECK(realOf(access("test.gain").value) == 3.f, "stored gain");
	CHECK(bitOf(access("test.enable").value) == 0, "stored enable");
	CHECK(integerOf(access("uavcan.node.id").value) == 42, "stored node.id");

	CHECK(executeCommand(uavcan_node_ExecuteCommand_Request_1_3_COMMAND_FACTORY_RESET) ==
		  uavcan_node_ExecuteCommand_Response_1_3_STATUS_SUCCESS, "factory reset");
	checkDefaults();
	restartNode();
}

void
thirdBoot()
{
	waitForNode();
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
	MODM_LOG_INFO << "\n\nCyphal node test, boot " << (state.boot + 1) << modm::endl;

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
	constexpr modm::canard::NodeConfig config{.filters = false, .loopback = true};
	CHECK(tool.initialize(ToolId, 0x7001, config), "tool");
	static canard_subscription_t heartbeat_subscription;
	static constexpr canard_subscription_vtable_t heartbeat_vtable{
			onReply<heartbeat, uavcan_node_Heartbeat_1_0_deserialize_>};
	CHECK(canard_subscribe_13b(&tool.canard, &heartbeat_subscription, uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_,
							   uavcan_node_Heartbeat_1_0_EXTENT_BYTES_, CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_us,
							   &heartbeat_vtable) == &heartbeat_subscription, "heartbeat");
	subscribeResponse<info, uavcan_node_GetInfo_Response_1_0_deserialize_>(
			uavcan_node_GetInfo_1_0_FIXED_PORT_ID_, uavcan_node_GetInfo_Response_1_0_EXTENT_BYTES_);
	subscribeResponse<command, uavcan_node_ExecuteCommand_Response_1_3_deserialize_>(
			uavcan_node_ExecuteCommand_1_3_FIXED_PORT_ID_, uavcan_node_ExecuteCommand_Response_1_3_EXTENT_BYTES_);
	subscribeResponse<access_reply, uavcan_register_Access_Response_1_0_deserialize_>(
			uavcan_register_Access_1_0_FIXED_PORT_ID_, uavcan_register_Access_Response_1_0_EXTENT_BYTES_);
	subscribeResponse<list, uavcan_register_List_Response_1_0_deserialize_>(
			uavcan_register_List_1_0_FIXED_PORT_ID_, uavcan_register_List_Response_1_0_EXTENT_BYTES_);

	const uint8_t id = node_id.integer() <= CANARD_NODE_ID_MAX ?
					   uint8_t(node_id.integer()) : uint8_t(CANARD_NODE_ID_ANONYMOUS);
	CHECK(node.initialize({.name = Name, .unique_id = UniqueId, .software_major = 2, .software_minor = 3,
						   .vcs_revision = 0xabcd},
						  {.node_id = id, .parameters = parameters, .storage = &storage,
						   .restart = [] { state.boot++; state.failed = not passed; NVIC_SystemReset(); },
						   .node = config}), "node");

	switch (state.boot)
	{
		case 0: firstBoot(); break;
		case 1: secondBoot(); break;
		case 2: thirdBoot(); break;
		default: CHECK(false, "boot %lu", state.boot);
	}
	state.magic = 0;
	MODM_LOG_INFO.printf("tx expired %lu, stack %u bytes\n",
						 uint32_t(node.canard.err.tx_expiration + tool.canard.err.tx_expiration), stackUsed());
	return finishTest(passed);
}
