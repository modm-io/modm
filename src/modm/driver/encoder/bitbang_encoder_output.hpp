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
#define MODM_BITBANG_ENCODER_OUTPUT_HPP

#include <modm/processing/timer/periodic_timer.hpp>

namespace modm
{

/**
 * @ingroup 				modm_driver_bitbang_encoder_output
 * @author  				Raphael Lehmann
 *
 * @brief					This driver generates a AB (incremental) encoder signal to
 *							emulate a hardware encoder with a microcontroller.
 *
 * @tparam SignalA			First modm::platform::Gpio pin to output the encoder signal.
 * @tparam SignalB			Second modm::platform::Gpio pin to output the encoder signal.
 * @tparam PositionType		Data type (integer) of the position value, default `int32_t`.
 * @tparam PeriodicTimer	Defaults to `modm::PeriodicTimer`, must be replaced for
 *							encoder frequencies above 1kHz by a class that offers
 *							time steps less than 1ms, e.g. `modm::PrecisePeriodicTimer`.
 * @tparam period			Timebase for the output signal. This limits the maximal
 *							frequency of the encoder signal. Defaults to `1`.
 */
template<class SignalA, class SignalB, typename PositionType = int32_t,
		 class PeriodicTimer = modm::PeriodicTimer, uint32_t period = 1>
class BitBangEncoderOutput
{
public:
	/**
	 * @brief Initializes the Encoder.
	 *
	 * Sets pins (SignalA and SignalB) as output pins.
	 *
	 * @param initialValue for the encoder. Useful with unsigned PositionType
	 *          data types. Defaults to 0.
	 */
	BitBangEncoderOutput(PositionType initialValue);

	/**
	 * @brief Update method. Generates the encoder signal.
	 *
	 * Call this function in your main loop.
	 */
	void
	update();

	/**
	 * @brief Set the position for the encoder.
	 * @param position The position setpoint for the encoder.
	 */
	void
	setPosition(PositionType position)
	{
		setpoint = position;
	};

private:
	PositionType setpoint;
	PositionType actualValue;
	PeriodicTimer timer;
	enum class State : uint8_t
	{
		State0,
		State1,
		State2,
		State3,
	};
	State state;
};

}  // namespace modm

#include "bitbang_encoder_output_impl.hpp"

#endif  // MODM_BITBANG_ENCODER_OUTPUT_HPP
