/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BME280_HPP
#define XPCC_BME280_HPP

#include <modm/processing/protothread.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/timer.hpp>

#include "bme280_data.hpp"

namespace xpcc
{

template < typename I2cMaster >
class Bme280;

struct bme280
{

using Calibration = xpcc::bme280data::Calibration;
using Data = xpcc::bme280data::Data;
using DataBase = xpcc::bme280data::DataBase;
using DataDouble = xpcc::bme280data::DataDouble;

protected:
	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		HUM_LSB    = 0xFE,
		HUM_MSB    = 0xFD,
		TEMP_XLSB  = 0xFC,
		TEMP_LSB   = 0xFB,
		TEMP_MSB   = 0xFA,
		PRESS_XLSB = 0xF9,
		PRESS_LSB  = 0xF8,
		PRESS_MSB  = 0xF7,

		CONFIG     = 0xF5,
		CTRL_MEAS  = 0xF4,
		STATUS     = 0xF3,
		CTRL_HUM   = 0xF2,

		// up to     0xF0
		CALIB26    = 0xE1,

		RESET      = 0xE0,

		CHIP_ID    = 0xD0,

		// up to     0xA1
		CALIB00    = 0x88,
	};

	/// The ChipId that is read back for a BME280
	static constexpr uint8_t ChipId = 0x60;

	/// The value written to the reset register that resets the device
	static constexpr uint8_t ResetValue = 0xb6;

	/// The options of REGISTER_CONTROL
	enum class
	Conversion : uint8_t
	{
		Temperature = 0x2E,
		Pressure = 0x34,
	};
	/// @endcond

public:
	// Register 0xf2 "ctrl_hum"
	enum class
	CtrlHum : uint8_t
	{
		OSRS_H2 = Bit2,
		OSRS_H1 = Bit1,
		OSRS_H0 = Bit0,
	};
	XPCC_FLAGS8(CtrlHum);

	// Register 0xF3 "status"
	enum class
	Status : uint8_t
	{
		MEASURING = Bit3,	// Automatically set to ‘1’ whenever a conversion is running and back to ‘0’ when the results have been transferred to the data registers.
		IM_UPDATE = Bit0,	// Automatically set to ‘1’ when the NVM data are being copied to image registers and back to ‘0’ when the copying is done. The data are copied at power-on-reset and before every conversion.
	};
	XPCC_FLAGS8(Status);

	// Register 0xF4 "ctrl_meas"
	enum class
	CtrlMeas : uint8_t
	{
		OSRS_T2 = Bit7,
		OSRS_T1 = Bit6,
		OSRS_T0 = Bit5,
		OSRS_P2 = Bit4,
		OSRS_P1 = Bit3,
		OSRS_P0 = Bit2,
		Mode1   = Bit1,
		Mode0   = Bit0,
	};
	XPCC_FLAGS8(CtrlMeas);

	enum class
	Mode : uint8_t
	{
		Sleep = 0,
		Forced = int(CtrlMeas::Mode0),
		Normal = int(CtrlMeas::Mode1) | int(CtrlMeas::Mode0),
	};
	typedef Configuration< CtrlMeas_t, Mode, (Bit1 | Bit0) >  Mode_t;

	// [4:2]
	enum class
	Oversampling : uint8_t
	{
		Skipped    = (0b000),
		Single     = (0b001),
		Double     = (0b010),
		Quadrupel  = (0b011),
		Octupel    = (0b100),
		Sexdecuple = (0b101),
	};
	typedef Configuration< CtrlMeas_t, Oversampling, (Bit2 | Bit1 | Bit0), 2 > Pressure;
	typedef Configuration< CtrlMeas_t, Oversampling, (Bit2 | Bit1 | Bit0), 5 > Temperature;
	typedef Configuration< CtrlHum_t,  Oversampling, (Bit2 | Bit1 | Bit0), 0 > Humidity;

	// Register 0xF5 "config"
	enum class
	Config : uint8_t
	{
		T_SB_2 = Bit7,
		T_SB_1 = Bit6,
		T_SB_0 = Bit5,
		FILTER_2 = Bit4,
		FILTER_1 = Bit3,
		FILTER_0 = Bit2,
		SPI3W_EN = Bit0,
	};
	XPCC_FLAGS8(Config);

	enum class
	TimeStandby : uint8_t
	{
		Us500 = 0,
		Ms62  =                                             int(Config::T_SB_0),
		Ms125 =                       int(Config::T_SB_1),
		Ms250 =                       int(Config::T_SB_1) | int(Config::T_SB_0),
		Ms500 = int(Config::T_SB_2),
		S1    = int(Config::T_SB_2)                       | int(Config::T_SB_0),
		Ms10  = int(Config::T_SB_2) | int(Config::T_SB_1),
		Ms20  = int(Config::T_SB_2) | int(Config::T_SB_1) | int(Config::T_SB_0),
	};
	typedef Configuration< Config_t, TimeStandby, (Bit7 | Bit6 | Bit5) >  TimeStandby_t;

	enum class
	FilterSettings : uint8_t
	{
		Off = 0,
		F2  =                                                 int(Config::FILTER_0),
		F4  =                         int(Config::FILTER_1),
		F8  =                         int(Config::FILTER_1) | int(Config::FILTER_0),
		F16 = int(Config::FILTER_2) | int(Config::FILTER_1) | int(Config::FILTER_0),
	};

protected:
	/// @cond
	static constexpr uint8_t
	i(Mode mode) { return uint8_t(mode); }

	static constexpr uint8_t
	i(Conversion conv) { return uint8_t(conv); }

	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	/// @endcond
};

/**
 * BME280 digital absolute pressure sensor driver
 *
 * The BME280 is a high precision digital temperature, pressure and humidity sensor with I2C interface.
 *
 * It is known that many BME280-modules suffer from self-heating (I2C pull-ups?). 
 * So the reported temperature is about 1 to 3 degree higher than with other sensors.
 * Forced ventilation can drop the temperature by 2 to 3 degrees.
 *
 * @author  Sascha Schade
 * @ingroup driver_pressure
 *
 * @tparam I2cMaster I2C interface
 */
template < typename I2cMaster >
class Bme280 : public bme280, public xpcc::I2cDevice<I2cMaster, 1>
{
public:
	/**
	 * @param	data		pointer to buffer of the internal data of type Data
	 * @param	address		address defaults to 0x76
	 */
	Bme280(Data &data, uint8_t address=0x76);

	// MARK: - TASKS
	/// Reads out and stores the calibration bytes
	xpcc::ResumableResult<bool>
	initialize(
			Mode mode = Mode::Normal, 
			Oversampling temperature = Oversampling::Single,
			Oversampling pressure = Oversampling::Single,
			Oversampling humidity = Oversampling::Single);

	/// Read the raw data from the sensor. Conversion must be freerunning.
	xpcc::ResumableResult<bool>
	readout();

public:
	/// Get the data object for this sensor.
	inline Data&
	getData()
	{ return data; }

private:
	Data &data;

	// Command buffer for writing to the device
	uint8_t buffer[2];

	// Buffer for reading the H2-H6 calibration data and unfiddle the bits.
	uint8_t calBuffer[7];
};

}	// namespace xpcc

#include "bme280_impl.hpp"

#endif // XPCC_BME280_HPP
