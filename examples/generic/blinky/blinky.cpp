/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------- */
#include <modm/architecture/platform.hpp>

int main()
{
	Board::initialize();
	Board::Leds::setOutput();

	while (true)
	{
		Board::Leds::toggle();
		modm::delayMilliseconds(Board::Button::read() ? 250 : 500);
#ifdef MODM_BOARD_HAS_LOGGER
		static uint32_t counter(0);
		MODM_LOG_INFO << "Loop counter: " << (counter++) << modm::endl;
#endif
	}
	return 0;
}
