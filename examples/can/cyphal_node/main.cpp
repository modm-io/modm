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

// A Cyphal node with persistent registers, e.g. for Yakut. Connect a CAN
// transceiver to PB8 (RX) and PB9 (TX) and a bus with 1 Mbit/s.
//
// The registers are stored with ZMS in the last 16 kB of the flash, which are
// in the second bank of the STM32G474RE, so the CPU keeps running while the
// flash is written. Write registers with `y r <node> <name> <value>`, store
// them with `y cmd <node> store` and restart the node with `y cmd <node> restart`
// to see them persist:
//
// - uavcan.node.id: 65535 to let the node pick a free node-ID, 0-127 for a fixed one.
// - uavcan.pub.gain.id: subject-ID of the published gain, 65535 to not publish it.
// - example.blink_ms: the period of the LED.
// - example.led: enables the LED.
// - example.gain: published as uavcan.primitive.scalar.Real32 once per second.

#include <modm/board.hpp>
#include <modm/communication/cyphal.hpp>
#include <modm/ext/canard/parameter_zms.hpp>
#include <modm/platform/id/id.hpp>
#include <modm/processing/timer.hpp>
#include <uavcan/primitive/scalar/Real32_1_0.h>

using namespace modm::literals;
using namespace std::chrono_literals;

modm::canard::Parameter node_id{"uavcan.node.id", 65535, 0, 65535};
modm::canard::Parameter gain_id{"uavcan.pub.gain.id", 65535, 0, 65535};
modm::canard::Parameter blink_ms{"example.blink_ms", 500, 50, 5000};
modm::canard::Parameter led{"example.led", true};
modm::canard::Parameter gain{"example.gain", 1.f, -100.f, 100.f};
modm::canard::Parameter* const parameters[]{&node_id, &gain_id, &blink_ms, &led, &gain};

modm::cyphal::Node<Fdcan1> node;
zms_fs fs;

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "Cyphal node" << modm::endl;

	// Four sectors of the largest flash page, which is 2 kB in dual-bank mode
	modm::canard::ZmsStorage* storage{};
	static modm::canard::ZmsStorage zms_storage{fs};
	if (modm::zms::setup(fs, Flash::Size - 4 * Flash::MaxPageSize, 4, Flash::MaxPageSize) and
		zms_mount(&fs) == 0)
	{
		storage = &zms_storage;
		modm::canard::loadParameters(parameters, *storage);
	}
	else MODM_LOG_ERROR << "No register storage!" << modm::endl;

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
					{.node_id = node_id.integer() <= CANARD_NODE_ID_MAX ?
								uint8_t(node_id.integer()) : uint8_t(CANARD_NODE_ID_ANONYMOUS),
					 .parameters = parameters, .storage = storage,
					 .restart = [] { NVIC_SystemReset(); }});

	modm::PeriodicTimer report{1s};
	modm::Timeout blink{std::chrono::milliseconds(blink_ms.integer())};
	uint8_t transfer_id{};
	uint8_t last_node_id{0xff};
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

		if (report.execute() and gain_id.integer() <= CANARD_SUBJECT_ID_MAX_13b)
		{
			const uavcan_primitive_scalar_Real32_1_0 value{.value = gain.real()};
			uint8_t buffer[uavcan_primitive_scalar_Real32_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
			size_t size = sizeof(buffer);
			uavcan_primitive_scalar_Real32_1_0_serialize_(&value, buffer, &size);
			canard_publish_13b(&node.canard, modm::canard::now() + 1'000'000, CANARD_IFACE_BITMAP_ALL,
							   canard_prio_nominal, gain_id.integer(), transfer_id,
							   {{size, buffer}, nullptr}, nullptr);
			transfer_id = (transfer_id + 1) % CANARD_TRANSFER_ID_MODULO;
		}
	}
}
