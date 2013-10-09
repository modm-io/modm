#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>
/**
 * Please connect PinA9 to PinA10 as loop back.
 *
 *
 **/

// ----------------------------------------------------------------------------
using namespace xpcc::stm32;

// Usart 2 is used for text output
xpcc::IODeviceWrapper< Usart2 > loggerDevice;
xpcc::log::Logger xpcc::log::info(loggerDevice);

// Usart2 Pins
typedef GpioOutputA2 UART2_TX;
typedef GpioInputA3 UART2_RX;
// Usart1 Pins
typedef GpioOutputA9 UART1_TX;
typedef GpioInputA10 UART1_RX;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	//StartupError err =
	//	SystemClock<Pll<ExternalClock<MHz8>, MHz48>>::enable();

	// Initialize Usart1 (which needs to be buffered)
	UART1_TX::connect(Usart1::Tx);
	UART1_RX::connect(Usart1::Rx);
	Usart1::initialize(115200, 12);

	// Initialize Usart2
	UART2_TX::connect(Usart2::Tx);
	UART2_RX::connect(Usart2::Rx);
	Usart2::initialize(115200, 13);

	// Test Blocks
	const uint8_t block1 [] = { 'W', 'o', 'r', 'l', 'd', '!', '\n' };
	const uint8_t block2 [] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n' };
	uint8_t return_block [40];

	// Send block 1 asynchronously
	Usart1::write(block1, sizeof(block1));
	// Wait for block 1 to be written to receive buffer
	xpcc::delay_ms(500);
	// Read back from receive buffer
	Usart1::read(return_block, sizeof(block1));
	// Success ??
	if(return_block[sizeof(block1)-1] == block1[sizeof(block1)-1]) {
		XPCC_LOG_INFO << "Received block of size " << sizeof(block1)
													<< "." << xpcc::endl;
	} else {
		XPCC_LOG_INFO << "Failed to receive block of size " << sizeof(block1)
													<< "." << xpcc::endl;
	}

	// Send block 2 asynchronously
	Usart1::write(block2, sizeof(block2));
	// Wait for block 1 to be written to receive buffer
	xpcc::delay_ms(500);
	// Read back from receive buffer
	Usart1::read(return_block, sizeof(block2));
	// Success ??
	if(return_block[sizeof(block2)-1] == block2[sizeof(block2)-1]) {
		XPCC_LOG_INFO << "Received block of size " << sizeof(block2)
													<< "." << xpcc::endl;
	} else {
		XPCC_LOG_INFO << "Failed to receive block of size " << sizeof(block2)
													<< "." << xpcc::endl;
	}
	// Trying to receive block2 fails because it is too lang for the buffer

	while (1)
	{
	}

	return 0;
}
