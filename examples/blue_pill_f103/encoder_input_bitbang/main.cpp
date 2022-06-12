/*
 * Copyright (c) 2021, Thomas Sommer
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
#include <modm/processing/timer.hpp>
#include <modm/driver/encoder/bitbang_encoder_input.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

// Connect the encoders outputs to D7 and D8 Pins (usually the outer pins)
// The common third pin (usually in the middle) is connected to GND.
modm::BitBangEncoderInput<GpioInputB6, GpioInputB7, 4, int16_t> encoder;

MODM_ISR(TIM2)
{
	Timer2::acknowledgeInterruptFlags(Timer2::InterruptFlag::Update);
	encoder.update();
}

template<class Timer, class Rep, class Period>
void
initPeriodicInterrupt(std::chrono::duration<Rep, Period> period)
{
	Timer::enable();
	Timer::setMode(Timer::Mode::UpCounter);

	Timer::template setPeriod<Board::SystemClock>(period);
	Timer::enableInterruptVector(true, 10);
	Timer::enableInterrupt(Timer::Interrupt::Update);

	Timer::applyAndReset();
	Timer::start();
}

int
main()
{
	Board::initialize();

	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::SystemClock, 115200_Bd>();

	encoder.initialize();
	initPeriodicInterrupt<Timer2>(1ms);

	int value(0);

	modm::ShortPeriodicTimer heartbeat(1s);

	while (true)
	{
		if (heartbeat.execute()) {
			Board::LedGreen::toggle();

			const auto delta = encoder.getDelta();
			MODM_LOG_INFO << "Delta: " << delta << modm::endl;

			value += delta;
			MODM_LOG_INFO << "Encoder Absolut: " << value << modm::endl;
		}
	}
}
