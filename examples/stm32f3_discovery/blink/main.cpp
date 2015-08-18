#include "../stm32f3_discovery.hpp"
#include <xpcc/processing/timer.hpp>

using namespace Board;

MAIN_FUNCTION
{
	Board::initialize();

//	Board::LedNorth::set();

	xpcc::PeriodicTimer tmr(100);

	while (1)
	{
		Board::LedNorth::toggle();

		while(not tmr.execute()) ;
//		xpcc::delayMilliseconds(100);

		Board::LedNorthEast::toggle();

		while(not tmr.execute()) ;
//		xpcc::delayMilliseconds(100);

		Board::LedEast::toggle();

		while(not tmr.execute()) ;
//		xpcc::delayMilliseconds(100);

		Board::LedSouthEast::toggle();

		while(not tmr.execute()) ;
//		xpcc::delayMilliseconds(100);

		Board::LedSouth::toggle();

		while(not tmr.execute()) ;
//		xpcc::delayMilliseconds(100);

		Board::LedSouthWest::toggle();

		while(not tmr.execute()) ;
//		xpcc::delayMilliseconds(100);

		Board::LedWest::toggle();

		while(not tmr.execute()) ;
//		xpcc::delayMilliseconds(100);

		Board::LedNorthWest::toggle();

		while(not tmr.execute()) ;
//		xpcc::delayMilliseconds(100);
	}

	return 0;
}
