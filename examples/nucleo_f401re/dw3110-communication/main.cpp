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
#include <modm/processing/protothread.hpp>
#include <modm/processing/timer.hpp>

using namespace Board;
using namespace std::chrono_literals;

using MySpiMaster = modm::platform::SpiMaster1;
using MyDw3110_a = modm::Dw3110Phy<MySpiMaster, GpioB6>;
using MyDw3110_b = modm::Dw3110Phy<MySpiMaster, GpioA10>;

constexpr static size_t PacketLength = 1021;

class TXThread : public modm::pt::Protothread
{
public:
	bool
	init()
	{
		auto ret = RF_CALL_BLOCKING(radio.initialize(
			modm::Dw3110::Channel::Channel9, modm::Dw3110::PreambleCode::Code_64Mhz_9,
			modm::Dw3110::PreambleLength::Preamble_128,
			modm::Dw3110::StartFrameDelimiter::Decawave_8));
		RF_CALL_BLOCKING(radio.setEnableLongFrames(true));
		return ret;
	}

	bool
	run()
	{
		PT_BEGIN();
		while (true)
		{

			txdata[txdata.size() - 1]++;
			timeout.restart(Button::read() ? 500ms : 10ms);
			PT_WAIT_UNTIL(timeout.execute());
			if (PT_CALL(radio.transmit(txdata, true)) == MyDw3110_b::Error::None)
			{
				sentCount++;
			} else
			{
				MODM_LOG_DEBUG << "[TX] Failed to trasmit!" << modm::endl;
			}
		}
		PT_END();
	}

	size_t
	getCount()
	{
		return sentCount;
	}

private:
	MyDw3110_b radio{};
	std::array<uint8_t, PacketLength> txdata = {0xBA, 0xDE, 0xAF, 0xFE, 0x00};
	modm::Timeout timeout{10ms};
	size_t sentCount{0};
};

class RXThread : public modm::pt::Protothread
{
public:
	bool
	init()
	{
		auto ret = RF_CALL_BLOCKING(radio.initialize(
			modm::Dw3110::Channel::Channel9, modm::Dw3110::PreambleCode::Code_64Mhz_9,
			modm::Dw3110::PreambleLength::Preamble_128,
			modm::Dw3110::StartFrameDelimiter::Decawave_8));
		RF_CALL_BLOCKING(radio.setEnableLongFrames(true));
		return ret;
	}

	bool
	run()
	{
		PT_BEGIN();
		while (true)
		{
			while (!PT_CALL(radio.packetReady()))
			{
				if (!PT_CALL(radio.isReceiving()))
				{
					// KEEP ON SEPERATE LINE
					PT_CALL(radio.startReceive());
				}
				PT_YIELD();
			}

			if (PT_CALL(radio.fetchPacket(rxdata, rxlen))) { recvCount++; }
		}
		PT_END();
	}

	size_t
	getCount()
	{
		return recvCount;
	}

private:
	MyDw3110_a radio{};
	size_t rxlen{0}, recvCount{0};
	std::array<uint8_t, PacketLength> rxdata = {};
};

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	MySpiMaster::initialize<Board::SystemClock, 21_MHz>();
	MySpiMaster::connect<GpioA6::Miso, GpioA7::Mosi, GpioA5::Sck>();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG << "debug" << modm::endl;
	MODM_LOG_INFO << "info" << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR << "error" << modm::endl;

	MODM_LOG_INFO << "Initializing Devices..." << modm::endl;
	bool success = true;
	TXThread tx;
	if (!tx.init())
	{
		MODM_LOG_ERROR << "Failed to initialize TX Device!" << modm::endl;
		success = false;
	}

	RXThread rx;
	if (!rx.init())
	{
		MODM_LOG_ERROR << "Failed to initialize TR Device!" << modm::endl;
		success = false;
	}
	if (!success)
		while (true) {}

	modm::PeriodicTimer timer{1000ms};
	MODM_LOG_INFO << "Starting ping pong..." << modm::endl;
	while (true)
	{
		rx.run();
		tx.run();
		if (timer.execute())
		{
			MODM_LOG_DEBUG << "Sent " << tx.getCount() << ", received " << rx.getCount()
						   << ". Diff:" << tx.getCount() - rx.getCount() << modm::endl;
		}
	}

	return 0;
}
