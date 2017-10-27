/**
 * \brief	Minimal example for LPC11C24 on LPCXpresso board.
 *
 *
 */

#include <xpcc/architecture.hpp>

#include <xpcc/debug/logger.hpp>

using namespace xpcc::lpc;

using WriteInd = GpioOutput3_1;

// ----------------------------------------------------------------------------
// Logging
using Uart = Uart1;
xpcc::IODeviceWrapper< Uart, xpcc::IOBuffer::BlockIfFull > loggerDevice;

xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);
// ----------------------------------------------------------------------------


void
testWriteSingle(void);

void
testWriteBuffer(void);

void
testXpccLogger(void);

int
main()
{
	Board::initialize();

	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	// init_timer32(0, TIME_INTERVAL);

	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	// enable_timer32(0);

	Uart::initialize<Board::systemClock, 115200>();

	xpcc::delayMilliseconds(100); // glitch ?

	Uart::write('X');
	Uart::write('I');
	Uart::write('X');

//	testWriteSingle();
//	testWriteBuffer();

	testXpccLogger();

	while(true);

		// Single byte reading and writing, loopback.
//		uint8_t c;
//		if (Uart::read(c))
//		{
//			Uart::write(c);
//		}

		// Buffer reading and writing
//		uint8_t buf[16];
//		uint8_t rr = Uart::read(buf, 16);
//		Uart::write(buf, rr);
}

void
testWriteSingle(void)
{
	while (true)
	{
		Board::LedRed::toggle();

		// Multiple single byte writes in blocking mode
		Uart::write('\n');
		for (uint8_t ii = 'a'; ii < 't'; ++ii) {
			WriteInd::setOutput(true);
			Uart::write(ii);
			WriteInd::setOutput(false);
		}

		xpcc::delayMilliseconds(2);

		// Some pause between burst otherwise USB is overloaded.
		static uint8_t burst = 0;
		if (burst++ > 5)
		{
			burst = 0;
			xpcc::delayMilliseconds(500);
		}
	} // while (true)
}

void
testWriteBuffer(void)
{
	uint8_t buf[40];
	for (uint8_t ii = 0; ii < 40; ++ii)
	{
		buf[ii] = ii + 'A';
	}

	while (true)
	{
		Board::LedRed::toggle();

		WriteInd::setOutput(true);
		Uart::write(buf, 40);
		WriteInd::setOutput(false);
		WriteInd::setOutput(true);
		Uart::write('\n');
		WriteInd::setOutput(false);

		xpcc::delayMilliseconds(2);

		// Some pause between burst otherwise USB is overloaded.
		static uint8_t burst = 0;
		if (burst++ > 5) {
			burst = 0;
			xpcc::delayMilliseconds(5000);
		}
	} // while (true)
}

void
testXpccLogger(void)
{
	while (true)
	{
		XPCC_LOG_DEBUG << "Hello world. This is more than 16 chars." << xpcc::endl;
		xpcc::delayMilliseconds(10);
	}
}
