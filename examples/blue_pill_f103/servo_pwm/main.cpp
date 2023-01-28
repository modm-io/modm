/*
 * Copyright (c) 2022, Raphael Lehmann
 * Copyright (c) 2022, Steven Macías
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/debug.hpp>

using namespace Board;

Rtt rtt(0);
modm::IODeviceObjectWrapper< Rtt, modm::IOBuffer::DiscardIfFull > rtt_device(rtt);
// Set all four logger streams to use RTT
modm::log::Logger modm::log::debug(rtt_device);
modm::log::Logger modm::log::info(rtt_device);
modm::log::Logger modm::log::warning(rtt_device);
modm::log::Logger modm::log::error(rtt_device);

#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO


int main()
{
	Board::initialize();
	LedGreen::setOutput();

	MODM_LOG_INFO << modm::endl;
	MODM_LOG_INFO << "modm example: Servo PWM" << modm::endl << modm::endl;
	MODM_LOG_INFO << "Generates a 50 Hz PWM signal with a pulse width between 1 ms and 2 ms" << modm::endl;
	MODM_LOG_INFO << "for many servo motors or RC electronic speed controllers on pin B7." << modm::endl;

	GpioB7::setOutput(); // Needed on STM32 F1 Family. Check https://github.com/modm-io/modm/discussions/891
	Timer4::connect<GpioB7::Ch2>();
	Timer4::enable();
	Timer4::setMode(Timer4::Mode::UpCounter);

	constexpr uint16_t overflow = 0xffff;
	constexpr uint16_t minPwm = (overflow / 20.f) * 1.f; // 1 ms pulse width
	constexpr uint16_t targetPwm = (overflow / 20.f) * 1.5f; // 1.5 ms pulse width
	constexpr uint16_t maxPwm = (overflow / 20.f) * 2.f; // 2 ms pulse width

	// 72 MHz / 22 / 2^16 = approx. 50 Hz
	Timer4::setPrescaler(22);
	Timer4::setOverflow(overflow);

	Timer4::configureOutputChannel<GpioB7::Ch2>(Timer4::OutputCompareMode::Pwm, 0);

	Timer4::applyAndReset();

	Timer4::start();

	MODM_LOG_INFO << modm::endl;
	MODM_LOG_INFO << "Teaching sequence for electronic speed controllers:" << modm::endl;
	MODM_LOG_INFO << "1.0 ms pulse width for 2 s" << modm::endl;
	MODM_LOG_INFO << "2.0 ms pulse width for 2 s" << modm::endl;
	MODM_LOG_INFO << "1.0 ms pulse width for 1 s" << modm::endl;

	LedGreen::set();
	Timer4::configureOutputChannel<GpioB7::Ch2>(Timer4::OutputCompareMode::Pwm, minPwm);
	modm::delay(2s);
	Timer4::configureOutputChannel<GpioB7::Ch2>(Timer4::OutputCompareMode::Pwm, maxPwm);
	modm::delay(2s);
	Timer4::configureOutputChannel<GpioB7::Ch2>(Timer4::OutputCompareMode::Pwm, minPwm);
	LedGreen::reset();
	modm::delay(1s);

	MODM_LOG_INFO << "Teaching sequence completed." << modm::endl;
	MODM_LOG_INFO << modm::endl;

	MODM_LOG_INFO << "Pulse width set to 1.5 ms" << modm::endl;
	Timer4::setCompareValue<GpioB7::Ch2>(targetPwm);

	while (true)
	{
		LedGreen::toggle();
		modm::delay(100ms);
	}

	return 0;
}
