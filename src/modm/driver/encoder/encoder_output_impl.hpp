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

#ifndef	MODM_ENCODER_OUTPUT_HPP
#	error	"Don't include this file directly, use 'encoder_output.hpp' instead!"
#endif
#include "encoder_output.hpp"

template <
	class PinA,
	class PinB,
	typename PositionType,
	class PeriodicTimer,
	uint32_t period
	>
modm::EncoderOutput<PinA, PinB, PositionType, PeriodicTimer, period>::EncoderOutput(PositionType initialValue) :
	setpoint(initialValue), actualValue(initialValue), timer(period), state(State::State0)
{
	PinA::setOutput();
	PinB::setOutput();
	PinA::set();
	PinB::set();
	setpoint = initialValue;
	actualValue = initialValue;
}

template <
	class PinA,
	class PinB,
	typename PositionType,
	class PeriodicTimer,
	uint32_t period
	>
void
modm::EncoderOutput<PinA, PinB, PositionType, PeriodicTimer, period>::update()
{
	if(timer.execute()){
		if(setpoint > actualValue) {
			// generate forward tick
			switch(state) {
				case State::State0:
					PinA::reset();
					state = State::State1;
					break;
				case State::State1:
					PinB::reset();
					state = State::State2;
					break;
				case State::State2:
					PinA::set();
					state = State::State3;
					break;
				case State::State3:
					PinB::set();
					state = State::State0;
					break;
			}
			actualValue++;
		}
		else if(setpoint < actualValue) {
			// generate backward tick
			switch(state) {
				case State::State0:
					PinB::reset();
					state = State::State3;
					break;
				case State::State1:
					PinA::reset();
					state = State::State0;
					break;
				case State::State2:
					PinB::set();
					state = State::State1;
					break;
				case State::State3:
					PinA::set();
					state = State::State2;
					break;
			}
			actualValue--;
		}
	}
}
