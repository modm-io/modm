/*
 * Copyright (c) 2023, Zühlke Engineering (Austria) GmbH
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EXAMPLE_TimerHANDLER_HPP
#define EXAMPLE_TimerHANDLER_HPP

#include <modm/architecture/interface/interrupt.hpp>
#include <modm/board.hpp>
#include <modm/platform.hpp>

#include "adc_dma.hpp"

using namespace Board;
using namespace modm::platform;
using namespace std::chrono_literals;

template<class Timer>
void
advancedTimerConfig(uint8_t repetitionCount)
{
	Timer::enable();
	Timer::setMode(Timer::Mode::UpCounter, Timer::SlaveMode::Disabled,
				   Timer::SlaveModeTrigger::Internal0, Timer::MasterMode::Update, true);
	Timer::setPrescaler(84);
	Timer::setOverflow(9999);
	Timer::setRepetitionCount(repetitionCount);

	Timer::enableOutput();
	Timer::configureOutputChannel(1, Timer::OutputCompareMode::Pwm, 999, Timer::PinState::Enable);
}

template<class Timer>
static void
timerStart()
{
	Timer::applyAndReset();
	Timer::start();
}


#endif  // EXAMPLE_TimerHANDLER_HPP
