/*
 * Copyright (c) 2024, Elias H.
 * Copyright (c) 2024, Raphael Lehmann
 * Copyright (c) 2024, Michael Jossen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/driver/radio/dw3110/dw3110_phy.hpp>
#include <modm/processing/timer.hpp>

using namespace Board;
using namespace std::chrono_literals;

using MySpiMaster = modm::platform::SpiMaster1;
using MyDw3110_a = modm::Dw3110Phy<MySpiMaster, GpioB6>;
using MyDw3110_b = modm::Dw3110Phy<MySpiMaster, GpioA10>;

MyDw3110_a myDw3110_a{};
MyDw3110_b myDw3110_b{};

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	MySpiMaster::initialize<Board::SystemClock, 21_MHz>();
	MySpiMaster::connect<GpioA6::Miso, GpioA7::Mosi, GpioA5::Sck>();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	auto ret = RF_CALL_BLOCKING(myDw3110_a.initialize(
		modm::Dw3110::Channel::Channel5, modm::Dw3110::PreambleCode::Code_64Mhz_9,
		modm::Dw3110::PreambleLength::Preamble_4096, modm::Dw3110::StartFrameDelimiter::Decawave_8));
	if (!ret) { MODM_LOG_ERROR << "Failed to initialize Dw3110 Number 1" << modm::endl; }
	auto ret2 = RF_CALL_BLOCKING(myDw3110_b.initialize(
		modm::Dw3110::Channel::Channel5, modm::Dw3110::PreambleCode::Code_64Mhz_9,
		modm::Dw3110::PreambleLength::Preamble_4096, modm::Dw3110::StartFrameDelimiter::Decawave_8));
	if (!ret2) { MODM_LOG_ERROR << "Failed to initialize Dw3110 Number 2" << modm::endl; }

	if (!ret || !ret2)
	{
		while (true) {}
	}

	std::array<uint8_t, 5> txdata = {0xDE, 0xAD, 0xBE, 0xEF, 0x00};
	std::array<uint8_t, 32> rxdata = {};
	std::span<const uint8_t, 5> view{txdata};
	std::span<uint8_t, 32> recv{rxdata};
	MODM_LOG_INFO << "Starting ping pong..." << modm::endl;
	while (true)
	{
		LedD13::toggle();
		txdata[4]++;
		modm::delay(Button::read() ? 100ms : 500ms);
		RF_CALL_BLOCKING(myDw3110_a.startReceive());
		if (!RF_CALL_BLOCKING(myDw3110_b.transmit(view, view.size())))
		{
			MODM_LOG_DEBUG << "Failed to trasmit!" << modm::endl;
		} else
		{
			MODM_LOG_DEBUG << "Transmitted 0x";
			for (size_t i = 0; i < txdata.size(); i++) { MODM_LOG_DEBUG << modm::hex << txdata[i]; }
			MODM_LOG_DEBUG << modm::endl;
		}
		if (RF_CALL_BLOCKING(myDw3110_a.packetReady()))
		{
			size_t len = 0;
			if (RF_CALL_BLOCKING(myDw3110_a.fetchPacket(recv, len)))
			{
				MODM_LOG_DEBUG << modm::ascii << "Got packet of length " << len << modm::endl;
				MODM_LOG_DEBUG << "Got 0x";
				for (size_t i = 0; i < len; i++) { MODM_LOG_DEBUG << modm::hex << recv[i]; }
				MODM_LOG_DEBUG << modm::endl;
			} else
			{
				MODM_LOG_DEBUG << "Failed to fetch packet!" << modm::endl;
			}
		}
	}

	return 0;
}
