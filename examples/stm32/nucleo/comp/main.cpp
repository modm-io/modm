/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

#include <modm/platform/comp/comp_1.hpp>

int
main()
{
	Board::initialize();
	Board::LedD13::setOutput();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	using Comparator = modm::platform::Comp1;

	Comparator::connect<GpioA0::Out, GpioA1::Inp>();

	Comparator::initialize(
			Comparator::InvertingInput::Vref1Div2,
			Comparator::NonInvertingInput::GpioA1,
			Comparator::Hysteresis::NoHysteresis,
			Comparator::Mode::HighSpeed,
			Comparator::Polarity::NonInverted,
			false);

	while (1)
	{
		modm::delayMilliseconds(250);
		MODM_LOG_INFO << "Comparator: " << Comparator::getOutput() << modm::endl;
		Board::LedD13::set(Comparator::getOutput());
	}

	return 0;
}
