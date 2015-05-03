// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_DS1631_HPP
#define XPCC_DS1631_HPP

#include <xpcc/architecture/interface/register.hpp>
#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/processing/protothread.hpp>
#include "lm75.hpp"

namespace xpcc
{

struct ds1631
{
protected:
	/// @cond
	enum class
	Command : uint8_t
	{
		Temperature = 0xAA,
		Configuration = 0xAC,
		TemperatureUpperLimit = 0xA1,
		TemperatureLowerLimit = 0xA2,

		/**
		 * Initiates temperature conversions. If the part is in one-shot mode (1SHOT = 1), only
		 * one conversion is performed. In continuous mode (1SHOT = 0), continuous temperature
		 * conversions are performed until a Stop Convert T command is issued.
		 */
		StartConvert = 0x51,
		/// Stops temperature conversions when the device is in continuous conversion mode (1SHOT = 0).
		StopConvert = 0x22,

		/**
		 * Initiates a software power-on-reset (POR), which stops temperature conversions and resets all
		 * registers and logic to their power-up states. The software POR allows the user to simulate
		 * cycling the power without actually powering down the device.
		 */
		SoftwareReset = 0x54
	};

	enum class
	Config : uint8_t
	{
		Done = Bit7,
		TemperatureHighFlag = Bit6,
		TemperatureLowFlag = Bit5,
		MemoryBusy = Bit4,
		// Resolution 3:2
		Polarity = Bit1,
		OneShot = Bit0
	};
	XPCC_FLAGS8(Config);
	/// @endcond

public:
	enum class
	ConversionMode : uint8_t
	{
		Continous = 0,
		OneShot = Config::OneShot
	};

	enum class
	AlertPolarity : uint8_t
	{
		ActiveLow = 0,
		ActiveHigh = Config::Polarity
	};

	enum class
	Resolution : uint8_t
	{
		Bits9 = 0,				///< Conversion Time:  94ms
		Bits10 = Bit0,			///< Conversion Time: 188ms
		Bits11 = Bit1,			///< Conversion Time: 375ms
		Bits12 = Bit1 | Bit0	///< Conversion Time: 750ms
	};
protected:
	/// @cond
	typedef Configuration< Config_t, Resolution, (Bit1 | Bit0), 2 > Resolution_t;
	/// @endcond

public:
	typedef lm75::Data Data;
};

/**
 * DS1631, DS1631A and DS1731 temperature sensor driver.
 *
 * @see	ds1631
 *
 * @ingroup	driver_temperature
 * @author	Fabian Greif
 * @author	Niklas Hauser
 */
template < typename I2cMaster >
class Ds1631 :	public ds1631, public I2cDevice< I2cMaster, 2 >,
				protected xpcc::pt::Protothread
{
public:
	///
	Ds1631(Data &data, uint8_t address=0x90);

	void ALWAYS_INLINE
	update()
	{ run(); }

	xpcc::ResumableResult<bool>
	initialize();

	// @param	rate	Update rate in Hz: 1 to 33.
	xpcc::ResumableResult<bool>
	setUpdateRate(uint8_t rate);

	xpcc::ResumableResult<bool>
	setResolution(Resolution resolution);

	xpcc::ResumableResult<bool>
	setAlertPolarity(AlertPolarity polarity);

	xpcc::ResumableResult<bool>
	setConversionMode(ConversionMode mode);

	/// Writes the upper limit of the alarm.
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	setUpperLimit(float temperature)
	{ return setLimitRegister(Command::TemperatureUpperLimit, temperature); }

	/// Writes the lower limit of the alarm.
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	setLowerLimit(float temperature)
	{ return setLimitRegister(Command::TemperatureLowerLimit, temperature); }


	/// reads the Temperature registers and buffers the results
	xpcc::ResumableResult<bool>
	readTemperature();


	xpcc::ResumableResult<bool> ALWAYS_INLINE
	startConversion()
	{ return writeCommand(Command::StartConvert); }

	xpcc::ResumableResult<bool> ALWAYS_INLINE
	stopConversion()
	{ return writeCommand(Command::StopConvert); }

	xpcc::ResumableResult<bool> ALWAYS_INLINE
	reset()
	{ return writeCommand(Command::SoftwareReset); }


	inline Data&
	getData()
	{ return data; }

private:
	bool
	run();

	xpcc::ResumableResult<bool>
	writeCommand(Command cmd);

	xpcc::ResumableResult<bool>
	writeConfiguration();

	xpcc::ResumableResult<bool>
	setLimitRegister(Command cmd, float temperature);

	Data &data;
	uint8_t buffer[3];
	Config_t config;

	xpcc::ShortTimeout periodTimeout;
	xpcc::ShortTimeout conversionTimeout;
	uint16_t updateTime;
	uint8_t conversionTime;
};

}	// namespace xpcc

#include "ds1631_impl.hpp"

#endif // XPCC_DS1631_HPP
