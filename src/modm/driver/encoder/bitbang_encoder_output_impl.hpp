// coding: utf-8
/*
 * Copyright (c) 2019, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BITBANG_ENCODER_OUTPUT_HPP
#error "Don't include this file directly, use 'bitbang_encoder_output.hpp' instead!"
#endif
#include "bitbang_encoder_output.hpp"

template<class SignalA, class SignalB, typename PositionType, class PeriodicTimer, uint32_t period>
modm::BitBangEncoderOutput<SignalA, SignalB, PositionType, PeriodicTimer,
						   period>::BitBangEncoderOutput(PositionType initialValue)
	: setpoint(initialValue), actualValue(initialValue), timer(period), state(State::State0)
{
	SignalA::setOutput();
	SignalB::setOutput();
	SignalA::set();
	SignalB::set();
	setpoint = initialValue;
	actualValue = initialValue;
}

template<class SignalA, class SignalB, typename PositionType, class PeriodicTimer, uint32_t period>
void
modm::BitBangEncoderOutput<SignalA, SignalB, PositionType, PeriodicTimer, period>::update()
{
	if (timer.execute())
	{
		if (setpoint > actualValue)
		{
			// generate forward tick
			switch (state)
			{
				case State::State0:
					SignalA::reset();
					state = State::State1;
					break;
				case State::State1:
					SignalB::reset();
					state = State::State2;
					break;
				case State::State2:
					SignalA::set();
					state = State::State3;
					break;
				case State::State3:
					SignalB::set();
					state = State::State0;
					break;
			}
			actualValue++;
		} else if (setpoint < actualValue)
		{
			// generate backward tick
			switch (state)
			{
				case State::State0:
					SignalB::reset();
					state = State::State3;
					break;
				case State::State1:
					SignalA::reset();
					state = State::State0;
					break;
				case State::State2:
					SignalB::set();
					state = State::State1;
					break;
				case State::State3:
					SignalA::set();
					state = State::State2;
					break;
			}
			actualValue--;
		}
	}
}
