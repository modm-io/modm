#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#include <xpcc/processing/timer.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart1, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

int
main()
{
	Board::initialize();

    // initialize Uart2 for XPCC_LOG_*
    GpioOutputA9::connect(Usart1::Tx);
    GpioInputA10::connect(Usart1::Rx, Gpio::InputType::PullUp);
    Usart1::initialize<Board::systemClock, 115200>(12);

    // Use the logging streams to print some messages.
    // Change XPCC_LOG_LEVEL above to enable or disable these messages
    XPCC_LOG_DEBUG   << "debug"   << xpcc::endl;
    XPCC_LOG_INFO    << "info"    << xpcc::endl;
    XPCC_LOG_WARNING << "warning" << xpcc::endl;
    XPCC_LOG_ERROR   << "error"   << xpcc::endl;

	Board::LedBlue::reset();

	uint32_t ii(1);
	xpcc::Timeout timeout;

	while (true) {
        Board::LedBlue::set();
        timeout.restart(100);
        while(not timeout.isExpired())
        	{};

        Board::LedBlue::reset();
        timeout.restart(900);
        while(not timeout.isExpired())
        	{};

		XPCC_LOG_INFO << "Seconds since reboot: " << ii++ << xpcc::endl;
	}
}
