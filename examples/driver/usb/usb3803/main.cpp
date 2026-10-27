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
#include <modm/driver/usb/usb3803.hpp>

using namespace Board;

using I2c = I2cMaster1;
using Scl = GpioB8;
using Sda = GpioB9;
// Connected to the RESET_N pin of the USB3803
using HubReset = GpioA8;

modm::Usb3803<I2c> hub;

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "USB3803 USB 2.0 Hub Example" << modm::endl;

	I2c::connect<Scl::Scl, Sda::Sda>();
	I2c::initialize<SystemClock, 100_kHz>();

	// The I2C port is only available in the configuration stage after reset
	HubReset::setOutput(modm::Gpio::Low);
	modm::delay(10ms);
	HubReset::set();

	// Hold the configuration stage before it times out after 400ms
	const auto start = modm::Clock::now();
	while (not hub.ping())
	{
		if (modm::Clock::now() - start > 300ms)
		{
			MODM_LOG_ERROR << "USB3803 does not respond!" << modm::endl;
			while (true) {}
		}
	}
	hub.setInterlock(true);

	modm::usb3803::Identity identity;
	if (hub.readIdentity(identity))
	{
		MODM_LOG_INFO.printf("VID=%04x PID=%04x DID=%04x\n",
				identity.vid, identity.pid, identity.did);
	}

	hub.setSelfPowered();
	hub.setDisabledPorts(modm::usb3803::Port::Port3, modm::usb3803::Port::Port3);
	hub.setLanguageId();
	hub.setString(modm::usb3803::String::Manufacturer, "modm");
	hub.setString(modm::usb3803::String::Product, "USB3803 Example Hub");
	hub.setString(modm::usb3803::String::Serial, "0123456789");
	hub.setStringsEnabled();

	// Release the configuration stage, the hub connects and the serial port
	// is no longer available afterwards
	hub.setInterlock(false);

	while (true)
	{
		Board::Leds::toggle();
		modm::delay(500ms);
	}
}
