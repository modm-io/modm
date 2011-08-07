
#include <xpcc/architecture.hpp>
#include <xpcc/architecture/board/cpuboard2/slave.hpp>

typedef xpcc::CpuBoard2Slave<> Board;

MAIN_FUNCTION
{
	Board::initialize();

	while (1)
	{
	}
}

