// coding: utf-8
/* Copyright (c) 2017, Raphael Lehmann
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>

#include <xpcc/processing.hpp>
#include <xpcc/driver/pressure/amsys5915.hpp>
#include <xpcc/io/iostream.hpp>
#include <xpcc/debug/logger.hpp>

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > device;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(device);
xpcc::log::Logger xpcc::log::info(device);
xpcc::log::Logger xpcc::log::warning(device);
xpcc::log::Logger xpcc::log::error(device);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

/**
 * Example to demonstrate a XPCC driver for pressure sensor AMSYS 5915
 *
 * This example uses I2cMaster2 of STM32F407
 *
 * SDA		PB11
 * SCL		PB10
 *
 * GND and +3V are connected to the sensor.
 */

typedef I2cMaster2 MyI2cMaster;
// typedef xpcc::SoftwareI2cMaster<GpioB10, GpioB11> MyI2cMaster;

xpcc::amsys5915::Data data;
xpcc::Amsys5915<MyI2cMaster> pressureSensor(data);

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
			// we wait until the task started
			if (PT_CALL(pressureSensor.ping())) {
			 	break;
			}
			// otherwise, try again in 10ms
			this->timeout.restart(10);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		XPCC_LOG_DEBUG << "Device responded" << xpcc::endl;

		while (true)
		{
			while (! PT_CALL(pressureSensor.readPressure()))
			{
				PT_YIELD();
			}
			
			XPCC_LOG_INFO << "Pressure [0..1]: " << data.getPressure() << xpcc::endl;
			XPCC_LOG_INFO << "Temperature [degree centigrade]: " << data.getTemperature() << xpcc::endl;
			
			// read next pressure measurement in 1ms
			this->timeout.restart(1);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}
private:
	xpcc::ShortTimeout timeout;
};

ThreadOne one;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<Board::systemClock, xpcc::Uart::B115200>(10);

	GpioB11::connect(I2cMaster2::Sda);
	GpioB10::connect(I2cMaster2::Scl);

	MyI2cMaster::initialize<Board::systemClock, 400000>();

	XPCC_LOG_INFO << "\n\nWelcome to AMSYS 5915 pressure sensor demo!\n\n";

	xpcc::ShortPeriodicTimer tmr(500);

	while (1)
	{
		one.update();
		if (tmr.execute()) {
			Board::LedOrange::toggle();
		}
	}

	return 0;
}
