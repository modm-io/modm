/*
 * Copyright (c) 2018, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <modm/board.hpp>
#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/i2c_multiplexer.hpp>
#include <modm/driver/gpio/pca9548a.hpp>

using MyI2cMaster = modm::platform::I2cMaster1;
using Mpx = modm::Pca9548a<MyI2cMaster>;
using I2cMultiplexer = modm::I2cMultiplexer<MyI2cMaster, Mpx>;


#ifndef MODM_BOARD_HAS_LOGGER
#include <modm/debug.hpp>
// Add logger manually
using LoggerUsart = Usart2;
using LoggerUsartTx = modm::platform::GpioA2;
using LoggerUsartRx = modm::platform::GpioA3;
modm::IODeviceWrapper< LoggerUsart, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);
#endif

#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG


namespace multiplexer
{
	I2cMultiplexer i2cMultiplexer;

	// Instances for each channel
	using Ch0 = I2cMultiplexer::Ch0< i2cMultiplexer >;
	using Ch1 = I2cMultiplexer::Ch1< i2cMultiplexer >;
	using Ch2 = I2cMultiplexer::Ch2< i2cMultiplexer >;
	using Ch3 = I2cMultiplexer::Ch3< i2cMultiplexer >;
	using Ch4 = I2cMultiplexer::Ch4< i2cMultiplexer >;
	using Ch5 = I2cMultiplexer::Ch5< i2cMultiplexer >;
	using Ch6 = I2cMultiplexer::Ch6< i2cMultiplexer >;
	using Ch7 = I2cMultiplexer::Ch7< i2cMultiplexer >;
}


class DeviceThread: public modm::pt::Protothread
{
public:
	DeviceThread() : dev0(0x29), dev1(0x29), dev2(0x29), dev3(0x29)
	{}

	bool
	update();

private:
	modm::ShortTimeout timeout;

	// Simple devices which are just pingable.
	// Independent of real device. Any I2C device should be pingable at its address.
	modm::I2cDevice<multiplexer::Ch1> dev0;
	modm::I2cDevice<multiplexer::Ch2> dev1;
	modm::I2cDevice<multiplexer::Ch3> dev2;
	modm::I2cDevice<multiplexer::Ch7> dev3;
};

bool
DeviceThread::update()
{
	PT_BEGIN();

	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "Ping the Devices" << modm::endl;

	// ping the devices repeatedly
	while(true)
	{
		MODM_LOG_DEBUG.printf("[dev  ] ping0\n");
		MODM_LOG_DEBUG.printf("[dev  ] ping0 res: %d\n", PT_CALL(dev0.ping()));
		MODM_LOG_DEBUG.printf("[dev  ] ping1\n");
		MODM_LOG_DEBUG.printf("[dev  ] ping1 res: %d\n", PT_CALL(dev1.ping()));
		MODM_LOG_DEBUG.printf("[dev  ] ping2\n");
		MODM_LOG_DEBUG.printf("[dev  ] ping2 res: %d\n", PT_CALL(dev2.ping()));
		MODM_LOG_DEBUG.printf("[dev  ] ping3\n");
		MODM_LOG_DEBUG.printf("[dev  ] ping3 res: %d\n", PT_CALL(dev3.ping()));
		// Do again in 1s
		timeout.restart(1s);
		PT_WAIT_UNTIL(timeout.isExpired());
	}

	PT_END();
}

int
main()
{
	Board::initialize();

#ifndef MODM_BOARD_HAS_LOGGER
	LoggerUsart::connect<LoggerUsartTx::Tx, LoggerUsartRx::Rx>();
	LoggerUsart::initialize<Board::SystemClock, 115200_Bd>();
#endif

	modm::platform::I2cMaster1::connect<modm::platform::GpioB7::Sda, modm::platform::GpioB6::Scl>();
	modm::platform::I2cMaster1::initialize<Board::SystemClock, 100_kHz>();

	constexpr uint32_t rate = 1; // Hz
	constexpr float interval = 1000.0 / rate; // msec
	modm::ShortPeriodicTimer heartbeat(interval);

	// Main loop
	DeviceThread deviceThread;

	uint32_t counter(0);
	while (true)
	{
		deviceThread.update();

		if (heartbeat.execute()) {
			Board::Leds::toggle();
			MODM_LOG_INFO << "Loop counter: " << (counter++) << modm::endl;
		}
	}

	return 0;
}
