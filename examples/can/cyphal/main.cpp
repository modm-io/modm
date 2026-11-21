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

// Two Cyphal nodes on one CAN controller in loopback mode, no transceiver
// needed. The publisher (node-ID 42) sends the heartbeat and a diagnostic
// record, which is a multi-frame transfer. The subscriber allocates its node-ID
// automatically and receives both from the loopback. The publisher must not
// receive its own frames, libcanard would see a node-ID collision.

#include <modm/board.hpp>
#include <modm/ext/canard/canard.hpp>
#include <modm/processing/timer.hpp>
#include <uavcan/node/Heartbeat_1_0.h>
#include <uavcan/diagnostic/Record_1_1.h>
#include <cstdio>

using namespace modm::literals;
using namespace std::chrono_literals;

#if defined(FDCAN1)
using Can = Fdcan1;
constexpr bool CanFd = true;
#elif __has_include(<modm/platform/can/can_1.hpp>)
using Can = Can1;
constexpr bool CanFd = false;
#else
using Can = modm::platform::Can;
constexpr bool CanFd = false;
#endif

modm::canard::Node<Can> publisher;
modm::canard::Node<Can> subscriber;
uint32_t heartbeats{}, records{}, errors{};

static void
onHeartbeat(canard_subscription_t* sub, canard_us_t, canard_prio_t, uint_least8_t source,
			uint_least8_t, canard_payload_t payload)
{
	uavcan_node_Heartbeat_1_0 heartbeat{};
	size_t size = payload.view.size;
	if (uavcan_node_Heartbeat_1_0_deserialize_(&heartbeat, (const uint8_t*)payload.view.data, &size) < 0)
		errors++;
	else heartbeats++;
	MODM_LOG_INFO.printf("Heartbeat from %u: uptime %lu s\n", source, heartbeat.uptime);
	modm::canard::release(sub, payload);
}

static void
onRecord(canard_subscription_t* sub, canard_us_t, canard_prio_t, uint_least8_t source,
		 uint_least8_t, canard_payload_t payload)
{
	uavcan_diagnostic_Record_1_1 record{};
	size_t size = payload.view.size;
	if (uavcan_diagnostic_Record_1_1_deserialize_(&record, (const uint8_t*)payload.view.data, &size) < 0)
		errors++;
	else records++;
	MODM_LOG_INFO.printf("Record from %u: %.*s\n", source,
						 int(record.text.count), (const char*)record.text.elements);
	modm::canard::release(sub, payload);
}

constexpr canard_subscription_vtable_t heartbeatVtable{onHeartbeat};
constexpr canard_subscription_vtable_t recordVtable{onRecord};
canard_subscription_t heartbeatSubscription, recordSubscription;

static void
publish(uint32_t uptime, uint8_t transfer_id)
{
	const canard_us_t deadline = modm::canard::now() + 1'000'000;
	uint8_t buffer[uavcan_diagnostic_Record_1_1_SERIALIZATION_BUFFER_SIZE_BYTES_];

	uavcan_node_Heartbeat_1_0 heartbeat{};
	heartbeat.uptime = uptime;
	heartbeat.health.value = uavcan_node_Health_1_0_NOMINAL;
	heartbeat.mode.value = uavcan_node_Mode_1_0_OPERATIONAL;
	size_t size = sizeof(buffer);
	uavcan_node_Heartbeat_1_0_serialize_(&heartbeat, buffer, &size);
	if (not canard_publish_13b(&publisher.canard, deadline, CANARD_IFACE_BITMAP_ALL,
			canard_prio_nominal, uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_, transfer_id,
			{{size, buffer}, nullptr}, nullptr)) errors++;

	uavcan_diagnostic_Record_1_1 record{};
	record.severity.value = uavcan_diagnostic_Severity_1_0_INFO;
	record.text.count = snprintf((char*)record.text.elements, sizeof(record.text.elements),
			"Hello from modm after %lu s, in several frames", uptime);
	size = sizeof(buffer);
	uavcan_diagnostic_Record_1_1_serialize_(&record, buffer, &size);
	if (not canard_publish_13b(&publisher.canard, deadline, CANARD_IFACE_BITMAP_ALL,
			canard_prio_low, uavcan_diagnostic_Record_1_1_FIXED_PORT_ID_, transfer_id,
			{{size, buffer}, nullptr}, nullptr)) errors++;
}

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "Cyphal loopback example" << modm::endl;

#ifdef FDCAN1
	Can::initialize<Board::SystemClock, 500_kbps, 1_pct, 2_Mbps>(9, Can::Mode::TestInternalLoopback);
#else
	// bxCAN needs a recessive CANRX pin to leave the initialization mode
	Can::connect<GpioA11::Rx, GpioA12::Tx>(Gpio::InputType::PullUp);
	if (not Can::initialize<Board::SystemClock, 500_kbps>(9, Can::Mode::ListenOnlyLoopBack))
		errors++;
#endif

	// Both nodes share one controller, but only the subscriber receives, so it
	// configures the acceptance filters for its subscriptions.
	if (not publisher.initialize(42, 0x4242'4242'4242'4242, {.fd = CanFd, .filters = false})) errors++;
	if (not subscriber.initialize(CANARD_NODE_ID_ANONYMOUS, 0x1234'5678'9abc'def0, {.fd = CanFd})) errors++;

	canard_subscribe_13b(&subscriber.canard, &heartbeatSubscription,
			uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_, uavcan_node_Heartbeat_1_0_EXTENT_BYTES_,
			CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_us, &heartbeatVtable);
	canard_subscribe_13b(&subscriber.canard, &recordSubscription,
			uavcan_diagnostic_Record_1_1_FIXED_PORT_ID_, uavcan_diagnostic_Record_1_1_EXTENT_BYTES_,
			CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_us, &recordVtable);

	modm::PeriodicTimer timer{1s};
	uint32_t uptime{};
	uint8_t transfer_id{};
	while (true)
	{
		publisher.poll();		// only transmits
		subscriber.update();	// receives everything from the loopback

		if (timer.execute())
		{
			MODM_LOG_INFO.printf("node-ID %u, heartbeats %lu records %lu errors %lu\n",
								 subscriber.canard.node_id, heartbeats, records,
								 errors + uint32_t(publisher.canard.err.tx_expiration +
								 subscriber.canard.err.rx_transfer));
			publish(uptime++, transfer_id);
			transfer_id = (transfer_id + 1) % CANARD_TRANSFER_ID_MODULO;
		}
	}
}
