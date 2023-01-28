/*
 * Copyright (c) 2021, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/io/iostream.hpp>
#include <modm/platform.hpp>

using namespace modm::platform;
using namespace modm::literals;

MODM_ISR(TC3)
{
	// Clear pending interrupts by reading them
	(void)TimerChannel3::getInterruptFlags();

	static bool flag = false;
	if (flag)
	{
		TimerChannel0::setTiobEffects(TimerChannel0::TioEffect::Clear,
									  TimerChannel0::TioEffect::Set);
	} else
	{
		TimerChannel0::setTiobEffects(TimerChannel0::TioEffect::Set,
									  TimerChannel0::TioEffect::Clear);
	}
	flag = !flag;
}

int
main()
{
	/*
	 * This example uses channel 0 to generate two output waveforms, and channel3
	 * to create a periodic IRQ which swaps the polarity on the TIOB output.
	 *
	 * Note:
	 * On the SAMG55, the waveform outputs on the second module (TC1, TimerChannel[3-5])
	 * are not connected to external pins.
	 */
	Board::initialize();

	TimerChannel0::initialize();
	TimerChannel0::connect<GpioA0::Tioa, GpioA1::Tiob>();

	TimerChannel0::setClockSource(TimerChannel0::ClockSource::MckDiv2);
	TimerChannel0::setWaveformMode(true);
	// Setup timer to count up, and trigger reset on Rc match
	TimerChannel0::setWaveformSelection(TimerChannel0::WavSel::Up_Rc);

	// Setup TioA to set on RA match, and clear on RC match
	TimerChannel0::setTioaEffects(TimerChannel0::TioEffect::Set, TimerChannel0::TioEffect::Clear);
	// Setup TioB to clear on RB match, and set on RC match
	TimerChannel0::setTiobEffects(TimerChannel0::TioEffect::Clear, TimerChannel0::TioEffect::Set);

	// Change external event source, so that TIOB can be used as an output
	TimerChannel0::setExtEventSource(TimerChannel0::ExtEventSource::Xc0);

	TimerChannel0::setRegA(10000);
	TimerChannel0::setRegB(5000);
	TimerChannel0::setRegC(15000);

	TimerChannel0::enable();
	TimerChannel0::start();

	// Setup TC3 irq to swap TIOB polarity periodically
	// Period = 128 * 10000 / 120MHz = ~10.6ms
	TimerChannel3::initialize();
	TimerChannel3::setClockSource(TimerChannel0::ClockSource::MckDiv128);
	TimerChannel3::setRegC(10000);
	TimerChannel3::setWaveformMode(true);
	TimerChannel3::setWaveformSelection(TimerChannel0::WavSel::Up_Rc);
	TimerChannel3::enableInterruptVector(true);
	TimerChannel3::enableInterrupt(TimerChannel3::Interrupt::RcCompare);
	TimerChannel3::enable();
	TimerChannel3::start();

	while (true)
		;
}