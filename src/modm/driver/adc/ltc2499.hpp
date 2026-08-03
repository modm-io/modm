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

/// @ingroup modm_driver_ltc2499
struct ltc2499
{
	/// Three-state wiring option of one address pin (CA0, CA1 or CA2).
	enum class
	AddressPin : uint8_t
	{
		Low = 0,
		High = 1,
		Float = 2,
	};

	/// The 27 pin-selectable 7bit addresses.
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

	/// Line frequency rejection mode (FA, FB config bits). Simultaneous
	/// rejects both 50Hz and 60Hz by at least 87dB; the single-frequency
	/// modes reject their target frequency by at least 110dB but are not
	/// as robust against the other line frequency.
	enum class
	RejectionMode : uint8_t
	{
		Simultaneous50Hz60Hz = 0,
		Hz50 = 1,
		Hz60 = 2,
	};

	/// Output data rate (SPD config bit). 2x speed disables the
	/// auto-calibration that removes offset and drift every conversion,
	/// in exchange for roughly double the output rate. This is ignored
	/// (always 1x) for temperature measurements.
	enum class
	SpeedMode : uint8_t
	{
		Speed1x = 0,
		Speed2x = 1,
	};

	/// Worst-case (max) conversion time for a given rejection/speed
	/// configuration, see tCONV_1/tCONV_2 in the datasheet.
	static constexpr std::chrono::milliseconds
	conversionTime(RejectionMode rejection, SpeedMode speed)
	{
		// indexed [speed][Simultaneous50Hz60Hz, Hz50, Hz60]
		constexpr uint32_t ms[2][3]
		{
			{150, 164, 137}, // 1x
			{ 76,  82,  67}, // 2x
		};
		return std::chrono::milliseconds{ms[speed == SpeedMode::Speed2x ? 1 : 0][uint8_t(rejection)]};
	}

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

	// Data output is 32 bits: SIG, MSB, a 24-bit two's complement result and
	// 6 sub-LSBs. SIG+MSB act as two extra range bits on top of the 24-bit
	// result, so the mid-scale offset is 1<<25.
	struct modm_packed
	Data
	{
		template <class I2cMaster>
		friend class Ltc2499;

		constexpr uint32_t
		getRawValue() const
		{
			return (uint32_t(data[0]) << 24) | (uint32_t(data[1]) << 16) |
					(uint32_t(data[2]) << 8) | data[3];
		}

		constexpr int32_t
		getValue() const
		{
			return static_cast<int32_t>(getRawValue() >> 6) - (1 << 25);
		}

		constexpr float
		getVoltage(float vref) const
		{
			if (getRawValue() == 0xC0000000) return std::numeric_limits<float>::infinity();
			if (getRawValue() == 0x3FFFFFC0) return -std::numeric_limits<float>::infinity();
			return static_cast<float>(getValue()) * vref / static_cast<float>(1 << 25);
		}

		/// The raw 24-bit result field (DATAOUT24 in the datasheet), used by
		/// the temperature formulas below. Unlike getValue(), this is not
		/// converted from offset-binary to a signed value, since the PTAT
		/// signal is always a small positive fraction of VREF.
		constexpr uint32_t
		getRawResult24() const
		{ return (getRawValue() >> 6) & 0xFFFFFF; }

		/// Converts a temperature-sensor reading (see `Ltc2499::readTemperature()`)
		/// to Kelvin, given the reference voltage.
		constexpr float
		getTemperatureKelvin(float vref) const
		{ return static_cast<float>(getRawResult24()) * vref / 1570.0f; }

		/// Converts a temperature-sensor reading (see `Ltc2499::readTemperature()`)
		/// to degrees Celsius, given the reference voltage.
		constexpr float
		getTemperatureCelsius(float vref) const
		{ return getTemperatureKelvin(vref) - 273.0f; }

	protected:
		uint8_t data[4];
	};

protected:
	/// @cond
	/// Builds the second (configuration) input byte: EN2, IM, FA, FB, SPD.
	static constexpr uint8_t
	configByte(RejectionMode rejection, SpeedMode speed, bool measureTemperature = false)
	{
		constexpr uint8_t rejectionBits[3] { 0x00, 0x10, 0x20 }; // Simultaneous50Hz60Hz, Hz50 (FB), Hz60 (FA)
		return 0x80 // EN2
				| (measureTemperature ? 0x40 : 0x00) // IM
				| rejectionBits[uint8_t(rejection)]
				| (speed == SpeedMode::Speed2x ? 0x08 : 0x00); // SPD
	}

	/// First input byte that keeps the previously selected channel instead
	/// of selecting a new one (preamble "10", EN = 0), used when only the
	/// configuration (e.g. for a temperature reading) needs to be updated.
	static constexpr uint8_t KeepChannel = 0x80;
	/// @endcond
};

/**
 * @tparam	I2cMaster	I2cMaster interface
 *
 * The LTC2499 continuously converts. A write selects the mux for the next
 * conversion, while a read returns the previous result and starts a new one.
 *
 * This driver exposes a rejection mode (50Hz, 60Hz or simultaneous), a 1x/2x
 * speed mode (2x roughly doubles the output rate by disabling the offset
 * auto-calibration) and the integrated temperature sensor.
 * Every `startMeasurement()`/`read()` call always sends both configuration
 * bytes, so the device configuration never depends on hidden prior state.
 *
 * Since a read also starts a new conversion, every method below waits for
 * `tmr` (armed by whichever call started the conversion currently in
 * progress, for however long that specific rejection/speed mode takes) before
 * issuing its own I2C transaction, and rearms it afterwards.
 */
template <typename I2cMaster>
class Ltc2499 : public ltc2499, public modm::I2cDevice<I2cMaster>
{
public:
	inline Ltc2499(Data &data, uint8_t address = ltc2499::address()) :
		modm::I2cDevice<I2cMaster>(address), data(data) {}

	/// Selects the mux and the rejection/speed mode for the next conversion, and starts it.
	bool inline
	startMeasurement(InputChannel channel, RejectionMode rejection = RejectionMode::Simultaneous50Hz60Hz,
			SpeedMode speed = SpeedMode::Speed1x)
	{
		tmr.wait();
		const uint8_t command[2] { uint8_t(channel), configByte(rejection, speed) };
		const bool success = this->write(command, 2);
		if (success) rearm(rejection, speed);
		return success;
	}

	/// Selects the internal temperature sensor and rejection mode for the
	/// next conversion, and starts it. Always uses 1x speed, as required
	/// by the datasheet.
	bool inline
	startTemperatureMeasurement(RejectionMode rejection = RejectionMode::Simultaneous50Hz60Hz)
	{
		tmr.wait();
		const uint8_t command[2] { KeepChannel, configByte(rejection, SpeedMode::Speed1x, /*measureTemperature=*/true) };
		const bool success = this->write(command, 2);
		if (success) rearm(rejection, SpeedMode::Speed1x);
		return success;
	}

	/// Reads the latest conversion result and restarts conversion with the current configuration.
	bool inline
	readConversionResult()
	{
		tmr.wait();
		const bool success = I2cDevice<I2cMaster>::read(data.data, 4);
		if (success) tmr.restart(currentConversionTime);
		return success;
	}

	/// Reads the latest conversion result and selects the next mux and
	/// rejection/speed mode in one transfer.
	bool inline
	readConversionResult(InputChannel nextInput, RejectionMode rejection = RejectionMode::Simultaneous50Hz60Hz,
			SpeedMode speed = SpeedMode::Speed1x)
	{
		tmr.wait();
		const uint8_t command[2] { uint8_t(nextInput), configByte(rejection, speed) };
		const bool success = this->writeRead(command, 2, data.data, 4);
		if (success) rearm(rejection, speed);
		return success;
	}

	/// Selects the mux/rejection/speed mode, waits for the conversion to
	/// finish (blocking) and reads the result.
	bool inline
	read(InputChannel channel, RejectionMode rejection = RejectionMode::Simultaneous50Hz60Hz,
			SpeedMode speed = SpeedMode::Speed1x)
	{
		if (not startMeasurement(channel, rejection, speed)) return false;
		return readConversionResult();
	}

	/// Selects the internal temperature sensor, waits for the conversion to
	/// finish (blocking) and reads the result. Use `Data::getTemperatureKelvin()`
	/// / `Data::getTemperatureCelsius()` to interpret the result.
	bool inline
	readTemperature(RejectionMode rejection = RejectionMode::Simultaneous50Hz60Hz)
	{
		if (not startTemperatureMeasurement(rejection)) return false;
		return readConversionResult();
	}

	inline Data &
	getData()
	{
		return data;
	}

private:
	void inline
	rearm(RejectionMode rejection, SpeedMode speed)
	{
		currentConversionTime = conversionTime(rejection, speed);
		tmr.restart(currentConversionTime);
	}

	Data &data;
	modm::ShortTimeout tmr;
	// the conversion time of whichever rejection/speed mode was configured
	// by the last successful startMeasurement()/startTemperatureMeasurement(),
	// used to rearm `tmr` after a plain readConversionResult() that doesn't
	// itself know the current configuration
	std::chrono::milliseconds currentConversionTime{
			conversionTime(RejectionMode::Simultaneous50Hz60Hz, SpeedMode::Speed1x)};
};

} // modm namespace
