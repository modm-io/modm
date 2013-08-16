#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

// ----------------------------------------------------------------------------
using namespace xpcc::stm32;

// Usart 3 Pins
typedef GpioOutputA2 UART2_TX;
typedef GpioInputA3 UART2_RX;

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2 > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	//StartupError err =
	//	SystemClock<Pll<ExternalOscillator<MHz8>, MHz48>>::enable();

	// Initialize Usart
	UART2_TX::connect(Usart2::Tx);
	UART2_RX::connect(Usart2::Rx);
	Usart2::initialize(115200, 10);

	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages 
	XPCC_LOG_DEBUG   << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << "error"   << xpcc::endl;

	while (1)
	{
	}

	return 0;
}
