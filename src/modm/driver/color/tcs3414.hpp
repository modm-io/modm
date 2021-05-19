/*
 * Copyright (c) 2013, David Hebbeker
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013-2016, 2018, 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TCS3414_HPP
#define MODM_TCS3414_HPP

#include <stdint.h>
#include <modm/ui/color.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/math/utils/endianness.hpp>

namespace modm
{

// forward declaration for friending with tcs3414::Data
template < class I2cMaster >
class Tcs3414;

/// @ingroup	modm_driver_tcs3414
struct tcs3414
{
	/** Device Address depends on version TCS341x
	 *
	 * - TCS3413 = 0x29
	 * - TCS3414 = 0x39
	 * - TCS3415 = 0x49
	 * - TCS3416 = 0x59
	 */
	static constexpr uint8_t
	addr(uint8_t version=4)
	{ return ((version - 1) << 4) | 0x09; }

	/// Analog gain control
	enum class
	Gain : uint8_t
	{
		X1	= 0b000000,	///< x1 gain
		X4	= 0b010000,	///< x4 gain
		X16	= 0b100000,	///< x16 gain
		X64	= 0b110000,	///< x64 gain
	};

	/// Prescaler mode
	enum class
	Prescaler : uint8_t
	{
		D1		= 0, ///< divide by 1
		D2		= 1, ///< divide by 2
		D4		= 2, ///< divide by 4
		D8		= 3, ///< divide by 8
		D16		= 4, ///< divide by 16
		D32		= 5, ///< divide by 32
		D64		= 6, ///< divide by 64
	};

	/// Select mode how to choose the integration time
	enum class
	IntegrationMode : uint8_t
	{
		INTERNAL	= 0b000000,	///< integrates with the free-running mode
		ADC_CTR		= 0b010000,	///< use the ADC enable bit field in the control register to start and stop integration
		SYNC_NOM	= 0b100000,	///< use synchronize signal to integrate over nominal integration time
		SYNC_COUNT	= 0b110000,	///< integrate over pulse count pulses of the sync pin
	};

	/// Integration for a fixed time
	enum class
	NominalIntegrationTime : uint8_t
	{
		MSEC_12		= 0,	///< integrate over 12 ms
		MSEC_100	= 1,	///< integrate over 100 ms
		MSEC_400	= 2,	///< integrate over 400 ms
	};

	/// The number of pulses on sync pin to integrate over
	enum class
	SyncPulseCount : uint8_t
	{
		PULSES_1	= 0,	///< integrate over 1 pulses of sync pin
		PULSES_2	= 1,	///< integrate over 2 pulses of sync pin
		PULSES_4	= 2,	///< integrate over 4 pulses of sync pin
		PULSES_8	= 3,	///< integrate over 8 pulses of sync pin
		PULSES_16	= 4,	///< integrate over 16 pulses of sync pin
		PULSES_32	= 5,	///< integrate over 32 pulses of sync pin
		PULSES_64	= 6,	///< integrate over 64 pulses of sync pin
		PULSES_128	= 7,	///< integrate over 128 pulses of sync pin
		PULSES_256	= 8,	///< integrate over 256 pulses of sync pin
	};

	/// Register addresses
	enum class
	RegisterAddress : uint8_t
	{
		CONTROL					= 0x00,	///< Control of basic functions
		TIMING					= 0x01,	///< Integration time control  @see Tcs3414::setIntegrationTime
		INTERRUPT				= 0x02,	///< Interrupt settings
		INT_SOURCE				= 0x03,	///< Interrupt source
		ID						= 0x04,	///< Part number
		GAIN					= 0x07,	///< Sensitivity settings @see Tcs3414::setGain
		LOW_THRESH_LOW_BYTE		= 0x08,	///< Low byte of low interrupt threshold
		LOW_THRESH_HIGH_BYTE	= 0x09,	///< High byte of low interrupt threshold
		HIGH_THRESH_LOW_BYTE	= 0x0A,	///< Low byte of high interrupt threshold
		HIGH_THRESH_HIGH_BYTE	= 0x0B,	///< High byte of high interrupt threshold
		// Data registers
		DATA1LOW				= 0x10,	///< Low byte of ADC green channel
		DATA1HIGH				= 0x11,	///< High byte of ADC green channel
		DATA2LOW				= 0x12,	///< Low byte of ADC green channel
		DATA2HIGH				= 0x13,	///< High byte of ADC green channel
		DATA3LOW				= 0x14,	///< Low byte of ADC green channel
		DATA3HIGH				= 0x15,	///< High byte of ADC green channel
		DATA4LOW				= 0x16,	///< Low byte of ADC green channel
		DATA5HIGH				= 0x17	///< High byte of ADC green channel
	};


	using Rgb = color::RgbT<uint16_t>;

	struct modm_packed
	Data
	{
		inline Rgb
		getColor()
		{ return {getRed(), getGreen(), getBlue()}; }

		inline uint16_t
		getClear()  { return getValue(3); }
		inline uint16_t
		getRed() { return getValue(1); }
		inline uint16_t
		getGreen() { return getValue(0); }
		inline uint16_t
		getBlue() { return getValue(2); }

		inline uint16_t
		operator[] (uint8_t index)
		{ return (index < 4) ? getValue(index) : 0; }

	protected:
		inline uint16_t
		getValue(uint8_t index)
		{ return modm::fromLittleEndian(reinterpret_cast<uint16_t*>(data+2)[index]); }

		// [2B-alignment, read-byte-count], green, red, blue, clear
		uint8_t data[2 + sizeof(uint16_t) * 4];
		template<class I2cMaster> friend class Tcs3414;
	};
};

/**
 * TCS3414 Digital Color Sensors
 *
 * @todo	Not all features of the sensors are implemented in this driver yet.
 *
 * @see		tcs3414
 * @author	David Hebbeker, Niklas Hauser
 * @ingroup	modm_driver_tcs3414
 */
template < typename I2cMaster >
class Tcs3414 : public tcs3414, public modm::I2cDevice< I2cMaster, 2 >
{
public:
	Tcs3414(Data &data, uint8_t address = addr());

	modm::ResumableResult<bool>
	initialize()
	{ return writeRegister(RegisterAddress::CONTROL, 0b11); };

	modm::ResumableResult<bool>
	configure(Gain gain = Gain::X1, Prescaler prescaler = Prescaler::D1)
	{ return setGain(gain, prescaler); }

public:
	/// The gain can be used to adjust the sensitivity of all ADC output channels.
	modm::ResumableResult<bool>
	setGain(Gain gain = Gain::X1, Prescaler prescaler = Prescaler::D1)
	{ return writeRegister(RegisterAddress::GAIN, uint8_t(gain) | uint8_t(prescaler)); }

	/// Sets the integration time for the ADCs.
	modm::ResumableResult<bool>
	setIntegrationTime(IntegrationMode mode, NominalIntegrationTime time)
	{ return writeRegister(RegisterAddress::TIMING, uint8_t(mode) | uint8_t(time)); }

	/// Sets the integration time for the ADCs.
	modm::ResumableResult<bool>
	setIntegrationTime(IntegrationMode mode, SyncPulseCount time)
	{ return writeRegister(RegisterAddress::TIMING, uint8_t(mode) | uint8_t(time)); }

public:
	/// Read current samples of ADC conversions for all channels.
	modm::ResumableResult<bool>
	readColor()
	{ return readRegisters(RegisterAddress::DATA1LOW, data.data+1, sizeof(data.data)); }

public:
	modm::ResumableResult<bool>
	readRegisters(RegisterAddress address, uint8_t *values, uint8_t count = 1);

	modm::ResumableResult<bool>
	writeRegister(RegisterAddress address, uint8_t value);

protected:
	Data &data;
	uint8_t buffer[3];
};

}	// namespace modm

#include "tcs3414_impl.hpp"

#endif // MODM_TCS3414_HPP
