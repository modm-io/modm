/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------- */
#include <xpcc/architecture/platform.hpp>

int main()
{
	Board::initialize();
	Board::Leds::setOutput();

	while (true)
	{
		Board::Leds::toggle();
		xpcc::delayMilliseconds(Board::Button::read() ? 250 : 500);
#ifdef XPCC_BOARD_HAS_LOGGER
		static uint32_t counter(0);
		XPCC_LOG_INFO << "Loop counter: " << (counter++) << xpcc::endl;
#endif
	}
	return 0;
}
