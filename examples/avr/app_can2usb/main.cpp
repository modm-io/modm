// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

/**
 * CAN2USB Hardware Revision 1.3
 *
 * An AT90CAN128 and FT245 based CAN sniffer.
 *
 * AT90CAN128 fuse settings:
 *
 * lfuse: 0xfe
 * hfuse: 0xd1
 * efuse: 0xfd
 *
 */

#include <xpcc/architecture/platform.hpp>

using namespace xpcc::at90;

// LED1 Status
typedef GpioOutputD7 LedStatus;

// LED2 RX
typedef GpioOutputG3 LedRxLow;
typedef GpioOutputG4 LedRxHigh;

// LED3 TX
typedef GpioOutputB6 LedTxLow;
typedef GpioOutputB7 LedTxHigh;

// LED4
typedef GpioOutputF7 Led4Low;
typedef GpioOutputF4 Led4High;

// LED5
typedef GpioOutputF6 Led5Low;
typedef GpioOutputF3 Led5High;

MAIN_FUNCTION
{
	LedStatus::setOutput();
	LedStatus::set();

	LedRxLow::setOutput();
	LedRxHigh::setOutput();

	LedTxLow::setOutput();
	LedTxHigh::setOutput();

	Led4Low::setOutput();
	Led4High::setOutput();

	Led5Low::setOutput();
	Led5High::setOutput();

	LedTxHigh::set();
	LedRxLow::set();
	Led4High::set();
	Led5Low::set();

	while (1)
	{
		xpcc::delayMilliseconds(1000);
		LedStatus::toggle();
		LedRxHigh::toggle();
		LedRxLow::toggle();
		LedTxHigh::toggle();
		LedTxLow::toggle();
		Led4Low::toggle();
		Led4High::toggle();
		Led5Low::toggle();
		Led5High::toggle();

	}
}
