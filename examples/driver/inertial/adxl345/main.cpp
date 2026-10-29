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

#include <modm/board.hpp>
#include <modm/driver/inertial/adxl345.hpp>

using namespace Board;

using I2cSda = GpioA10;
using I2cScl = GpioA9;

uint8_t data[6];
modm::Adxl345<I2cMaster1> sensor(data, 0x53);

int
main()
{
	Board::initialize();
	I2cMaster1::connect<I2cSda::Sda, I2cScl::Scl>();
	I2cMaster1::initialize<SystemClock, 400_kBd>();

	MODM_LOG_INFO << "ADXL345 demo" << modm::endl;
	if (not sensor.configure()) {
		MODM_LOG_ERROR << "Failed to configure sensor!" << modm::endl;
	}

	while (true)
	{
		sensor.readAccelerometer();
		sensor.update();
		if (sensor.isNewDataAvailable())
		{
			const uint8_t *d = sensor.getData();
			const auto axis = [d](uint8_t i) { return int16_t(d[2*i] | (d[2*i + 1] << 8)); };
			MODM_LOG_INFO.printf("x=%6d y=%6d z=%6d\n", axis(0), axis(1), axis(2));
		}
		modm::delay(100ms);
	}
}
