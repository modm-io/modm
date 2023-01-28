// coding: utf-8
// ----------------------------------------------------------------------------
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

#ifndef MODM_ADS816X_HPP
#define MODM_ADS816X_HPP

#include <array>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/processing/timer/timeout.hpp>

namespace modm
{

/// @ingroup modm_driver_ads816x
struct ads816x
{
	enum class
	Register : uint16_t
	{
		REG_ACCESS		= 0x000,
		PD_CNTL			= 0x004,
		SDI_CNTL		= 0x008,
		SDO_CNTL1		= 0x00C,
		SDO_CNTL2		= 0x00D,
		SDO_CNTL3		= 0x00E,
		SDO_CNTL4		= 0x00F,
		DATA_CNTL		= 0x010,
		PARITY_CNTL		= 0x011,

		OFST_CAL		= 0x018,
		REF_MRG1		= 0x019,
		REF_MRG2		= 0x01A,
		REFby2_MRG		= 0x01B,

		AIN_CFG			= 0x024,
		COM_CFG			= 0x027,

		DEVICE_CFG		= 0x01C,
		CHANNEL_ID		= 0x01D,
		SEQ_START		= 0x01E,
		SEQ_STOP		= 0x01F,
		ON_THE_FLY_CFG	= 0x02A,
		AUTO_SEQ_CFG1	= 0x080,
		AUTO_SEQ_CFG2	= 0x082,

		// Alert, etc...
	};

	enum class
	Command : uint8_t
	{
		NoOperation		= 0b00000 << 3,
		Write			= 0b00001 << 3,
		Read			= 0b00010 << 3,
		SetBits			= 0b00011 << 3,
		ClearBits		= 0b00100 << 3,
	};

	enum class
	Mode : uint8_t
	{
		Manual			= 0b00,
		OnTheFly		= 0b01,
		AutoSequence	= 0b10,
		CustomSequence	= 0b11,
	};

	enum class
	DataFormat : uint8_t
	{
		OnlyResult			= (0b00 << 4),
		ResultChannel		= (0b01 << 4),
		ResultChannelStatus	= (0b10 << 4),
		//Reserved			= (0b11 << 4),
	};
};

/**
 * @tparam	SpiMaster	SpiMaster interface
 * @tparam	Cs			Chip-select pin
 *
 * @author	Raphael Lehmann
 * @ingroup modm_driver_ads816x
 */
template <typename SpiMaster, typename Cs>
class Ads816x : public ads816x, public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<3>
{
public:
	Ads816x() = default;

	/// Call this function before using the device or to change operation mode
	/// \warning Only Mode::Manual is currently supported!
	modm::ResumableResult<void>
	initialize(Mode mode = Mode::Manual);

	/// Initiate a single conversion and return the result of the conversion.
	/// Simultanously the channel for the after next conversion will be set,
	/// i.e. before reading the first valid data two dummy conversions have to
	/// be executed.
	modm::ResumableResult<uint16_t>
	manualModeConversion(uint8_t afterNextChannel);

	/*
	// FIXME: Does not work correctly...
	/// Start a "auto sequence" conversion.
	/// The device will automatically cycle through the specified channels in
	/// the bitmask.
	template<std::size_t N>
	modm::ResumableResult<void>
	autoSequenceConversion(uint8_t channelsBitmask, std::span<uint16_t, N> result);
	*/

	/// Set T_CONV (minimal) value.
	/// According to the datasheet 660ns (-> 1us) for ADS8168, 1200ns (-> 2us)
	/// for ADS8167 and 2500ns (3us) for ADS8166 are the minimal values.
	/// Defaults to 3us, which works for all variants of the chip.
	void
	setTConv(modm::ShortPreciseDuration t)
	{
		tConv = t;
	}

private:
	modm::ResumableResult<uint8_t>
	registerAccess(Command command, Register reg, uint8_t value = 0);

	uint8_t buffer[3];
	uint8_t buffer2[3];

	modm::ShortPreciseTimeout timeout;
	modm::ShortPreciseDuration tConv = std::chrono::microseconds(3);
};

} // namespace modm

#include "ads816x_impl.hpp"

#endif // MODM_ADS816X_HPP
