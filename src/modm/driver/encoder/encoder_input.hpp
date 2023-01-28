/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <modm/platform.hpp>

namespace modm
{
/**
 * @ingroup				modm_driver_encoder_input
 * @brief 				Gray code decoder using STM32 Timer mode
 *
 * @tparam Timer 		STM32 General purpose timer
 * @tparam SignalA		First modm::platform::Gpio pin that connects to Timer::Ch1
 * @tparam SignalB		Second modm::platform::Gpio pin that connects to Timer::Ch2
 * @tparam PRESCALER	How many encoder pulses to count as one
 * @tparam DeltaType	For very fast or high resolution encoder, int32_t may be required
 */
template<class Timer, typename SignalA, typename SignalB, uint16_t PRESCALER = 4,
	std::signed_integral DeltaType = int16_t>
requires (std::numeric_limits<DeltaType>::max() < std::pow(2, 31))
class EncoderInput
{
public:
	void
	initialize(
		const uint8_t filter = 4,
		const modm::platform::Gpio::InputType inputType = modm::platform::Gpio::InputType::PullUp
	) {
		SignalA::setInput(inputType);
		SignalB::setInput(inputType);

		Timer::template connect<typename SignalA::Ch1, typename SignalB::Ch2>();

		Timer::enable();

		Timer::setMode(Timer::Mode::UpCounter, Timer::SlaveMode::Encoder3);
		Timer::setPrescaler(PRESCALER);

		if(filter) {
			Timer::template configureInputChannel<typename SignalA::Ch1>(filter);
			Timer::template configureInputChannel<typename SignalB::Ch2>(filter);
		}

		Timer::applyAndReset();
		Timer::start();
		last = Timer::getValue();
	}

	uint16_t
	getValue()
	{
		return Timer::getValue();
	}

	DeltaType
	getDelta()
	{
		uint16_t current = Timer::getValue();
		const DeltaType delta = current - last;
		last = current;

		return delta;
	}


private:
	uint16_t last = 0;
};
}  // namespace modm
