/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------ */

#include "al_avreb_can.hpp"
#include <xpcc/architecture/driver/clock.hpp>

ISR(TIMER0_COMP_vect)
{
	xpcc::Clock::increment();
}
