/* Copyright (c) 2017, Carl Treudler
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------- */
#include <xpcc/architecture/platform.hpp>
#include <xpcc/processing/timer.hpp>

using namespace xpcc::stm32;

int main()
{
	Board::initialize();
	Board::LedD3::setOutput();


	while (1)
	{
		Board::LedD3::toggle();
		xpcc::delayMilliseconds(200);
	}
	return 0;
}
