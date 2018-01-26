/*
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2015-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/architecture/interface/interrupt.hpp>

/**
 * This is some testing of Timers 1 to 14 on a STM32 F4 Discovery Board.
 *
 * The blue LED is blinked in the main loop with busy waiting.
 *
 * Red and green LEDs are blinked by interrupts from timers.
 * The red LED should blink three times, driven by Timer 1,
 * then the green one blinks three times driven by Timer 2,
 * and then the red one again by Timer 3, et cetera.
 *
 * After 14 * 3 blinks each timer TIM1 to TIM14 blinked an LED
 * from an interrupt.
 *
 * When the blue LED stops blinking, probably the processor was
 * caught in the default handler for unassigned interrupts.
 * This should not happen.
 */

// Advanced Timers
template< typename TIMER >
static void
testTimerAdvancedStart()
{
	TIMER::enable();
	TIMER::setMode(TIMER::Mode::UpCounter);

	TIMER::template setPeriod<Board::systemClock>(250000);
	TIMER::enableInterruptVector(TIMER::Interrupt::Update, true, 10);
	TIMER::enableInterrupt(TIMER::Interrupt::Update);

	TIMER::applyAndReset();
	TIMER::start();
}

template< typename TIMER >
static void
testTimerStart()
{
	TIMER::enable();
	TIMER::setMode(TIMER::Mode::UpCounter);

	TIMER::template setPeriod<Board::systemClock>(250000);
	TIMER::enableInterruptVector(true, 10);
	TIMER::enableInterrupt(TIMER::Interrupt::Update);

	TIMER::applyAndReset();
	TIMER::start();
}

template< typename TIMER >
static void
testTimerAdvancedStop()
{
	TIMER::pause();
	TIMER::disableInterrupt(TIMER::Interrupt::Update);
	TIMER::enableInterruptVector(TIMER::Interrupt::Update, false, 10);
	TIMER::disable();
}

template< typename TIMER >
static void
testTimerStop()
{
	TIMER::pause();
	TIMER::disableInterrupt(TIMER::Interrupt::Update);
	TIMER::enableInterruptVector(false, 10);
	TIMER::disable();
}

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	uint16_t state = 0;
	bool restart = false;

	while (1)
	{
		LedBlue::set();
		modm::delayMilliseconds(20);
		LedBlue::reset();
		modm::delayMilliseconds(150);

		if (restart) {
			restart = false;
			state = 0;
		}

		switch (state)
		{
		case 0:
			testTimerAdvancedStart<Timer1>();
			break;

		case 10:
			testTimerAdvancedStop<Timer1>();
			break;


		case 12:
			testTimerStart<Timer2>();
			break;

		case 22:
			testTimerStop<Timer2>();
			break;


		case 24:
			testTimerStart<Timer3>();
			break;

		case 34:
			testTimerStop<Timer3>();
			break;


		case 36:
			testTimerStart<Timer4>();
			break;

		case 46:
			testTimerStop<Timer4>();
			break;


		case 48:
			testTimerStart<Timer5>();
			break;

		case 58:
			testTimerStop<Timer5>();
			break;


		case 60:
			testTimerStart<Timer6>();
			break;

		case 70:
			testTimerStop<Timer6>();
			break;


		case 72:
			testTimerStart<Timer7>();
			break;

		case 82:
			testTimerStop<Timer7>();
			break;


		case 84:
			testTimerAdvancedStart<Timer8>();
			break;

		case 94:
			testTimerAdvancedStop<Timer8>();
			break;


		case 96:
			testTimerStart<Timer9>();
			break;

		case 106:
			testTimerStop<Timer9>();
			break;


		case 108:
			testTimerStart<Timer10>();
			break;

		case 118:
			testTimerStop<Timer10>();
			break;


		case 120:
			testTimerStart<Timer11>();
			break;

		case 130:
			testTimerStop<Timer11>();
			break;


		case 132:
			testTimerStart<Timer12>();
			break;

		case 142:
			testTimerStop<Timer12>();
			break;


		case 144:
			testTimerStart<Timer13>();
			break;

		case 154:
			testTimerStop<Timer13>();
			break;


		case 156:
			testTimerStart<Timer14>();
			break;

		case 166:
			testTimerStop<Timer14>();
			break;

		case 180:
			restart = true;
			break;


		default:
			break;
		}

		// Next state
		++state;
	}

	return 0;
}

MODM_ISR(TIM2)
{
	Timer2::acknowledgeInterruptFlags(Timer2::InterruptFlag::Update);
	LedGreen::toggle();
}

MODM_ISR(TIM3)
{
	Timer3::acknowledgeInterruptFlags(Timer3::InterruptFlag::Update);
	LedRed::toggle();
}

MODM_ISR(TIM4)
{
	Timer4::acknowledgeInterruptFlags(Timer4::InterruptFlag::Update);
	LedGreen::toggle();
}

MODM_ISR(TIM5)
{
	Timer5::acknowledgeInterruptFlags(Timer5::InterruptFlag::Update);
	LedRed::toggle();
}

MODM_ISR(TIM6_DAC)
{
	Timer6::acknowledgeInterruptFlags(Timer6::InterruptFlag::Update);
	LedGreen::toggle();
}

MODM_ISR(TIM7)
{
	Timer7::acknowledgeInterruptFlags(Timer7::InterruptFlag::Update);
	LedRed::toggle();
}

// For TIM8 See TIM13

MODM_ISR(TIM1_BRK_TIM9)
{
	Timer9::acknowledgeInterruptFlags(Timer9::InterruptFlag::Update);
	LedGreen::toggle();
}

// Timer 1 and 10
MODM_ISR(TIM1_UP_TIM10)
{
	Timer1::acknowledgeInterruptFlags(Timer1::InterruptFlag::Update);
	Timer10::acknowledgeInterruptFlags(Timer10::InterruptFlag::Update);
	LedRed::toggle();
}

MODM_ISR(TIM1_TRG_COM_TIM11)
{
	Timer11::acknowledgeInterruptFlags(Timer11::InterruptFlag::Update);
	LedGreen::toggle();
}

MODM_ISR(TIM8_BRK_TIM12)
{
	Timer12::acknowledgeInterruptFlags(Timer12::InterruptFlag::Update);
	LedRed::toggle();
}

MODM_ISR(TIM8_UP_TIM13)
{
	Timer8::acknowledgeInterruptFlags(Timer8::InterruptFlag::Update);
	Timer13::acknowledgeInterruptFlags(Timer13::InterruptFlag::Update);
	LedGreen::toggle();
}

MODM_ISR(TIM8_TRG_COM_TIM14)
{
	Timer14::acknowledgeInterruptFlags(Timer14::InterruptFlag::Update);
	LedRed::toggle();
}
