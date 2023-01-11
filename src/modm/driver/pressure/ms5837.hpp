// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2023, Vivien Henry
 * Based on the ms5837, courtesy of Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MS5837_HPP
#define MODM_MS5837_HPP

#include "ms5837_data.hpp"

#include <modm/processing/resumable.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/processing/timer.hpp>


namespace modm
{

/// @ingroup modm_driver_ms5837
struct ms5837
{

using Prom = modm::ms5837data::Prom;
using Data = modm::ms5837data::Data;
using DataBase = modm::ms5837data::DataBase;

protected:
	/// @cond
	enum class
	Command : uint8_t
	{
		Reset       = 0x1E,
		Convert     = 0x40,
		AdcRead     = 0x00,
		PromRead    = 0xA0,
	};

	enum class
	Conversion : uint8_t
	{
		Pressure = 0x00,
		Temperature = 0x10,
	};
	/// @endcond

public:
	enum class
	OversamplingRatio : uint8_t
	{
		Osr256  = 0x00,
		Osr512  = 0x02,
		Osr1024 = 0x04,
		Osr2048 = 0x06,
		Osr4096 = 0x08,
	};

protected:
	/// @cond
	static constexpr uint8_t
	i(Command cmd) { return uint8_t(cmd); }
	static constexpr uint8_t
	i(Conversion conv) { return uint8_t(conv); }
	static constexpr uint8_t
	i(OversamplingRatio osr) { return uint8_t(osr); }
	/// @endcond
};

/**
 * @tparam	I2cMaster	I2C interface
 *
 * @author	Vivien Henry
 * @ingroup modm_driver_ms5837
 */
template < typename I2cMaster >
class Ms5837 : public ms5837, public modm::I2cDevice<I2cMaster, 2>
{
public:
	/**
	 * @param	data pointer to buffer of the internal data of type Data
	 */
	Ms5837(DataBase &data, uint8_t address=0x76);

	/// Call this function before using the device to read the factory calibration
	/// @warning calls to this function resets the device
	modm::ResumableResult<bool>
	initialize();

	/// Do a readout sequence to convert and read temperature and then pressure from sensor
	modm::ResumableResult<bool>
	readout(OversamplingRatio osrPressure = OversamplingRatio::Osr256,
			OversamplingRatio osrTemperature = OversamplingRatio::Osr256);

public:
	/// Get the data object for this sensor
	inline DataBase&
	getData() { return data; }

private:
	/// Read the PROM register at the address
	modm::ResumableResult<uint16_t>
	readProm(uint8_t addr);

private:
	DataBase &data;
	modm::ShortTimeout timeout;

	/**
	 * Conversion time of the Analog Digital Convert for different oversampling ratios
	 * The conversion times are taken from the application note AN520
	 */
	static constexpr uint8_t conversionDelay[] = {1, 3, 4, 6, 10};

	/// Command buffer for writing to and reading from the device
	uint8_t buffer[2];

	uint8_t factory_crc;
};

} // modm namespace

#include "ms5837_impl.hpp"

#endif // MODM_MS5837_HPP
