/*
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2015-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/architecture/interface/interrupt.hpp>

#include "../integration_test.hpp"

/**
 * Tests that Timers 1 to 14 of a STM32F4 generate update interrupts with the
 * right period and that every interrupt arrives at the right handler.
 *
 * Every timer runs with a period of 50ms for 525ms: this must result in
 * exactly 10 interrupts.
 */

static volatile uint8_t interrupts[15];

template< typename TIMER >
static bool
testTimer(uint8_t number)
{
	interrupts[number] = 0;

	TIMER::enable();
	TIMER::setMode(TIMER::Mode::UpCounter);
	TIMER::template setPeriod<Board::SystemClock>(50ms);
	// advanced timers have several interrupt vectors
	if constexpr (requires { TIMER::enableInterruptVector(TIMER::Interrupt::Update, true, 10); }) {
		TIMER::enableInterruptVector(TIMER::Interrupt::Update, true, 10);
	} else {
		TIMER::enableInterruptVector(true, 10);
	}
	TIMER::enableInterrupt(TIMER::Interrupt::Update);
	TIMER::applyAndReset();
	// applying the settings generates an update event
	TIMER::acknowledgeInterruptFlags(TIMER::InterruptFlag::Update);
	TIMER::start();

	modm::delay(525ms);

	TIMER::pause();
	TIMER::disableInterrupt(TIMER::Interrupt::Update);
	if constexpr (requires { TIMER::enableInterruptVector(TIMER::Interrupt::Update, false, 10); }) {
		TIMER::enableInterruptVector(TIMER::Interrupt::Update, false, 10);
	} else {
		TIMER::enableInterruptVector(false, 10);
	}
	TIMER::disable();

	const uint8_t count = interrupts[number];
	const bool passed = (count == 10);
	MODM_LOG_INFO << "Timer" << number << ": " << count << " interrupts"
				  << (passed ? "" : ", expected 10!") << modm::endl;
	Board::Leds::toggle();
	return passed;
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	bool passed = true;

	passed &= testTimer<Timer1>(1);
	passed &= testTimer<Timer2>(2);
	passed &= testTimer<Timer3>(3);
	passed &= testTimer<Timer4>(4);
	passed &= testTimer<Timer5>(5);
	passed &= testTimer<Timer6>(6);
	passed &= testTimer<Timer7>(7);
	passed &= testTimer<Timer8>(8);
	passed &= testTimer<Timer9>(9);
	passed &= testTimer<Timer10>(10);
	passed &= testTimer<Timer11>(11);
	passed &= testTimer<Timer12>(12);
	passed &= testTimer<Timer13>(13);
	passed &= testTimer<Timer14>(14);

	return finishTest(passed);
}

// Some timers share their interrupt
MODM_ISR(TIM2)
{
	if (Timer2::getInterruptFlags() & Timer2::InterruptFlag::Update) {
		Timer2::acknowledgeInterruptFlags(Timer2::InterruptFlag::Update);
		interrupts[2]++;
	}
}

MODM_ISR(TIM3)
{
	if (Timer3::getInterruptFlags() & Timer3::InterruptFlag::Update) {
		Timer3::acknowledgeInterruptFlags(Timer3::InterruptFlag::Update);
		interrupts[3]++;
	}
}

MODM_ISR(TIM4)
{
	if (Timer4::getInterruptFlags() & Timer4::InterruptFlag::Update) {
		Timer4::acknowledgeInterruptFlags(Timer4::InterruptFlag::Update);
		interrupts[4]++;
	}
}

MODM_ISR(TIM5)
{
	if (Timer5::getInterruptFlags() & Timer5::InterruptFlag::Update) {
		Timer5::acknowledgeInterruptFlags(Timer5::InterruptFlag::Update);
		interrupts[5]++;
	}
}

MODM_ISR(TIM6_DAC)
{
	if (Timer6::getInterruptFlags() & Timer6::InterruptFlag::Update) {
		Timer6::acknowledgeInterruptFlags(Timer6::InterruptFlag::Update);
		interrupts[6]++;
	}
}

MODM_ISR(TIM7)
{
	if (Timer7::getInterruptFlags() & Timer7::InterruptFlag::Update) {
		Timer7::acknowledgeInterruptFlags(Timer7::InterruptFlag::Update);
		interrupts[7]++;
	}
}

MODM_ISR(TIM1_BRK_TIM9)
{
	if (Timer9::getInterruptFlags() & Timer9::InterruptFlag::Update) {
		Timer9::acknowledgeInterruptFlags(Timer9::InterruptFlag::Update);
		interrupts[9]++;
	}
}

MODM_ISR(TIM1_UP_TIM10)
{
	if (Timer1::getInterruptFlags() & Timer1::InterruptFlag::Update) {
		Timer1::acknowledgeInterruptFlags(Timer1::InterruptFlag::Update);
		interrupts[1]++;
	}
	if (Timer10::getInterruptFlags() & Timer10::InterruptFlag::Update) {
		Timer10::acknowledgeInterruptFlags(Timer10::InterruptFlag::Update);
		interrupts[10]++;
	}
}

MODM_ISR(TIM1_TRG_COM_TIM11)
{
	if (Timer11::getInterruptFlags() & Timer11::InterruptFlag::Update) {
		Timer11::acknowledgeInterruptFlags(Timer11::InterruptFlag::Update);
		interrupts[11]++;
	}
}

MODM_ISR(TIM8_BRK_TIM12)
{
	if (Timer12::getInterruptFlags() & Timer12::InterruptFlag::Update) {
		Timer12::acknowledgeInterruptFlags(Timer12::InterruptFlag::Update);
		interrupts[12]++;
	}
}

MODM_ISR(TIM8_UP_TIM13)
{
	if (Timer8::getInterruptFlags() & Timer8::InterruptFlag::Update) {
		Timer8::acknowledgeInterruptFlags(Timer8::InterruptFlag::Update);
		interrupts[8]++;
	}
	if (Timer13::getInterruptFlags() & Timer13::InterruptFlag::Update) {
		Timer13::acknowledgeInterruptFlags(Timer13::InterruptFlag::Update);
		interrupts[13]++;
	}
}

MODM_ISR(TIM8_TRG_COM_TIM14)
{
	if (Timer14::getInterruptFlags() & Timer14::InterruptFlag::Update) {
		Timer14::acknowledgeInterruptFlags(Timer14::InterruptFlag::Update);
		interrupts[14]++;
	}
}
