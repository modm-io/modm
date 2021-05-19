// coding: utf-8
/*
 * Copyright (c) 2017, Arjun Sarin
 * Copyright (c) 2017-2018, 2021, Niklas Hauser
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TCS3472_HPP
#define MODM_TCS3472_HPP

#include <stdint.h>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/math/utils.hpp>
#include <modm/ui/color.hpp>

namespace modm
{

// forward declaration for friending with tcs3472::Data
template < class I2cMaster >
class Tcs3472;

/// @ingroup	modm_driver_tcs3472
struct tcs3472
{
	/// Register addresses
	enum class
	RegisterAddress : uint8_t
	{
		ENABLE = 0x00,					///< Power up the device, Enable features
		ID = 0x12,						///< Read part-ID

		// Shutter configuration
		INTEGRATION_TIME = 0x01,		///< Shutter integration time
		GAIN = 0x0F,  					///< Shutter gain control

		// Waittime configuration
		WAIT_TIME = 0x03,				///< Waittime between samples. Conceived for power savings
		CONFIGURATION = 0x0D,			///< Setting WAITLONG multiplies WAIT_TIME by 12

		// Interrupt configuration
		LOW_THRESH_LOW_BYTE = 0x04,		///< Low byte of low interrupt threshold
		LOW_THRESH_HIGH_BYTE = 0x05,	///< High byte of low interrupt threshold
		HIGH_THRESH_LOW_BYTE = 0x06,	///< Low byte of high interrupt threshold
		HIGH_THRESH_HIGH_BYTE = 0x07,	///< High byte of high interrupt threshold
		INTERRUPT_PERSIST_FILTER = 0x0C,///< Set how much threshold-passing cycles are required, to trigger the interrupt
		RELOAD_INTERRUPT = 0x66,		///< Reset Interrupt special command

		// Data results
		CDATALOW = 0x14,				///< Low byte of ADC clear channel
		CDATAHIGH = 0x15,				///< High byte of ADC clear channel
		RDATALOW = 0x16,				///< Low byte of ADC red channel
		RDATAHIGH = 0x17,				///< High byte of ADC red channel
		GDATALOW = 0x18,				///< Low byte of ADC green channel
		GDATAHIGH = 0x19,				///< High byte of ADC green channel
		BDATALOW = 0x1A,				///< Low byte of ADC blue channel
		BDATAHIGH = 0x1B 				///< High byte of ADC blue channel
	};

	public:
	/// Analog rgbc gain control
	enum class
	Gain : uint8_t
	{
		X1 = 0,		///< x1 gain
		X4 = 1,		///< x4 gain
		X16 = 2,	///< x16 gain
		X60 = 3,	///< x60 gain
	};

	/// Light integration time in ms
	enum class
	IntegrationTime : uint8_t
	{
		MSEC_2_4 = 0xFF,	///< integrate over 2.4 ms
		MSEC_24 = 0xF6,		///< integrate over 24 ms
		MSEC_101 = 0xD5,	///< integrate over 101 ms
		MSEC_154 = 0xC0,	///< integrate over 154 ms
		MSEC_700 = 0x00,	///< integrate over 700 ms
	};

	/// Time to wait between samples in ms
	enum class
	WaitTime : uint8_t
	{
		MSEC_2_4 = 0xFF,	///< wait for 2.4 ms
		MSEC_204 = 0xAB,	///< wait for 204 ms>
		MSEC_614 = 0x00		///< wait for 614 ms>
	};

	/// Controls rate of interrupt to the host processor.
	enum class
	InterruptPersistence : uint8_t
	{
		EVERY = 0,		///< Interrupt for every new sample independent of thresholds
		CNT_1 = 1,		///< Interrupt after 1 cycle being below or above thresholds
		CNT_2 = 2,		///< Interrupt after 2 cycles being below or above thresholds
		CNT_3 = 3,		///< ...
		CNT_5 = 4,
		CNT_10 = 5,
		CNT_15 = 6,
		CNT_20 = 7,
		CNT_25 = 8,
		CNT_30 = 9,
		CNT_35 = 10,
		CNT_40 = 11,
		CNT_45 = 12,
		CNT_50 = 13,
		CNT_55 = 14,
		CNT_60 = 15,
	};

	/// Power-control and feature enable flags
	enum class
	Enable : uint8_t
	{
		POWER_ON = Bit0,
		ADC_ENABLE = Bit1,
		WAIT_ENABLE = Bit3,
		INTERRUPT_ENABLE = Bit4
	};
	MODM_FLAGS8(Enable);

	/** Device Address depends on version TCS3472x
	 *
	 * - 1 = 0x39 (with IR filter)
	 * - 3 = 0x39 (without IR filter)
	 * - 5 = 0x29 (with IR filter)
	 * - 7 = 0x29 (without IR filter)
	 */
	static constexpr uint8_t
	addr(uint8_t version=5)
	{ return version < 5 ? 0x39 : 0x29; }

	using Rgb = color::RgbT<uint16_t>;

	struct modm_packed
	Data
	{
		inline Rgb
		getColor()
		{ return {getRed(), getGreen(), getBlue()}; }

		inline uint16_t
		getClear()  { return getValue(0); }

		inline uint16_t
		getRed() { return getValue(1); }
		inline uint16_t
		getGreen() { return getValue(2); }
		inline uint16_t
		getBlue() { return getValue(3); }

		inline uint16_t
		operator[] (uint8_t index)
		{ return (index < 4) ? getValue(index) : 0; }

	protected:
		inline uint16_t
		getValue(uint8_t index)
		{ return modm::fromLittleEndian(reinterpret_cast<uint16_t*>(data)[index]); }

		// clear, red, green, blue
		uint8_t data[sizeof(uint16_t) * 4];
		template<class I2cMaster> friend class Tcs3472;
	};
};

/**
 * TCS3472X Digital Color Sensors
 *
 * @see		tcs3472
 * @author	David Hebbeker, Arjun Sarin, Thomas Sommer, Niklas Hauser
 * @ingroup	modm_driver_tcs3472
 */
template<typename I2cMaster>
class Tcs3472 : public tcs3472, public modm::I2cDevice<I2cMaster, 2>
{
public:
	Tcs3472(Data &data, uint8_t address = addr());

	// Typical Runmodes
	static constexpr Enable_t Enable_PollingMode = Enable::POWER_ON | Enable::ADC_ENABLE;
	static constexpr Enable_t Enable_InterruptMode = Enable_PollingMode | Enable::INTERRUPT_ENABLE;
	static constexpr Enable_t Enable_InterruptMode_Waittime = Enable_InterruptMode | Enable::INTERRUPT_ENABLE;

	/// Power up sensor and start conversions
	modm::ResumableResult<bool>
	initialize(Enable_t flags = Enable_PollingMode)
	{ return writeRegister(RegisterAddress::ENABLE, flags.value); }

	modm::ResumableResult<bool>
	configure(Gain gain = Gain::X1,
			  IntegrationTime int_time = IntegrationTime::MSEC_2_4);

public:
	/// The gain can be used to adjust the sensitivity of all ADC output channels.
	modm::ResumableResult<bool>
	setGain(Gain gain = Gain::X1)
	{ return writeRegister(RegisterAddress::GAIN, uint8_t(gain)); }

	/// Sets the integration time for the ADCs.
	modm::ResumableResult<bool>
	setIntegrationTime(IntegrationTime int_time = tcs3472::IntegrationTime::MSEC_2_4)
	{ return writeRegister(RegisterAddress::INTEGRATION_TIME, uint8_t(int_time)); }

	/// Sets the wait time for the ADCs.
	modm::ResumableResult<bool>
	setWaitTime(WaitTime wait_time, bool wait_long = false);

	/// Sets the low threshold for the interrupt-comparator
	modm::ResumableResult<bool>
	setInterruptLowThreshold(uint16_t threshold);

	/// Sets the high threshold for the interrupt-comparator
	modm::ResumableResult<bool>
	setInterruptHighThreshold(uint16_t threshold);

	/// The gain can be used to adjust the sensitivity of all ADC output channels.
	modm::ResumableResult<bool>
	setInterruptPersistenceFilter(InterruptPersistence value)
	{ return writeRegister(RegisterAddress::INTERRUPT_PERSIST_FILTER, uint8_t(value)); }

public:
	/// Resets the interrupt output.
	modm::ResumableResult<bool>
	reloadInterrupt();

	/// Read current samples of ADC conversions for all channels.
	modm::ResumableResult<bool>
	readColor()
	{ return readRegisters(RegisterAddress::CDATALOW, data.data, sizeof(data.data)); }

public:
	modm::ResumableResult<bool>
	readRegisters(RegisterAddress address, uint8_t *values, uint8_t count = 1);

	modm::ResumableResult<bool>
	writeRegister(RegisterAddress address, uint8_t value);

	Data& getData() { return data; }

protected:
	Data &data;
	uint8_t buffer[2];
};

}  // namespace modm

#include "tcs3472_impl.hpp"

#endif  // MODM_TCS3472_HPP
