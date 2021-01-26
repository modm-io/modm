/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/driver/temperature/stts22h.hpp>

using namespace Board;
using namespace std::literals;

using I2cMaster = I2cMaster1;

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	I2cMaster::connect<GpioB7::Sda, GpioB6::Scl>();
	I2cMaster::initialize<Board::SystemClock, 100_kHz>();

	MODM_LOG_INFO << "Welcome to STTS22H Test" << modm::endl;

	modm::stts22h::Data data{};
	modm::Stts22h<I2cMaster> sensor{data, 0x3f};

	bool success = RF_CALL_BLOCKING(sensor.initialize());
	if(!success)
	{
		MODM_LOG_ERROR << "Initialization failed" << modm::endl;
	}

	modm::PeriodicTimer timer{500ms};
	while (true)
	{
		if (timer.execute())
		{
			LedD13::toggle();

			RF_CALL_BLOCKING(sensor.readTemperature());
			MODM_LOG_INFO << "temperature: " << data.getTemperatureFractional();
			MODM_LOG_INFO << " [1/100th °C]" << modm::endl;
		}
	}

	return 0;
}
