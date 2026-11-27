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

// A DroneCAN node with persistent parameters, e.g. for the DroneCAN GUI tool
// or ArduPilot. Connect a CAN transceiver to PB8 (RX) and PB9 (TX) and a bus
// with 1 Mbit/s and a node-ID allocation server, e.g. ArduPilot or the GUI
// tool with "Dynamic node ID allocation server".
//
// The parameters are stored with ZMS in the last 16 kB of the flash, which are
// in the second bank of the STM32G474RE, so the CPU keeps running while the
// flash is written. Set parameters with GetSet, save them with ExecuteOpcode
// SAVE and restart the node with RestartNode to see them persist:
//
// - uavcan.node_id: 0 for dynamic node-ID allocation, 1-125 for a fixed one.
// - example.blink_ms: the period of the LED.
// - example.led: enables the LED.
// - example.gain: published in a uavcan.protocol.debug.KeyValue message.

#include <modm/board.hpp>
#include <modm/communication/dronecan.hpp>
#include <modm/ext/canard/parameter_zms.hpp>
#include <modm/platform/id/id.hpp>
#include <modm/processing/timer.hpp>
#include <uavcan.protocol.debug.KeyValue.h>

using namespace modm::literals;
using namespace std::chrono_literals;
namespace dronecan = modm::dronecan;

modm::canard::Parameter node_id{"uavcan.node_id", 0, 0, 125};
modm::canard::Parameter blink_ms{"example.blink_ms", 500, 50, 5000};
modm::canard::Parameter led{"example.led", true};
modm::canard::Parameter gain{"example.gain", 1.f, -100.f, 100.f};
modm::canard::Parameter* const parameters[]{&node_id, &blink_ms, &led, &gain};

dronecan::Node<Fdcan1> node;
zms_fs fs;

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "DroneCAN node" << modm::endl;

	// Four sectors of the largest flash page, which is 2 kB in dual-bank mode
	modm::canard::ZmsStorage* storage{};
	static modm::canard::ZmsStorage zms_storage{fs};
	if (modm::zms::setup(fs, Flash::Size - 4 * Flash::MaxPageSize, 4, Flash::MaxPageSize) and
		zms_mount(&fs) == 0)
	{
		storage = &zms_storage;
		modm::canard::loadParameters(parameters, *storage);
	}
	else MODM_LOG_ERROR << "No parameter storage!" << modm::endl;

	std::array<uint8_t, 16> unique_id{};
	for (uint8_t ii = 0; ii < 3; ii++)
	{
		const uint32_t uid = modm::platform::getUniqueId(ii);
		std::memcpy(unique_id.data() + 4 * ii, &uid, 4);
	}

	Fdcan1::connect<GpioB8::Rx, GpioB9::Tx>(Gpio::InputType::PullUp);
	Fdcan1::initialize<Board::SystemClock, 1_Mbps>(9);

	node.initialize({.name = "io.modm.example.node", .unique_id = unique_id,
					 .software_major = 1, .software_minor = 0},
					{.node_id = uint8_t(node_id.integer()), .parameters = parameters,
					 .storage = storage, .restart = [] { NVIC_SystemReset(); }});

	modm::PeriodicTimer report{1s};
	modm::Timeout blink{std::chrono::milliseconds(blink_ms.integer())};
	uint8_t transfer_id{};
	uint8_t last_node_id{};
	while (true)
	{
		node.update();

		if (blink.execute())
		{
			blink.restart(std::chrono::milliseconds(blink_ms.integer()));
			if (led.boolean()) Board::Leds::toggle();
			else Board::Leds::reset();
		}

		if (node.nodeId() != last_node_id)
		{
			last_node_id = node.nodeId();
			MODM_LOG_INFO << "Node-ID " << last_node_id << modm::endl;
		}

		if (node.nodeId() and report.execute())
		{
			uavcan_protocol_debug_KeyValue value{};
			value.value = gain.real() * node.status().uptime_sec;
			std::memcpy(value.key.data, "gain", 4);
			value.key.len = 4;
			dronecan::publish(&node.canard, value, transfer_id, canard_prio_low);
		}
	}
}
