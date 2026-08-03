// coding: utf-8
/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/timer.hpp>
#include <chrono>
#include <limits>

namespace modm
{

/// @ingroup modm_driver_ltc2497
struct ltc2497
{
	/// Three-state wiring option of one address pin (CA0, CA1 or CA2).
	enum class
	AddressPin : uint8_t
	{
		Low = 0,
		High = 1,
		Float = 2,
	};

	/// The 27 pin-selectable 7-bit addresses.
	static constexpr uint8_t
	address(AddressPin ca2 = AddressPin::Low, AddressPin ca1 = AddressPin::Low, AddressPin ca0 = AddressPin::Low)
	{
		constexpr uint8_t table[3][3][3]
		{
			// ca1 = Low            ca1 = High           ca1 = Float
			{ {0x14, 0x16, 0x15},  {0x26, 0x34, 0x27},  {0x17, 0x25, 0x24} }, // ca2 = Low
			{ {0x56, 0x64, 0x57},  {0x74, 0x76, 0x75},  {0x65, 0x67, 0x66} }, // ca2 = High
			{ {0x35, 0x37, 0x36},  {0x47, 0x55, 0x54},  {0x44, 0x46, 0x45} }, // ca2 = Float
		};
		return table[uint8_t(ca2)][uint8_t(ca1)][uint8_t(ca0)];
	}

	/// Reserved general call address to synchronize the conversion start of
	/// multiple LTC24xx delta-sigma devices on the same bus (write-only).
	static constexpr uint8_t GlobalAddress = 0x77;

	/// Worst-case (max) conversion time: simultaneous 50Hz/60Hz rejection with
	/// auto-calibration.
	static constexpr std::chrono::milliseconds ConversionTime{150};

	enum class
	InputChannel : uint8_t
	{
		// Differential inputs
		Ch0Ch1 = 0xA0,
		Ch1Ch0 = 0xA8,
		Ch2Ch3 = 0xA1,
		Ch3Ch2 = 0xA9,
		Ch4Ch5 = 0xA2,
		Ch5Ch4 = 0xAA,
		Ch6Ch7 = 0xA3,
		Ch7Ch6 = 0xAB,
		Ch8Ch9 = 0xA4,
		Ch9Ch8 = 0xAC,
		Ch10Ch11 = 0xA5,
		Ch11Ch10 = 0xAD,
		Ch12Ch13 = 0xA6,
		Ch13Ch12 = 0xAE,
		Ch14Ch15 = 0xA7,
		Ch15Ch14 = 0xAF,

		// Single-ended inputs
		Ch0 = 0xB0,
		Ch1 = 0xB8,
		Ch2 = 0xB1,
		Ch3 = 0xB9,
		Ch4 = 0xB2,
		Ch5 = 0xBA,
		Ch6 = 0xB3,
		Ch7 = 0xBB,
		Ch8 = 0xB4,
		Ch9 = 0xBC,
		Ch10 = 0xB5,
		Ch11 = 0xBD,
		Ch12 = 0xB6,
		Ch13 = 0xBE,
		Ch14 = 0xB7,
		Ch15 = 0xBF,
	};

	static constexpr InputChannel
	channel(uint8_t channel)
	{
		return InputChannel(0xB0 | ((channel & 1) << 3) | ((channel & 0xF) >> 1));
	}

	// Data output is 24 bits: SIG, MSB, a 16-bit two's complement result and
	// 6 bits that are always 0 (the LTC2499 outputs 32 bits instead: SIG,
	// MSB, a 24-bit result and 6 meaningful sub-LSBs). SIG+MSB act as two
	// extra range bits on top of the 16-bit result, so the mid-scale offset
	// is 1<<17, see Table 1 of the datasheet.
	struct modm_packed
	Data
	{
		template <class I2cMaster>
		friend class Ltc2497;

		constexpr uint32_t
		getRawValue() const
		{
			return (uint32_t(data[0]) << 16) | (uint32_t(data[1]) << 8) | data[2];
		}

		constexpr int32_t
		getValue() const
		{
			return static_cast<int32_t>(getRawValue() >> 6) - (1 << 17);
		}

		constexpr float
		getVoltage(float vref) const
		{
			if (getRawValue() == 0xC00000) return std::numeric_limits<float>::infinity();
			if (getRawValue() == 0x3FFFC0) return -std::numeric_limits<float>::infinity();
			return static_cast<float>(getValue()) * vref / static_cast<float>(1 << 17);
		}

	protected:
		uint8_t data[3];
	};
};

/**
 * @tparam	I2cMaster	I2cMaster interface
 *
 * The LTC2497 continuously converts. A write selects the mux for the next
 * conversion, while a read returns the previous result and starts a new one.
 * Since a read also starts a new conversion, every method below waits for
 * `tmr` (armed by whichever call started the conversion currently in
 * progress) before issuing its own I2C transaction, and rearms it for
 * `ConversionTime` again afterwards.
 */
template <typename I2cMaster>
class Ltc2497 : public ltc2497, public modm::I2cDevice<I2cMaster>
{
public:
	inline Ltc2497(Data &data, uint8_t address = ltc2497::address()) :
		modm::I2cDevice<I2cMaster>(address), data(data) {}

	/// Selects the mux for the next conversion and starts it.
	bool inline
	startMeasurement(InputChannel channel)
	{
		tmr.wait();
		const uint8_t command = static_cast<uint8_t>(channel);
		const bool success = this->write(&command, 1);
		if (success) tmr.restart(ConversionTime);
		return success;
	}

	/// Reads the latest conversion result and restarts conversion on the current mux.
	bool inline
	readConversionResult()
	{
		tmr.wait();
		const bool success = I2cDevice<I2cMaster>::read(data.data, 3);
		if (success) tmr.restart(ConversionTime);
		return success;
	}

	/// Reads the latest conversion result and selects the next mux in one transfer.
	bool inline
	readConversionResult(InputChannel nextInput)
	{
		tmr.wait();
		const uint8_t command = static_cast<uint8_t>(nextInput);
		const bool success = this->writeRead(&command, 1, data.data, 3);
		if (success) tmr.restart(ConversionTime);
		return success;
	}

	bool inline
	read(InputChannel channel)
	{
		if (not startMeasurement(channel)) return false;
		return readConversionResult();
	}

	inline Data &
	getData()
	{
		return data;
	}

private:
	Data &data;
	modm::ShortTimeout tmr;
};

} // modm namespace
