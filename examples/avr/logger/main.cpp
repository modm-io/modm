
#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;

// Create a new UART object and configure it to a baudrate of 115200
Uart0 uart;
xpcc::IODeviceWrapper< Uart0 > loggerDevice(uart);

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

int
main()
{
    GpioOutputD1::connect(Uart0::Tx);
    GpioInputD0::connect(Uart0::Rx);
    Uart0::initialize<clock, 115200>();
    
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages 
	XPCC_LOG_DEBUG   << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << "error"   << xpcc::endl;
	
	while (1)
	{
	}
}
