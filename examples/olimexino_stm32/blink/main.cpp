/* Copyright (c) 2017, Carl Treudler
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------- */
#include <modm/board/board.hpp>

int main()
{
	Board::initialize();
	Board::LedD3::setOutput();


	while (1)
	{
		Board::LedD3::toggle();
		modm::delayMilliseconds(200);
	}
	return 0;
}
