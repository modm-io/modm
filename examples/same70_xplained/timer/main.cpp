/*
 * Copyright (c) 2023, Christopher Durand
 * Copyright (c) 2023, Luiz Carlos Gili
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/platform/timer/timer_channel_0.hpp>
#include <modm/platform/timer/timer_channel_10.hpp>

using namespace Board;

// use timer channel 10 to toggle led
MODM_ISR(TC10)
{
	// clear interrupt flags by reading
	(void) TimerChannel10::getInterruptFlags();
	Led0::toggle();
}

using Tioa0 = GpioA0;


int main()
{
	Board::initialize();

	MODM_LOG_INFO << "Timer / Counter Test" << modm::endl;

	// generate 25% duty-cycle 100 kHz PWM waveform on TIOA0 output (GpioA0)
	TimerChannel0::initialize();
	TimerChannel0::connect<Tioa0::Tioa>();

	TimerChannel0::setClockSource(TimerChannel0::ClockSource::Mck);
	TimerChannel0::setWaveformMode(true);
	// Up-counter, reset on register C compare match
	TimerChannel0::setWaveformSelection(TimerChannel0::WavSel::Up_Rc);

	// Clear output on register A match, set on register C match
	TimerChannel0::setTioaEffects(TimerChannel0::TioEffect::Clear, TimerChannel0::TioEffect::Set);
	// period MCLK = 150 MHz / 1500 = 100 kHz
	// duty-cycle 375 / 1500 = 25%
	TimerChannel0::setRegA(375);
	TimerChannel0::setRegC(1500);
	TimerChannel0::enable();
	TimerChannel0::start();

	// setup timer channel 10 to run interrupt at ~1 Hz from ~32 kHz internal slow clock
	TimerChannel10::initialize();
	TimerChannel10::setClockSource(TimerChannel10::ClockSource::Slck);
	// Toggle every 8192 / 32768 kHz = 0.25s => 1 Hz period
	TimerChannel10::setRegC(8192);
	TimerChannel10::setWaveformMode(true);
	TimerChannel10::setWaveformSelection(TimerChannel10::WavSel::Up_Rc);
	TimerChannel10::enableInterruptVector(true);
	TimerChannel10::enableInterrupt(TimerChannel10::Interrupt::RcCompare);
	TimerChannel10::enable();
	TimerChannel10::start();

	while (true)
	{
		modm::delay(500ms);
	}

	return 0;
}
