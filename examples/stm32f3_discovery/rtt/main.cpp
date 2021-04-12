/*
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing.hpp>
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

/*

 $ scons log-rtt
╭───OpenOCD───> Real Time Transfer
╰─────RTT────── stm32f303vct6
Info : STLINK V2J16S0 (API v2) VID:PID 0483:3748
Info : stm32f3x.cpu: hardware has 6 breakpoints, 4 watchpoints
Info : rtt: Searching for control block 'modm.rtt.modm'
Info : rtt: Control block found at 0x20000c04
Info : Listening on port 9090 for rtt connections
Info
Warning
Error
loop: 0
loop: 1
loop: 2
loop: 3
loop: 4
loop: 5


Type number 0-9, then press enter to send.
The LED should blink slower or faster.

Ctrl+D to exit

*/

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	MODM_LOG_DEBUG << "Debug" << modm::endl;
	MODM_LOG_INFO << "Info" << modm::endl;
	MODM_LOG_WARNING << "Warning" << modm::endl;
	MODM_LOG_ERROR << "Error" << modm::endl;

	uint32_t counter(0);
	modm::PeriodicTimer tmr(100ms);

	char data;
	while (true)
	{
		MODM_LOG_INFO.get(data);
		switch(data)
		{
			case '0':
				tmr.restart(1s);
				break;
			case '1'...'9':
				tmr.restart(std::chrono::milliseconds((data - '0') * 100));
				break;
		}
		if (tmr.execute())
		{
			LedNorth::toggle();

			MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
		}
	}

	return 0;
}
