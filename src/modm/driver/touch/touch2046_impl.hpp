// coding: utf-8
/*
 * Copyright (c) 2021, Raphael Lehmann
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include "touch2046.hpp"

#include <modm/math/utils/endianness.hpp>

template<class SpiMaster, class Cs, int16_t Width, int16_t Height>
modm::Touch2046<SpiMaster, Cs, Width, Height>::Touch2046()
{
	if constexpr ( spi::Support_DataSize_Bit16<SPI> ) {
		this->attachConfigurationHandler([]() {
			SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
			SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
			SpiMaster::setDataSize(SpiMaster::DataSize::Bit16);
		});
	} else {
		this->attachConfigurationHandler([]() {
			SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
			SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
		});
	}

	Cs::setOutput(true);
}

template<class SpiMaster, class Cs, int16_t Width, int16_t Height>
modm::ResumableResult<void>
modm::Touch2046<SpiMaster, Cs, Width, Height>::updateZ()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	RF_CALL(SpiMaster::transfer16(bufferWrite.begin(), bufferWrite.begin() + 3, bufferRead.begin()));

	z = 4095 + (bufferRead[1] >> 3) - (bufferRead[2] >> 3);

	if (this->releaseMaster())
		Cs::set();

	RF_END_RETURN();
}

template<class SpiMaster, class Cs, int16_t Width, int16_t Height>
modm::ResumableResult<void>
modm::Touch2046<SpiMaster, Cs, Width, Height>::updateXY()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	RF_CALL(SpiMaster::transfer16(bufferWrite.begin() + 2, bufferWrite.end(), bufferRead.begin()));

	if (this->releaseMaster())
		Cs::set();

	// TODO rewrite with algorithms
	x = (bufferRead[1] >> 3) + (bufferRead[3] >> 3) + (bufferRead[5] >> 3);
	y = (bufferRead[2] >> 3) + (bufferRead[4] >> 3) + (bufferRead[6] >> 3);

	static constexpr int scale_shift = 10;
	x = std::clamp<int16_t>((((uint32_t)(x * cal.FactorX) >> scale_shift) + cal.OffsetX), 0, Width);
	y = std::clamp<int16_t>((((uint32_t)(y * cal.FactorY) >> scale_shift) + cal.OffsetY), 0, Height);

	RF_END_RETURN();
}

template<class SpiMaster, class Cs, int16_t Width, int16_t Height>
modm::ResumableResult<bool>
modm::Touch2046<SpiMaster, Cs, Width, Height>::isTouched()
{
	RF_BEGIN();
	RF_CALL(updateZ());
	RF_END_RETURN(z > cal.ThresholdZ);
}

template<class SpiMaster, class Cs, int16_t Width, int16_t Height>
modm::ResumableResult<modm::glcd::Point>
modm::Touch2046<SpiMaster, Cs, Width, Height>::getTouchPoint()
{
	RF_BEGIN();
	RF_CALL(updateXY());

	switch(orientation) {
		case Orientation::Landscape0:
			RF_RETURN(modm::glcd::Point(Height - y, Width - x));
		case Orientation::Portrait90:
			RF_RETURN(modm::glcd::Point(x, Height - y));
		case Orientation::Landscape180:
			RF_RETURN(modm::glcd::Point(y, x));
		case Orientation::Portrait270:
			RF_RETURN(modm::glcd::Point(Width - x, y));
	}

	RF_END();
}

template<class SpiMaster, class Cs, int16_t Width, int16_t Height>
modm::ResumableResult<std::tuple<uint16_t, uint16_t>>
modm::Touch2046<SpiMaster, Cs, Width, Height>::getTouchPosition()
{
	RF_BEGIN();
	RF_CALL(updateXY());

	switch(orientation) {
		case Orientation::Landscape0:
			RF_RETURN(std::make_tuple(Height - y, Width - x));
		case Orientation::Portrait90:
			RF_RETURN(std::make_tuple(x, Height - y));
		case Orientation::Landscape180:
			RF_RETURN(std::make_tuple(y, x));
		case Orientation::Portrait270:
			RF_RETURN(std::make_tuple(Width - x, y));
	}

	RF_END();
}
