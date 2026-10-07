/*
 * Copyright (c) 2023, Christopher Durand
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

/**
 * Simple DAC example
 *
 * Output sawtooth waveform in free-running mode.
 *
 * See DMA examples for use of DAC with DMA.
 */

using Out0 = GpioB13;

void waitForTxReady()
{
	while (!(Dac::readInterruptFlags() & Dac::Interrupt::TxReadyCh0));
}

int main()
{
	Board::initialize();

	Dac::connect<Out0::Ch0>();
	Dac::initialize<SystemClock, 12_MHz>();

	Dac::enableChannel(Dac::Channel::Channel0);

	while (true)
	{
		for (int i = 0; i <= 4095; ++i)
		{
			waitForTxReady();
			Dac::setOutput0(i);
		}
	}

	return 0;
}
