/*
 * Copyright (c) 2019 Benjamin Weps
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_HIFIVE1_HPP
#define MODM_HIFIVE1_HPP

#include <modm/platform.hpp>

#include <cstdint>

/// @ingroup modm_hifive1

namespace Board{

struct SystemClock {
	static constexpr uint32_t Frequency = 320'000'000;

	static bool inline
	enable()
	{
		//Configure the clock to use the PLL driven by the external Oscillator
		modm::platform::PRCI::enableExternalOscillator();
		modm::platform::PRCI::setupPll(modm::platform::PRCI::PllSource::ExternalOscillator,
									   false,
									   modm::platform::PRCI::PllPrediv::Div2,
									   80,
									   modm::platform::PRCI::PllPostDiv::Div2);
		//modm::platform::PRCI::disableInternalOscillator();
		return true;
	}
};

using LedRed   = modm::platform::GpioA22;
using LedGreen = modm::platform::GpioA19;
using LedBlue  = modm::platform::GpioA21;

inline void
initialize()
{
	SystemClock::enable();

	LedRed::setOutput();
	LedRed::setInverted(true);

	LedGreen::setOutput();
	LedGreen::setInverted(true);

	LedBlue::setOutput();
	LedBlue::setInverted(true);
}

}

#endif	// MODM_HIFIVE1_HPP
