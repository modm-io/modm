/*
 * Copyright (c) 2024, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing/fiber.hpp>
#include <modm/driver/motion/adns9800.hpp>
#include <modm/math/geometry/vector2.hpp>

// Set the log level
#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DEBUG

using Usart2 = BufferedUart<UsartHal2, UartTxBuffer<256>>;
// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper<Usart2, modm::IOBuffer::BlockIfFull> loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

using Cs = GpioOutputA4;

modm::Fiber<> adns9800_fiber([]() {
	modm::Adns9800<SpiMaster1, Cs> adns9800;
	modm::Vector2i position;

	Cs::setOutput(modm::Gpio::High);

	SpiMaster1::connect<GpioOutputA5::Sck, GpioInputA6::Miso, GpioOutputA7::Mosi>();
	SpiMaster1::initialize<Board::SystemClock, 2.25_MHz>();
	SpiMaster1::setDataMode(SpiMaster1::DataMode::Mode3);

	if(not adns9800.initialize()) {
		MODM_LOG_INFO << "Failed to initialize ADNS9800" << modm::endl;
		return;
	}

	adns9800.set(modm::adns9800::Resolution<8200>{});
  adns9800.set(modm::adns9800::ShutterConfig{
    period_min:   10000,
    period_max:   40000,
    exposure_max: 50000
  });

	while (true)
	{
		const auto data {adns9800.read<modm::adns9800::Data_FailFlags_Monitoring>()};
		position += data.delta;

		MODM_LOG_INFO << "delta: " << data.delta << modm::endl;
		MODM_LOG_INFO << "position: " << position << modm::endl;
		MODM_LOG_INFO << modm::endl;

		modm::this_fiber::sleep_for(100ms);
	}
});

int
main()
{
	Board::initialize();

	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::SystemClock, 115200_Bd>();

	modm::fiber::Scheduler::run();
	return 0;
}
