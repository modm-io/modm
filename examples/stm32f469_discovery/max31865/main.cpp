// coding: utf-8
/*
 * Copyright (c) 2023, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/temperature/max31865.hpp>
#include <modm/processing.hpp>

using namespace Board;

using SpiMaster = modm::platform::SpiMaster2;

using Cs = D9;
using Mosi = D11;
using Miso = D12;
using Sck = D13;

class ThermocoupleThread : public modm::pt::Protothread
{
public:
	ThermocoupleThread() : data{}, pt100(data) {}

	bool
	run()
	{
		PT_BEGIN();
		PT_CALL(pt100.initialize());

		while (true)
		{
			MODM_LOG_INFO << "\nNew readout:" << modm::endl;
			PT_CALL(pt100.readout());

			MODM_LOG_INFO << "          resistance : " << data.getResistance() << " Ohm"
						  << modm::endl;
			MODM_LOG_INFO << "     temperature fast: " << data.getTemperatureFast() << " degrees"
						  << modm::endl;
			MODM_LOG_INFO << "  temperature precise: " << data.getTemperaturePrecise() << " degrees"
						  << modm::endl;

			timeout.restart(std::chrono::milliseconds(1000));
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		PT_END();
	}

private:
	using Max31865 = modm::Max31865<SpiMaster, Cs, modm::max31865::pt100>;
	Max31865::Data data;
	Max31865 pt100;

	modm::ShortTimeout timeout;
};

ThermocoupleThread pt100Thread{};

int
main()
{
	Board::initialize();
	Cs::setOutput(modm::Gpio::High);

	SpiMaster::connect<Miso::Miso, Mosi::Mosi, Sck::Sck>();
	SpiMaster::initialize<Board::SystemClock, 351_kHz>();

	MODM_LOG_INFO << "==========MAX 31865 Test==========" << modm::endl;
	MODM_LOG_DEBUG << "Debug logging here" << modm::endl;
	MODM_LOG_INFO << "Info logging here" << modm::endl;
	MODM_LOG_WARNING << "Warning logging here" << modm::endl;
	MODM_LOG_ERROR << "Error logging here" << modm::endl;
	MODM_LOG_INFO << "===============================" << modm::endl;

	while (true)
	{
		pt100Thread.run();
		Board::LedOrange::toggle();
	}

	return 0;
}