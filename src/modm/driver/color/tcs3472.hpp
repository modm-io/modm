// coding: utf-8
/*
 * Copyright (c) 2017, Arjun Sarin
 * Copyright (c) 2017-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * \file	tcs3472.hpp
 * \date	04 Feb 2017
 * \author	David Hebbeker, Arjun Sarin
 */

#ifndef MODM_TCS3472_HPP
#define MODM_TCS3472_HPP

#include <stdint.h>

#include <modm/ui/color.hpp>
#include <modm/architecture/interface/i2c_device.hpp>

namespace modm
{
/**
 * \brief 	Settings to configure the ams digital color sensor family tcs34721, -13, -15, 17
 * \see		tcs3472
 * \ingroup	driver_other
 *
 * Device   Address
 * tcs34721  0x39 (with IR filter)
 *        3  0x39 (without IR filter)
 *        5  0x29 (with IR filter)
 *        7  0x29 (without IR filter)
 *
 */
struct tcs3472
{
	/** @name Gain_Register
	 * @{
	 */

	//! \brief 	Analog rgbc gain control
	enum class Gain : uint8_t
	{
		X1	= 0b00,	//!< x1 gain
		X4	= 0b01,	//!< x4 gain
		X16	= 0b10,	//!< x16 gain
		X64	= 0b11,	//!< x60 gain
		DEFAULT = 0	//!< default value on chip reset
	};
	//! @}


	//! \brief 	Integration for a fixed time
	enum class IntegrationTime : uint8_t
	{
		MSEC_2		= 0xFF,		//!< integrate over 2.4 ms
		MSEC_24		= 0xF6,		//!< integrate over 100 ms
		MSEC_101	= 0xD5,		//!< integrate over 101 ms
		MSEC_154	= 0xC0,		//!< integrate over 154 ms
		MSEC_700	= 0x00,		//!< integrate over 700 ms
		DEFAULT = 0xFF			//!< default value on chip reset
	};
	//! @}


	//! \brief 	Register addresses
	enum class RegisterAddress : uint8_t
	{
		ENABLE				= 0x00,	//!< Primarily to power up the device
		TIMING				= 0x01,	//!< Integration time control  @see tcs3472::setIntegrationTime
		//INTERRUPT			= 0x02,	//!< Interrupt settings
		//INT_SOURCE			= 0x03,	//!< Interrupt source
		ID				= 0x12,	//!< Part number
		GAIN				= 0x0F,	//!< Sensitivity settings @see Tcs3414::setGain
		// Interrupt threshold registers
		LOW_THRESH_LOW_BYTE		= 0x04,	//!< Low byte of low interrupt threshold
		LOW_THRESH_HIGH_BYTE		= 0x05,	//!< High byte of low interrupt threshold
		HIGH_THRESH_LOW_BYTE		= 0x06,	//!< Low byte of high interrupt threshold
		HIGH_THRESH_HIGH_BYTE		= 0x07,	//!< High byte of high interrupt threshold
		// Data registers
		CDATALOW			= 0x14,	//!< Low byte of ADC clear channel
		CDATAHIGH			= 0x15,	//!< High byte of ADC clear channel
		RDATALOW			= 0x16,	//!< Low byte of ADC red channel
		RDATAHIGH			= 0x17,	//!< High byte of ADC red channel
		GDATALOW			= 0x18,	//!< Low byte of ADC green channel
		GDATAHIGH			= 0x19,	//!< High byte of ADC green channel
		BDATALOW			= 0x1A,	//!< Low byte of ADC blue channel
		BDATAHIGH			= 0x1B	//!< High byte of ADC blue channel
	};

	typedef uint16_t	UnderlyingType;		//!< datatype of color values
	typedef color::RgbT<UnderlyingType> Rgb;

};

/**
 * \brief	Tcs3472X Digital Color Sensors
 *
 * \todo	Not all features of the sensors are implemented in this driver
 * 			yet.
 *
 * \tparam	I2CMaster	I2C interface which needs an \em initialized
 * 						modm::i2c::Master
 * \see		tcs3472
 * \author	David Hebbeker, Arjun Sarin
 * \ingroup	driver_other
 */
template < typename I2cMaster >
class Tcs3472 : public tcs3472, public modm::I2cDevice< I2cMaster, 2 >
{
public:
	Tcs3472(uint8_t address = 0x29);

	//! \brief 	Power up sensor and start conversions
	// Blocking
	bool inline
	initializeBlocking()
	{
		return RF_CALL_BLOCKING(initialize());
	}

	//! \brief	The gain can be used to adjust the sensitivity of all ADC output channels.
	modm::ResumableResult<bool>
	setGain(
                        const Gain      gain      = Gain::DEFAULT)
	{
		return writeRegister(RegisterAddress::GAIN,
				static_cast<uint8_t>(gain));
	}

	/**
	 * @name Return already sampled color
	 * @{
	 */
	inline static Tcs3472::Rgb
	getOldColors()
	{
		return color;
        };

	//!@}

	/**
	 * @name Sample and return fresh color values
	 * @{
	 */
	inline static Tcs3472::Rgb
	getNewColors()
	{
		refreshAllColors();
		return getOldColors();
	};

	//!@}

	//! \brief	Read current samples of ADC conversions for all channels.
	// Non-blocking
	modm::ResumableResult<bool>
	refreshAllColors();

	// MARK: - TASKS
	modm::ResumableResult<bool>
	initialize()
	{
		return writeRegister(RegisterAddress::ENABLE, 0b11);	// control to power up and start conversion
		// note: adafruits driver waits 3ms before writing AEN bit (0b10). we don't??
	};

	modm::ResumableResult<bool>
	configure(
                        const Gain	gain        = Gain::DEFAULT,
                        const uint8_t 	int_time    = IntegrationTime::DEFAULT);

private:
	//! \brief Sets the integration time for the ADCs.
	modm::ResumableResult<bool>
        setIntegrationTime(const uint8_t int_time = 0)
	{
		return writeRegister(
				RegisterAddress::TIMING,
                                static_cast<uint8_t>(int_time));
	}

private:
	uint8_t commandBuffer[4];
	bool success;

private:
	//! \brief	Read value of specific register.
	modm::ResumableResult<bool>
	readRegisters(
			const RegisterAddress address,
			uint8_t * const values,
			const uint8_t count = 1);

	modm::ResumableResult<bool>
	writeRegister(
			const RegisterAddress address,
			const uint8_t value);

private:
	class uint16_t_LOW_HIGH
	{
	private:
		uint8_t low;
		uint8_t high;
	public:
		uint16_t
		get() const
		{
			uint16_t value = low;
			value |= high << 8;
			return value;
		}
		inline uint8_t getLSB()	const { return low; }
		inline uint8_t getMSB()	const { return high; }
	} modm_packed;

	static union Data
	{
		uint8_t dataBytes[2*4];
		struct
		{
                        uint16_t_LOW_HIGH clear;
			uint16_t_LOW_HIGH red;
			uint16_t_LOW_HIGH green;
			uint16_t_LOW_HIGH blue;
		} modm_packed;
	} data;

	static Rgb	color;
};
}

#include "tcs3472_impl.hpp"

#endif // MODM_tcs3472_HPP
