/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

using namespace Board;

// Complementary output on PWM0 Channel2 High / Low outputs
using OutH2 = GpioC19;
using OutL2 = GpioD26;

int main()
{
	Board::initialize();

	MODM_LOG_INFO << "PWM Test" << modm::endl;

	Pwm0::connect<OutH2::PwmH2, OutL2::PwmL2>();

	Pwm0::initialize();
	// Period 1500 => MCLK / 1500 = 100 kHz
	Pwm0::setPeriod(Pwm0::Channel::Ch2, 1500);
	// 500/1500 = 33.3% duty-cycle
	Pwm0::setDutyCycle(Pwm0::Channel::Ch2, 500);

	constexpr auto mode = Pwm0::ChannelMode()
		.setClock(Pwm0::ChannelClock::Mck)
		.setDeadTimeGeneration(Pwm0::DeadTimeGeneration::Enabled);

	Pwm0::setChannelMode(Pwm0::Channel::Ch2, mode);

	// Set 50 ticks dead-time for both high and low output
	const auto deadTimeL = 50; // ticks
	const auto deadTimeH = 50; // ticks
	Pwm0::setDeadTime(Pwm0::Channel::Ch2, deadTimeL, deadTimeH);

	// Set all outputs to low in override mode
	Pwm0::configureOutputOverrideValues(Pwm0::Outputs_t{});

	Pwm0::enableChannelOutput(Pwm0::Channels::Ch2);

	while (true)
	{
		Led0::toggle();
		Led1::toggle();
		modm::delay(500ms);

		// Activate override mode to force outputs to low when button is pressed
		const auto outputs = Pwm0::Outputs::Ch2PwmH | Pwm0::Outputs::Ch2PwmL;
		if (Button::read())
			Pwm0::setOutputOverride(outputs, false);
		else
			Pwm0::clearOutputOverride(outputs, false);

	}

	return 0;
}
