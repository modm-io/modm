/*
 * Copyright (c) 2023, Zühlke (Austria) gmbh
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// The Repetition count register combined with one pulse mode is a great
// tool to enable the timer to produce a limited amount of pulses in PWM
// mode. In this example Timer1 is configured to produce 50 PWM pulses
// with 90% of duty cycle.

#include <modm/architecture/interface/interrupt.hpp>
#include <modm/board.hpp>
#include <modm/platform.hpp>

using namespace Board;

volatile uint32_t counter(0);
constexpr uint32_t number_of_pulses = 50;

template<class Timer>
void
advancedTimerConfig()
{
	Timer::enable();
	Timer::setMode(Timer::Mode::UpCounter, Timer::SlaveMode::Disabled,
				   Timer::SlaveModeTrigger::Internal0, Timer::MasterMode::Update, true);
	Timer::setPrescaler(84);
	Timer::setOverflow(9999);
	Timer::enableInterruptVector(Timer::Interrupt::CaptureCompare1, true, 0);
	Timer::enableInterrupt(Timer::Interrupt::CaptureCompare1);
	Timer::setRepetitionCount(number_of_pulses - 1);

	Timer::enableOutput();
	Timer::configureOutputChannel(1, Timer::OutputCompareMode::Pwm, 999, Timer::PinState::Enable);
}

template<class Timer>
void
timerStart()
{
	Timer::applyAndReset();
	Timer::start();
}

MODM_ISR(TIM1_CC)
{
	modm::platform::Timer1::acknowledgeInterruptFlags(
		modm::platform::Timer1::InterruptFlag::CaptureCompare1);
	LedD13::toggle();
	counter++;
}

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG << "debug" << modm::endl;
	MODM_LOG_INFO << "info" << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR << "error" << modm::endl;

	advancedTimerConfig<Timer1>();
	timerStart<Timer1>();

	while (true)
	{
		MODM_LOG_DEBUG << "Timer 1 pulsed " << static_cast<uint8_t>(counter) << "times\r"
					   << modm::endl;
		modm::delay(0.5s);
	}

	return 0;
}
