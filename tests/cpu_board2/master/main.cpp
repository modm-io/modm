
#include <xpcc/architecture.hpp>
#include <xpcc/architecture/board/cpuboard2/master.hpp>

typedef xpcc::CpuBoard2<> Board; 

MAIN_FUNCTION
{
	Board::initialize();

	while (1)
	{
	}
}

