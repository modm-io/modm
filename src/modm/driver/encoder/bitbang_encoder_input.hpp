/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BITBANG_ENCODER_INPUT_HPP
#define MODM_BITBANG_ENCODER_INPUT_HPP

#include <bit>
#include <modm/platform.hpp>
#include <type_traits>

namespace modm
{
/**
 * @ingroup 				modm_driver_bitbang_encoder_input
 * @author  				Thomas Sommer
 *
 * @brief					This driver decodes a AB (incremental) encoder signal
 *
 * @tparam SignalA          First modm::platform::Gpio pin to input the encoder signal.
 * @tparam SignalB          Second modm::platform::Gpio pin to input the encoder signal.
 * @tparam POSTSCALER	 	n_cycles to count as one in-/decrement.
 * @tparam DeltaType		Must be signed integer and fit at least POSTSCALER. The Bigger
 * 							DeltaType, the more inc-/decrements can be stored temporarily.
 */
template<typename SignalA, typename SignalB, uint8_t POSTSCALER = 4,
		 std::signed_integral DeltaType = int8_t>
class BitBangEncoderInput
{
	static_assert(std::popcount(POSTSCALER) == 1,
				  "POSTSCALER must be an integer to basis 2 and not 0: 1, 2, 4, 8, 16, ...");
	static_assert(POSTSCALER <= std::numeric_limits<DeltaType>::max(),
				  "DeltaType is to small for POSTSCALER.");

	using Signals = modm::platform::SoftwareGpioPort<SignalA, SignalB>;

	static_assert(Signals::number_of_ports == 1,
				  "Signal A/B must be on the same GPIO port to prevent signal tearing!");

	uint8_t inline getRaw();

public:
	using ValueType = DeltaType;
	BitBangEncoderInput() : raw_last(0), delta(0){};

	// Connect SingalA and SignalB and store power-up state
	inline void
	connect();

	// Call @1kHz for manual movement
	inline void
	update();

	ValueType
	getIncrement();

private:
	uint8_t raw_last;
	DeltaType delta;
};
}  // namespace modm

#include "bitbang_encoder_input_impl.hpp"

#endif  // MODM_BITBANG_ENCODER_INPUT_HPP
