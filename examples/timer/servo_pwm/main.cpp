/*
 * Copyright (c) 2021-2022, Raphael Lehmann
 * Copyright (c) 2022, Steven Macías
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

using namespace Board;

int
main()
{
	Board::initialize();
	Leds::setOutput();

	MODM_LOG_INFO << "modm example: Servo PWM" << modm::endl << modm::endl;
	MODM_LOG_INFO << "Generates a 50 Hz PWM signal with a pulse width between 1 ms and 2 ms" << modm::endl;
	MODM_LOG_INFO << "for many servo motors or RC electronic speed controllers on pin B7." << modm::endl;
	MODM_LOG_INFO << "1.0 ms pulse width is output while the user button is pressed," << modm::endl;
	MODM_LOG_INFO << "otherwise 1.5 ms pulse width." << modm::endl << modm::endl;

#ifdef STM32F1
	// Needed on STM32 F1 Family. Check https://github.com/modm-io/modm/discussions/891
	GpioB7::setOutput();
#endif
	Timer4::connect<GpioB7::Ch2>();
	Timer4::enable();
	Timer4::setMode(Timer4::Mode::UpCounter);

	// 20 ms period = 50 Hz
	const uint16_t overflow = Timer4::setPeriod<Board::SystemClock>(20ms);
	const uint16_t minPwm = overflow / 20.f * 1.f; // 1 ms pulse width
	const uint16_t targetPwm = overflow / 20.f * 1.5f; // 1.5 ms pulse width
	const uint16_t maxPwm = overflow / 20.f * 2.f; // 2 ms pulse width

	Timer4::configureOutputChannel<GpioB7::Ch2>(Timer4::OutputCompareMode::Pwm, 0);
	Timer4::applyAndReset();
	Timer4::start();

	MODM_LOG_INFO << "Teaching sequence for electronic speed controllers:" << modm::endl;
	MODM_LOG_INFO << "1.0 ms pulse width for 2 s" << modm::endl;
	MODM_LOG_INFO << "2.0 ms pulse width for 2 s" << modm::endl;
	MODM_LOG_INFO << "1.0 ms pulse width for 1 s" << modm::endl;

	Leds::set();
	Timer4::setCompareValue<GpioB7::Ch2>(minPwm);
	modm::delay(2s);
	Timer4::setCompareValue<GpioB7::Ch2>(maxPwm);
	modm::delay(2s);
	Timer4::setCompareValue<GpioB7::Ch2>(minPwm);
	Leds::reset();
	modm::delay(1s);

	MODM_LOG_INFO << "Teaching sequence completed." << modm::endl << modm::endl;

	while (true)
	{
		Leds::toggle();
		modm::delay(100ms);
		Timer4::setCompareValue<GpioB7::Ch2>(Button::read() ? minPwm : targetPwm);
	}

	return 0;
}
