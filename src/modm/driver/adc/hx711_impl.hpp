/*
 * Copyright (c) 2020, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_HX711_HPP
#	error "Don't include this file directly! Use 'hx711.hpp' instead."
#endif

namespace modm
{

template <typename Cfg>
ResumableResult<int32_t>
Hx711<Cfg>::singleConversion()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(Data::read() == modm::Gpio::Low);

	modm::delay_us(1);

	data = 0;
	for (uint8_t ii = 0; ii < 24; ++ii)
	{
		Sck::set();
		modm::delay_us(1);
		data = (data << 1) | Data::read();
		modm::delay_us(1);
		Sck::reset();
		modm::delay_us(1);
	}

	// Additional pulses for mode of next conversion
	for (uint8_t ii = 0; ii < static_cast<uint8_t>(Cfg::mode); ++ii) {
		Sck::set();
		modm::delay_us(1);
		Sck::reset();
		modm::delay_us(1);
	}

	// Fill up MSBs for negative numbers
	if (data & (1 << 23)) {
		data |= 0xff000000;
	}

	RF_END_RETURN(data);
}

} // modm namespace
