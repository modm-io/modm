// coding: utf-8
/*
 * Copyright (c) 2022, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/pressure/ms5611.hpp>

using Cs = modm::platform::GpioC13;
using Mosi = modm::platform::GpioA7;
using Miso = modm::platform::GpioA6;
using Sck = modm::platform::GpioA5;

using SpiMaster = modm::platform::SpiMaster1;

modm::ms5611data::Data data;
modm::ms5611data::Prom prom;
modm::Ms5611<SpiMaster, Cs> barometer{data};

int
main()
{
	Board::initialize();
	Cs::setOutput(modm::Gpio::High);

	SpiMaster::connect<Miso::Miso, Sck::Sck>();
	SpiMaster::initialize<Board::SystemClock, 21500000>();

	MODM_LOG_INFO << "==========MS5611 Test==========" << modm::endl;

	while (not barometer.initialize())
	{
		MODM_LOG_ERROR << "Ms5611 PROM CRC failed" << modm::endl;
		modm::delay(1s);
	}

	MODM_LOG_INFO << "Ms5611 initialized and PROM CRC succeded" << modm::endl;

	prom = data.getProm();
	MODM_LOG_INFO << "MS5611 PROM Contents" << modm::endl;
	MODM_LOG_INFO << "C0: " << prom.data[0] << modm::endl;
	MODM_LOG_INFO << "C1: " << prom.data[1] << modm::endl;
	MODM_LOG_INFO << "C2: " << prom.data[2] << modm::endl;
	MODM_LOG_INFO << "C3: " << prom.data[3] << modm::endl;
	MODM_LOG_INFO << "C4: " << prom.data[4] << modm::endl;
	MODM_LOG_INFO << "C5: " << prom.data[5] << modm::endl;
	MODM_LOG_INFO << "C6: " << prom.data[6] << modm::endl;
	MODM_LOG_INFO << "C7: " << prom.data[7] << modm::endl;

	while (true)
	{
		barometer.readout();

		float pressure;
		data.getPressure(pressure);
		MODM_LOG_INFO << "Pressure: " << pressure << " mbar" << modm::endl;

		float temperature;
		data.getTemperature(temperature);
		MODM_LOG_INFO << "Temperature: " << temperature << " degrees Centigrade" << modm::endl;

		modm::delay(1s);
	}

	return 0;
}
