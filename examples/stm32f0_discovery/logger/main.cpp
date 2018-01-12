#include <modm/board/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing/timer.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart1, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

int
main()
{
	Board::initialize();

    // initialize Uart1 for MODM_LOG_*
    Usart1::connect<GpioOutputA9::Tx, GpioInputA10::Rx>();
    Usart1::initialize<Board::systemClock, 115200>(12);

    // Use the logging streams to print some messages.
    // Change MODM_LOG_LEVEL above to enable or disable these messages
    MODM_LOG_DEBUG   << "debug"   << modm::endl;
    MODM_LOG_INFO    << "info"    << modm::endl;
    MODM_LOG_WARNING << "warning" << modm::endl;
    MODM_LOG_ERROR   << "error"   << modm::endl;

	Board::LedBlue::reset();

	uint32_t ii(1);
	modm::Timeout timeout;

	while (true) {
        Board::LedBlue::set();
        timeout.restart(100);
        while(not timeout.isExpired())
        	{};

        Board::LedBlue::reset();
        timeout.restart(900);
        while(not timeout.isExpired())
        	{};

		MODM_LOG_INFO << "Seconds since reboot: " << ii++ << modm::endl;
	}
}
