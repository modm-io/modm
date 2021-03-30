// coding: utf-8
/*
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TOUCH2046_HPP
	#error	"Don't include this file directly, use 'touch2046.hpp' instead!"
#endif


template < class SpiMaster, class Cs >
modm::ResumableResult<std::tuple<uint16_t,uint16_t,uint16_t>>
modm::Touch2046<SpiMaster, Cs>::getRawValues()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	RF_CALL(SpiMaster::transfer(
		bufferWrite.data(),
		reinterpret_cast<uint8_t*>(bufferRead.data()) + 1,
		17));

	z = 4095 + (modm::fromBigEndian(bufferRead[1]) >> 3)
		- (modm::fromBigEndian(bufferRead[2]) >> 3);

	y = (modm::fromBigEndian(bufferRead[3]) >> 3)
		+ (modm::fromBigEndian(bufferRead[5]) >> 3)
		+ (modm::fromBigEndian(bufferRead[7]) >> 3);

	x = (modm::fromBigEndian(bufferRead[4]) >> 3)
		+ (modm::fromBigEndian(bufferRead[6]) >> 3)
		+ (modm::fromBigEndian(bufferRead[8]) >> 3);

	if (this->releaseMaster()) {
		Cs::set();
	}

	RF_END_RETURN(std::make_tuple(x, y, z));
}

template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
modm::Touch2046<SpiMaster, Cs>::isTouched()
{
	RF_BEGIN();
	std::tie(std::ignore, std::ignore, z) = RF_CALL(getRawValues());
	RF_END_RETURN(z > cal.ThresholdZ);
}

template < class SpiMaster, class Cs >
modm::ResumableResult<std::tuple<uint16_t,uint16_t>>
modm::Touch2046<SpiMaster, Cs>::getTouchPosition()
{
	RF_BEGIN();

	std::tie(x, y, std::ignore) = RF_CALL(getRawValues());

	x = std::min<uint16_t>(
		((static_cast<int32_t>(x * cal.FactorX) / 1'000'000)
		+ cal.OffsetX),
		cal.MaxX);
	y = std::min<uint16_t>(
		((static_cast<int32_t>(y * cal.FactorY) / 1'000'000)
		+ cal.OffsetY),
		cal.MaxY);

	// todo: orientation processing

	RF_END_RETURN(std::make_tuple(x, y));
}
