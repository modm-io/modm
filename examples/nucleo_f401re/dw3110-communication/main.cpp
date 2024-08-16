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

class TXThread : public modm::pt::Protothread
{
public:
	bool
	init()
	{
		return RF_CALL_BLOCKING(radio.initialize(modm::Dw3110::Channel::Channel5,
												 modm::Dw3110::PreambleCode::Code_64Mhz_9,
												 modm::Dw3110::PreambleLength::Preamble_4096,
												 modm::Dw3110::StartFrameDelimiter::Decawave_8));
	}

	bool
	run()
	{
		PT_BEGIN();
		while (true)
		{
			txdata[txdata.size() - 1]++;
			timeout.restart(Button::read() ? 100ms : 500ms);
			PT_WAIT_UNTIL(timeout.execute());
			MODM_LOG_INFO << "Transmitting Packet..." << modm::endl;
			if (!PT_CALL(radio.transmit(txdata)))
			{
				MODM_LOG_DEBUG << "Failed to trasmit!" << modm::endl;
			} else
			{
				MODM_LOG_DEBUG << "Transmitted 0x";
				for (size_t i = 0; i < txdata.size(); i++)
				{
					MODM_LOG_DEBUG << modm::hex << txdata[i];
				}
				MODM_LOG_DEBUG << modm::endl;
			}
		}
		PT_END();
	}

private:
	MyDw3110_b radio{};
	std::array<uint8_t, 5> txdata = {0xBA, 0xDE, 0xAF, 0xFE, 0x00};
	modm::Timeout timeout{100ms};
};

class RXThread : public modm::pt::Protothread
{
public:
	bool
	init()
	{
		return RF_CALL_BLOCKING(radio.initialize(modm::Dw3110::Channel::Channel5,
												 modm::Dw3110::PreambleCode::Code_64Mhz_9,
												 modm::Dw3110::PreambleLength::Preamble_4096,
												 modm::Dw3110::StartFrameDelimiter::Decawave_8));
	}

	bool
	run()
	{
		PT_BEGIN();
		while (true)
		{
			MODM_LOG_INFO << "Checking Packet..." << modm::endl;
			while (!PT_CALL(radio.packetReady()))
			{
				if (!PT_CALL(radio.isReceiving()))
				{
					MODM_LOG_INFO << "Starting RX..." << modm::endl;
					PT_CALL(radio.startReceive());
				}
				PT_YIELD();
			}

			MODM_LOG_INFO << "Fetching Packet..." << modm::endl;
			if (PT_CALL(radio.fetchPacket(rxdata, rxlen)))
			{
				MODM_LOG_DEBUG << modm::ascii << "Got packet of length " << rxlen << modm::endl;
				MODM_LOG_DEBUG << "Got 0x";
				for (size_t i = 0; i < rxlen; i++) { MODM_LOG_DEBUG << modm::hex << rxdata[i]; }
				MODM_LOG_DEBUG << modm::endl;
			}
		}
		PT_END();
	}

private:
	MyDw3110_a radio{};
	size_t rxlen = 0;
	std::array<uint8_t, 125> rxdata = {};
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

	MODM_LOG_INFO << "Starting ping pong..." << modm::endl;
	while (true)
	{
		rx.run();
		tx.run();
	}

	return 0;
}
