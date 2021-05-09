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
#define MODM_TOUCH2046_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/processing/resumable.hpp>

#include <array>
#include <tuple>

namespace modm
{

/// \ingroup modm_driver_touch2046
struct touch2046 {
	enum class Orientation {
		Normal,
		//...
	};

	/**
	 * Calibration values are used to calculate touch point
	 * from raw values.
	 *
	 * \ref FactorX and \ref FactorY scaled by 1000000 to avoid float
	 * arithmetic. E.g. to get a factor of 0.75 \ref FactorX has to be
	 * set to 750'000.
	 *
	 * isTouched() = bool(Z > ThresholdZ)
	 *
	 * X = (rawX * FactorX / 1000000) + OffsetX
	 * limited to [0, MaxX]
	 * Y = (rawY * FactorY / 1000000) + OffsetY
	 * limited to [0, MaxY]
	 *
	 * Orientation (rotation, mirror) are applied after the
	 * above operations.
	 */
	struct Calibration
	{
		int16_t OffsetX = 0;
		int16_t OffsetY = 0;
		int32_t FactorX = 1'000'000;
		int32_t FactorY = 1'000'000;
		uint16_t MaxX = 240;
		uint16_t MaxY = 320;
		uint16_t ThresholdZ = 1500;
		Orientation orientation = Orientation::Normal;
	};
};

/**
 * \ingroup	modm_driver_touch2046
 * \author	Raphael Lehmann
 *
 * Datasheet TSC2046: https://www.ti.com/lit/ds/symlink/tsc2046.pdf
 */
template < class SpiMaster, class Cs>
class Touch2046 : public touch2046, public modm::SpiDevice< SpiMaster >, protected modm::NestedResumable<3>
{
public:
	/**
	 * Set calibration data
	 *
	 * See \ref Calibration.
	 */
	void
	setCalibration(Calibration calibration) {
		cal = calibration;
	}

	/**
	 * Get raw X, Y and Z values
	 *
	 * \return	Position and intensity of touch point. Full int16_t range.
	 */
	modm::ResumableResult<std::tuple<uint16_t,uint16_t,uint16_t>>
	getRawValues();

	/**
	 * Is screen touched?
	 *
	 * \return bool true if screen is touched
	 */
	modm::ResumableResult<bool>
	isTouched();

	/**
	 * Get touch position
	 *
	 * \return	Position (X, Y) of touch point.
	 */
	modm::ResumableResult<std::tuple<uint16_t,uint16_t>>
	getTouchPosition();

private:
	static constexpr uint8_t MeasureZ1 = 0xB1;
	static constexpr uint8_t MeasureZ2 = 0xC1;
	static constexpr uint8_t MeasureX = 0xD1;
	static constexpr uint8_t MeasureY = 0x91;
	static constexpr uint8_t Powerdown = 0b1111'1100;
	static constexpr std::array<uint8_t, 17> bufferWrite = {
		MeasureZ1, 0x00,
		MeasureZ2, 0x00,
		MeasureY, 0x00,
		MeasureX, 0x00,
		MeasureY, 0x00,
		MeasureX, 0x00,
		MeasureY, 0x00,
		(MeasureX & Powerdown), 0x00,
		0x00};
	std::array<uint16_t, 9> bufferRead = {};

	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t z = 0;

	Calibration cal;
};

} // modm namespace

#include "touch2046_impl.hpp"

#endif // MODM_TOUCH2046_HPP
