/*
 * Copyright (c) 2010-2012, Fabian Greif
 * Copyright (c) 2013-2014, 2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/platform.hpp>
#include <modm/driver/temperature/ds18b20.hpp>
#include <modm/io/iostream.hpp>

using namespace modm::platform;

using OneWirePin = GpioC2;
using OneWireMaster = BitBangOneWireMaster<OneWirePin>;

int
main()
{
	using systemClock = SystemClock;
	Uart0::connect<GpioD1::Txd, GpioD0::Rxd>();
	Uart0::initialize<systemClock, 9600>();

	// Enable interrupts, this is needed for every buffered UART
	enableInterrupts();

	// Create a IOStream for complex formatting tasks
	modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull > device;
	modm::IOStream output(device);

	output << "Welcome" << modm::endl;
	modm::delayMilliseconds(100);

	OneWireMaster::connect<OneWirePin::BitBang>();
	OneWireMaster::initialize<SystemClock>();

	if (!OneWireMaster::touchReset()) {
		output << "No devices found!" << modm::endl;
		modm::delayMilliseconds(100);
		while (1) {
			// wait forever
		}
	}

	// search for connected DS18B20 devices
	OneWireMaster::resetSearch(0x28);

	uint8_t rom[8];
	while (OneWireMaster::searchNext(rom)) {
		output << "found: " << modm::hex;
		for (uint8_t i = 0; i < 8; ++i) {
			output << rom[i];
		}
		output << modm::ascii << modm::endl;
		modm::delayMilliseconds(100);
	}
	output << "finished!" << modm::endl;

	// read the temperature from a connected DS18B20
	modm::Ds18b20< OneWireMaster > ds18b20(rom);

	ds18b20.startConversion();

	while (1)
	{
		if (ds18b20.isConversionDone())
		{
			int16_t temperature = ds18b20.readTemperature();

			output << "Temperature: " << temperature << modm::endl;
			modm::delayMilliseconds(100);

			ds18b20.startConversion();
		}
	}
}
