// coding: utf-8
/* Copyright (c) 2017, Christopher Durand
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>

#include <xpcc/processing.hpp>
#include <xpcc/driver/position/vl53l0.hpp>
#include <xpcc/debug/logger.hpp>

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

/**
 * Example to demonstrate a XPCC driver for distance sensor VL53L0X
 *
 * This example uses I2cMaster1 of STM32F401
 *
 * SDA		PB9
 * SCL		PB8
 *
 * GND and +3V are connected to the sensor.
 */

using namespace Board;

using MyI2cMaster = I2cMaster1;

xpcc::vl53l0::Data data;
xpcc::Vl53l0<MyI2cMaster> distance(data);

// in microseconds
// useful value range: from 20000us = 20ms to 2000000us = 2s
constexpr uint32_t MeasurementTimeFast      = 30000;
constexpr uint32_t MeasurementTimePrecision = 200000;

class ThreadOne : public xpcc::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		XPCC_LOG_DEBUG << "Ping the device from ThreadOne" << xpcc::endl;

		// ping the device until it responds
		while (true)
		{
			// we wait until the device started
			if (PT_CALL(distance.ping())) {
				break;
			}

			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		XPCC_LOG_DEBUG << "Device responded" << xpcc::endl;

		while (true)
		{
			if (PT_CALL(distance.initialize())) {
				break;
			}

			// otherwise, try again in 200ms
			this->timeout.restart(200);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		XPCC_LOG_DEBUG << "Device initialized" << xpcc::endl;

		// set measurement time to 200ms (high accuracy mode)
		if(not PT_CALL(distance.setMaxMeasurementTime(MeasurementTimePrecision))) {
			XPCC_LOG_DEBUG << "Setting measurement time failed" << xpcc::endl;
		} else {
			this->highAccuracyMode = true;
		}

		this->timeout.restart(1000);

		while (true)
		{
			stamp = xpcc::Clock::now();

			if (PT_CALL(distance.readDistance()))
			{
				xpcc::vl53l0::RangeErrorCode error = distance.getRangeError();
				if (distance.getData().isValid())
				{
					uint16_t mm = distance.getData().getDistance();
					XPCC_LOG_DEBUG << "mm: " << mm;
				}
				else
				{
					XPCC_LOG_DEBUG << "Error: " << static_cast<uint8_t>(error);
				}
			}
			XPCC_LOG_DEBUG << ", t = " << (xpcc::Clock::now() - stamp) << "ms" << xpcc::endl;

			// query button state every 1000ms
			if(this->timeout.isExpired())
			{
				// toggle between fast and high accuracy mode when button is pressed
				if(Button::read())
				{
					if(this->highAccuracyMode)
					{
						if(PT_CALL(distance.setMaxMeasurementTime(MeasurementTimeFast))) {
							XPCC_LOG_DEBUG << "Enable fast mode" << xpcc::endl;
							this->highAccuracyMode = false;
						} else {
							XPCC_LOG_DEBUG << "Setting measurement time failed" << xpcc::endl;
						}
					}
					else
					{
						if(PT_CALL(distance.setMaxMeasurementTime(MeasurementTimePrecision))) {
							XPCC_LOG_DEBUG << "Enable high accuracy mode" << xpcc::endl;
							this->highAccuracyMode = true;
						} else {
							XPCC_LOG_DEBUG << "Setting measurement time failed" << xpcc::endl;
						}
					}
				}

				this->timeout.restart(1000);
			}
		}

		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
	xpcc::Timestamp stamp;
	bool highAccuracyMode = true;
};

ThreadOne one;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedD13::setOutput();

	GpioB8::connect(MyI2cMaster::Scl);
	GpioB9::connect(MyI2cMaster::Sda);
	MyI2cMaster::initialize<systemClock, 400000>();

	XPCC_LOG_INFO << "\n\nWelcome to VL53L0X demo!\n\n";

	xpcc::ShortPeriodicTimer tmr(500);

	while (1)
	{
		one.update();
		if(tmr.execute()) {
			LedD13::toggle();
		}

	}

	return 0;
}
