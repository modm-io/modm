// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TMP175_HPP
#define XPCC_TMP175_HPP

#include <xpcc/architecture/interface/register.hpp>
#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/processing/protothread.hpp>
#include "lm75.hpp"

namespace xpcc
{

// forward declaration for friending with tmp175::Data
template < class I2cMaster >
class Tmp175;

struct tmp175 : public lm75
{
protected:
	/// @cond
	enum class
	Config1 : uint8_t
	{
		OneShot = Bit7,
		// Resolution 6:5
		// Fault Queue 4:3
		Polarity = Bit2,
		ThermostatMode = Bit1,
		ShutdownMode = Bit0,
	};
	XPCC_FLAGS8(Config1);
	/// @endcond

public:
	enum class
	Resolution : uint8_t
	{
		Bits9 = 0,				///< Conversion Time:  28ms
		Bits10 = Bit0,			///< Conversion Time:  55ms
		Bits11 = Bit1,			///< Conversion Time: 110ms
		Bits12 = Bit1 | Bit0	///< Conversion Time: 220ms
	};
protected:
	/// @cond
	typedef Configuration< Config1_t, Resolution, (Bit1 | Bit0), 5 > Resolution_t;
	/// @endcond
};

/**
 * TMP175 digital temperature sensor driver.
 *
 * The TMP175 is a digital temperature sensor with a two-wire interface
 * and measures temperature over a range of -40 to +125 deg Celsius with a
 * resolution of 1/16 (0.0625) deg C and an accuracy of up to 1.5 deg C.
 *
 * The sensor has a default refresh rate of 4Hz but can be raised up to
 * 30Hz by repeatedly manually starting a conversion (with startConversion()),
 * which lasts between 30ms and 240ms depending on resolution.
 *
 * @ingroup driver_temperature
 * @author	Niklas Hauser
 *
 * @tparam I2cMaster Asynchronous Interface
 */
template < typename I2cMaster >
class Tmp175 :	public tmp175, public Lm75< I2cMaster >,
				protected xpcc::pt::Protothread
{
public:
	/// Constructor, requires a tmp175::Data object,
	/// sets address to default of 0x48 (alternatives are 0x49, 0x4A and 0x4B).
	Tmp175(Data &data, uint8_t address=0x48);

	void ALWAYS_INLINE
	update()
	{ run(); }

	// @param	rate	Update rate in Hz: 1 to 33.
	void
	setUpdateRate(uint8_t rate);

	xpcc::co::ResumableResult<bool>
	setResolution(Resolution resolution);

	/// Writes the upper limit of the alarm.
	xpcc::co::ResumableResult<bool> ALWAYS_INLINE
	setUpperLimit(float temperature)
	{ return setLimitRegister(Register::TemperatureUpperLimit, temperature); }

	/// Writes the lower limit of the alarm.
	xpcc::co::ResumableResult<bool> ALWAYS_INLINE
	setLowerLimit(float temperature)
	{ return setLimitRegister(Register::TemperatureLowerLimit, temperature); }

	/// starts a temperature conversion right now
	xpcc::co::ResumableResult<bool>
	startConversion();

	inline Data&
	getData();

private:
	bool
	run();

	xpcc::co::ResumableResult<bool>
	writeConfiguration();

	xpcc::co::ResumableResult<bool>
	setLimitRegister(Register reg, float temperature);

	xpcc::ShortTimeout periodTimeout;
	xpcc::ShortTimeout conversionTimeout;
	uint16_t updateTime;
	uint8_t conversionTime;
};

} // namespace xpcc

#include "tmp175_impl.hpp"

#endif // XPCC_TMP175_HPP
