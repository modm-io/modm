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

// The comparator, its pins and its options differ between the devices
#if defined STM32F3
using Comparator = modm::platform::Comp2;

static void
initializeComparator()
{
	Comparator::connect<GpioA7::Inp, GpioA2::Out>();
	Comparator::initialize(
			Comparator::InvertingInput::Vref1Div2,
			Comparator::NonInvertingInput::BitUnset, // GpioA7
			Comparator::Output::Tim1Or8BkIn2,
			Comparator::Hysteresis::NoHysteresis,
			Comparator::Mode::HighSpeed,
			Comparator::Polarity::NonInverted,
			false);
}
#else
using Comparator = modm::platform::Comp1;

static void
initializeComparator()
{
	Comparator::connect<GpioA0::Out, GpioA1::Inp>();
	Comparator::initialize(
			Comparator::InvertingInput::Vref1Div2,
			Comparator::NonInvertingInput::GpioA1,
			Comparator::Hysteresis::NoHysteresis,
			Comparator::Mode::HighSpeed,
			Comparator::Polarity::NonInverted,
			false);
}
#endif

int
main()
{
	Board::initialize();
	Board::Leds::setOutput();

	initializeComparator();

	while (true)
	{
		modm::delay(250ms);
		MODM_LOG_INFO << "Comparator: " << Comparator::getOutput() << modm::endl;
		Board::Leds::write(Comparator::getOutput());
	}

	return 0;
}
