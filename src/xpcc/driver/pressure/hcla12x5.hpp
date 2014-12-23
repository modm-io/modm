// coding: utf-8
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HCLAX_HPP
#define XPCC_HCLAX_HPP

#include <xpcc/architecture/peripheral/i2c_device.hpp>
#include <xpcc/processing/coroutine.hpp>
#include <xpcc/math/utils/endianess.hpp>

namespace xpcc
{

template < typename I2cMaster >
class HclaX;

struct hclax
{
	struct __attribute__ ((packed))
	Data
	{
		template < typename I2cMaster >
		friend class HclaX;

	public:
		/**
		 * This method returns the pressure as a normalized float from 0-1.
		 * You have to scale and offset this according to the specific sensor
		 * you have.
		 * So if you have HCLA12X5U, you can measure +12.5mBar, you need to
		 * multiply it with 12.5f to get the pressure in mBar.
		 * If you have HCLA12X5B, which can measure ±12.5mBar you first need to
		 * subtract 0.5f and then multiply it with 12.5f!
 		 */
		float
		getPressure()
		{
			// Full scale span is 0x6666, with offset 0x0666
			uint16_t *rData = reinterpret_cast<uint16_t*>(data);
			uint16_t pressure = xpcc::math::fromBigEndian(*rData) - 0x0666;
			return float(pressure) / 0x6666;
		}

	private:
		// 0: MSB
		// 1: LSB
		uint8_t data[2];
	};
};

/**
 * Driver for the HCLA differential pressure sensors.
 *
 * The device runs a cyclic program, which will store a corrected pressure value with
 * 12 bit resolution about every 250 μs within the output registers of the internal ASIC.
 *
 * @ingroup pressure
 * @author	Niklas Hauser
 */
template < typename I2cMaster >
class HclaX : public hclax, public xpcc::I2cDevice<I2cMaster>,
			  protected xpcc::co::Coroutine
{
public:
	/**
	 * @param	data	a hclax::Data object
	 * @bug The address of the sensor is by factory default set to 0x78.
	 *      This means you cannot use two HCLA sensors on the same bus!
	 *      You have to use a MUX or two seperate I2C busses.
	 */
	HclaX(Data &data)
	:	data(data), i2cTask(I2cTask::Idle), i2cSuccess(0),
		adapter(0x78, i2cTask, i2cSuccess)
	{
	}

	/// pings the sensor
	xpcc::co::Result<bool>
	ping(void *ctx)
	{
		CO_BEGIN(ctx);

		CO_WAIT_UNTIL(adapter.configurePing() and
				(i2cTask = I2cTask::Ping, this->startTransaction(&adapter)));

		CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

		CO_END_RETURN(i2cSuccess == I2cTask::Ping);
	}

	/// reads the Pressure registers and buffers the results
	xpcc::co::Result<bool>
	readPressure(void *ctx)
	{
		CO_BEGIN(ctx);

		CO_WAIT_UNTIL(adapter.configureRead(data.data, 2) and
				(i2cTask = I2cTask::ReadPressure, this->startTransaction(&adapter)));

		CO_WAIT_WHILE(i2cTask == I2cTask::ReadPressure);

		CO_END_RETURN(i2cSuccess == I2cTask::ReadPressure);
	}

public:
	Data &data;

private:
	enum
	I2cTask : uint8_t
	{
		Idle = 0,
		Ping,
		ReadPressure
	};

	volatile uint8_t i2cTask;
	volatile uint8_t i2cSuccess;
	xpcc::I2cTagAdapter< xpcc::I2cReadAdapter > adapter;
};

}	// namespace xpcc

#endif // XPCC_HCLAX_HPP
