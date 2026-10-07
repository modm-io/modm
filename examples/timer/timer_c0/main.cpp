/*
 * Copyright (c) 2024, Jörg Ebeling
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

MODM_ISR(TIM14)
{
	Timer14::acknowledgeInterruptFlags(Timer14::InterruptFlag::Update);
	MODM_LOG_DEBUG << "Set LED" << modm::endl;
	Board::LedD13::set();
}

MODM_ISR(TIM16)
{
	Timer16::acknowledgeInterruptFlags(Timer16::InterruptFlag::Update);
	MODM_LOG_DEBUG << "Reset LED" << modm::endl;
	Board::LedD13::reset();
}

int
main()
{
	Board::initialize();
	Board::LedD13::setOutput();

	MODM_LOG_INFO << "Board & Logger initialized" << modm::endl;

	Timer14::enable();
	Timer14::setMode(Timer14::Mode::UpCounter);
	Timer14::setPeriod<Board::SystemClock>(1000ms);
	Timer14::applyAndReset();
	Timer14::enableInterrupt(Timer14::Interrupt::Update);
	Timer14::enableInterruptVector(true, 5);

	Timer16::enable();
	Timer16::setMode(Timer16::Mode::UpCounter);
	Timer16::setPeriod<Board::SystemClock>(909ms);
	Timer16::applyAndReset();
	Timer16::enableInterrupt(Timer16::Interrupt::Update);
	Timer16::enableInterruptVector(true, 5);

	Timer14::start();
	Timer16::start();

	while (true) ;
}
