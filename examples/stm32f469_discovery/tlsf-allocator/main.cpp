#include <xpcc/architecture/platform.hpp>

int main()
{
	Board::initialize();
	Board::Leds::setOutput();

	// 600 pointers to allocated memories
	uint8_t *d[600];
	int free_ii = 0;

	for (int ii=0; ii < 600; ii++)
	{
		d[ii] = nullptr;
		uint32_t size = rand() % (1024 * 100);

		d[ii] = new (xpcc::MemoryFastData) uint8_t[size];

		// print what size we requested and if it succeeded
		XPCC_LOG_INFO.printf(" malloc(%3ukB) = ", size/1024);
		if (d[ii]) XPCC_LOG_INFO << d[ii];
		else   XPCC_LOG_INFO << "NO MEMORY ";

		if ((rand() % 100) >= 50) {
			// only some memory is returned to the system
			uint8_t *df;
			while ((df = d[free_ii++]) == nullptr and free_ii <= ii) ;
			XPCC_LOG_INFO << "  ...freeing " << df;
			delete df;
		}
		XPCC_LOG_INFO << xpcc::endl;
	}

	while (1)
	{
		Board::Leds::toggle();
		xpcc::delayMilliseconds(500);
	}
	return 0;
}
