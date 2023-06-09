/*
 * Copyright (c) 2022, Christopher Durand
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
#include <modm/driver/temperature/mcp990x.hpp>

using namespace Board;
using namespace std::literals;

using I2c = I2cMaster2;
using Sda = GpioA10;
using Scl = GpioOutputA9;

int main()
{
	Board::initialize();
	LedD13::setOutput();

	I2c::connect<Sda::Sda, Scl::Scl>();
	I2c::initialize<Board::SystemClock, 400_kHz>();

	MODM_LOG_INFO << "Welcome to MCP9902/3/4 Test" << modm::endl;

	modm::mcp990x::Data data{};
	// I2C address MCP990xT-1: 0x4c, -2: 0x4d, -A: adjustable, see datasheet
	modm::Mcp990x<I2c> sensor{data, 0x4d};

	// wait for sensor boot-up time
	modm::delay(15ms);

	bool success = RF_CALL_BLOCKING(sensor.initialize());
	if (!success)
	{
		MODM_LOG_ERROR << "Initialization failed" << modm::endl;
	}

	modm::PeriodicTimer timer{500ms};
	while (true)
	{
		if (timer.execute())
		{
			if (RF_CALL_BLOCKING(sensor.readInternalTemperature()))
			{
				MODM_LOG_INFO.printf("temperature: %3.3f\n °C\n", (double)data.getTemperature());
			}
			else
			{
				MODM_LOG_INFO << "Reading temperature failed!\n";
			}
		}
	}

	return 0;
}

