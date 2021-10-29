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

#include <array>
#include <tuple>

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/ui/graphic/display.hpp>

namespace modm
{

/// \ingroup modm_driver_touch2046
struct touch2046 {
	enum class Control : uint8_t
	{
		START = Bit7, 	// 1: Marks the control byte

		A2 = Bit6,		// see enum class ChDiff / ChSingleEnd
		A1 = Bit5,
		A0 = Bit4,
		MODE = Bit3,	// see enum class Mode
		REF = Bit2,		// see enum class Reference

		PD1 = Bit1,		// see enum class PowerDown
		PD0 = Bit0,
	};
	MODM_FLAGS8(Control);

	// Valid when Control::MODE is 0
 	enum class ChDiff : uint8_t
	{
		Z1 = int(Control::A0) | int(Control::A1),
		Z2 = int(Control::A2),
		X = int(Control::A0) | int(Control::A2),
		Y = int(Control::A0)
	};
	typedef Configuration<Control_t, ChDiff, 0b111'0000> ChDiff_t;

	// Valid when Control::MODE is 1
	enum class ChSingleEnd : uint8_t
	{
		TEMP0 = 0,
		Y = int(Control::A0),
		VBAT = int(Control::A1),
		Z1 = int(Control::A0) | int(Control::A1),
		Z2 = int(Control::A2),
		X = int(Control::A0) | int(Control::A2),
		AUX = int(Control::A0) | int(Control::A1),
		TEMP1 = int(Control::A0) | int(Control::A1) | int(Control::A2)
	};
	typedef Configuration<Control_t, ChSingleEnd, 0b111'0000> ChSingleEnd_t;

	enum class Mode : uint8_t
	{
		Res_12Bit = 0,
		Res_8Bit = int(Control::MODE)
	};
	typedef Configuration<Control_t, Mode, 0b1, 3> Mode_t;

	enum class Reference : uint8_t
	{
		Differential = 0,
		SingleEnded = int(Control::REF)
	};
	typedef Configuration<Control_t, Reference, 0b1, 2> Reference_t;

	enum class PowerDown : uint8_t
	{
		Auto = 0,
		RefOff_AdcOn = int(Control::PD0),
		RefOn_AdcOff = int(Control::PD1),
		AlwaysOn = int(Control::PD0) | int(Control::PD1)
	};
	typedef Configuration<Control_t, PowerDown, 0b11, 0> PowerDown_t;


	/**
	 * Calibration values are used to calculate touch point
	 * from raw values.
	 *
	 * isTouched() = bool(Z > ThresholdZ)
	 *
	 * X = (rawX * FactorX / 1000000) + OffsetX
	 * limited to [0, MaxX]
	 * Y = (rawY * FactorY / 1000000) + OffsetY
	 * limited to [0, MaxY]
	 */

	struct Calibration
	{
		int32_t FactorX = 24;
		int16_t OffsetX = 0;
		int32_t FactorY = 24;
		int16_t OffsetY = 0;
		uint16_t ThresholdZ = 1000;
	};
};

/**
 * \ingroup	modm_driver_touch2046
 * \author	Raphael Lehmann, Thomas Sommer
 *
 * Datasheet TSC2046: https://www.ti.com/lit/ds/symlink/tsc2046.pdf
 */
template < class SpiMaster, class Cs, Size R>
class Touch2046 : public touch2046, public modm::SpiDevice< SpiMaster >, protected modm::NestedResumable<3>
{
public:
	Touch2046();

	using Orientation = modm::graphic::Orientation;

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
	 * Is screen touched?
	 *
	 * \return bool true if screen is touched
	 */
	modm::ResumableResult<bool>
	isTouched();

	/**
	 * Get touch position as tuple
	 *
	 * \return	Position (X, Y) of touch point.
	 */
	modm::ResumableResult<std::tuple<uint16_t,uint16_t>>
	getTouchPosition();

	/**
	 * Get touch position as modm::shape::Point
	 *
	 * \return	Point of touch point.
	 */
	modm::ResumableResult<modm::shape::Point>
	getTouchPoint();

	void setOrientation(Orientation orientation)
	{ this->orientation = orientation; }

	Orientation getOrientation() const
	{ return orientation; }

private:
	modm::ResumableResult<void>
	updateZ();

	modm::ResumableResult<void>
	updateXY();

	static constexpr Control_t Measure = Control::START | Mode_t(Mode::Res_12Bit)
		| Reference_t(Reference::Differential) | PowerDown_t(PowerDown::RefOff_AdcOn);
	static constexpr std::array<uint16_t, 8> bufferWrite = {
		(Measure | ChDiff_t(ChDiff::Z1)).value,
		((Measure | ChDiff_t(ChDiff::Z2)) & ~PowerDown_t::mask()).value,
		(Measure | ChDiff_t(ChDiff::X)).value,
		(Measure | ChDiff_t(ChDiff::Y)).value,
		(Measure | ChDiff_t(ChDiff::X)).value,
		(Measure | ChDiff_t(ChDiff::Y)).value,
		(Measure | ChDiff_t(ChDiff::X)).value,
		((Measure | ChDiff_t(ChDiff::Y)) & ~PowerDown_t::mask()).value
	};
	std::array<uint16_t, 7> bufferRead = {};

public:
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t z = 0;

	Calibration cal;

	Orientation orientation = Orientation::Portrait90;
};
} // namespace modm

#include "touch2046_impl.hpp"