/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/temperature/tmp12x.hpp>
#include <modm/processing/timer.hpp>

using namespace Board;
using namespace std::chrono_literals;

/*
 * Example for the TMP12x driver
 * It assumes a TMP121 or TM123 is connected to the following pins:
 *   A4 CS
 *   A5 SCK
 *   A6 MISO
 */

using SpiCs = GpioA4;
using SpiSck = GpioA5;
using SpiMiso = GpioA6;

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "TMP12x test" << modm::endl;

	SpiMaster1::connect<SpiSck::Sck, SpiMiso::Miso>();
	SpiMaster1::initialize<SystemClock, 1'500'000_Hz>();

	modm::Tmp123<SpiMaster1, SpiCs> sensor;
	sensor.initialize();

	modm::PeriodicTimer timer{500ms};
	while(true) {
		if (timer.execute()) {
			const modm::Tmp123Temperature temperature = RF_CALL_BLOCKING(sensor.read());
			MODM_LOG_INFO.printf("Temperature %2.2f\n", (double)temperature.getTemperatureFloat());
		}
	}
}
