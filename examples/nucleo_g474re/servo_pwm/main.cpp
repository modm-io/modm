/*
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	MODM_LOG_INFO << "modm example: Servo PWM" << modm::endl << modm::endl;
	MODM_LOG_INFO << "Generates a 50 Hz PWM signal with a pulse width between 1 ms and 2 ms suitable" << modm::endl;
	MODM_LOG_INFO << "for many servo motors or RC electronic speed controllers on pin B7." << modm::endl;
	MODM_LOG_INFO << "1.5 ms pulse width is output when the user button is pressed, " << modm::endl;
	MODM_LOG_INFO << "otherwise 1 ms pulse width." << modm::endl << modm::endl;

	Timer4::connect<GpioB7::Ch2>();
	Timer4::enable();
	Timer4::setMode(Timer4::Mode::UpCounter);

	constexpr uint16_t overflow = 0xffff;
	constexpr uint16_t minPwm = (overflow / 20.f) * 1.f; // 1 ms pulse width
	constexpr uint16_t targetPwm = (overflow / 20.f) * 1.5f; // 1.5 ms pulse width
	constexpr uint16_t maxPwm = (overflow / 20.f) * 2.f; // 2 ms pulse width

	// 170 MHz / 52 / 2^16 = approx. 50 Hz
	Timer4::setPrescaler(52);
	Timer4::setOverflow(overflow);

	Timer4::configureOutputChannel<GpioB7::Ch2>(Timer4::OutputCompareMode::Pwm, 0);

	Timer4::applyAndReset();

	Timer4::start();

	MODM_LOG_INFO << "Teaching sequence for electronic speed controllers:" << modm::endl;
	MODM_LOG_INFO << "1.0 ms pulse width for 2 s" << modm::endl;
	MODM_LOG_INFO << "2.0 ms pulse width for 2 s" << modm::endl;
	MODM_LOG_INFO << "1.0 ms pulse width for 1 s" << modm::endl;
	LedD13::set();
	Timer4::configureOutputChannel<GpioB7::Ch2>(Timer4::OutputCompareMode::Pwm, minPwm);
	modm::delay(2000ms);
	Timer4::configureOutputChannel<GpioB7::Ch2>(Timer4::OutputCompareMode::Pwm, maxPwm);
	modm::delay(2000ms);
	Timer4::configureOutputChannel<GpioB7::Ch2>(Timer4::OutputCompareMode::Pwm, minPwm);
	LedD13::reset();
	modm::delay(1000ms);

	MODM_LOG_INFO << modm::endl;

	while (true)
	{
		LedD13::toggle();
		modm::delay(100ms);

		if (!Button::read()) {
			Timer4::setCompareValue<GpioB7::Ch2>(targetPwm);
			MODM_LOG_INFO << "^";
		}
		else {
			Timer4::setCompareValue<GpioB7::Ch2>(minPwm);
			MODM_LOG_INFO << "-";
		}
	}

	return 0;
}
