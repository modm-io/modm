/*
 * Copyright (c) 2009-2011, 2017, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2016-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <unittest/controller.hpp>

using namespace modm::platform;

extern Board::LoggerDevice loggerDevice;

int
main()
{
	Board::initialize();
	Board::Leds::setOutput(modm::Gpio::Low);
	Board::Leds::write(0b100);

	loggerDevice.write("Unittests (" __DATE__ ", " __TIME__")\n");

	Board::Leds::write(0b110);

	unittest::Controller::run(loggerDevice);

	Board::Leds::write(0b111);
	for (;;) {}
}
