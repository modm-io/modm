
#include <xpcc/architecture.hpp>

/**
 * Please connect PinA9 to PinA10 as loop back.
 *
 *
 **/

// ----------------------------------------------------------------------------
using namespace xpcc::stm32;

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
	//	SystemClock<Pll<ExternalOscillator<MHz8>, MHz48>>::enable();

	// Initialize Usart1 (which needs to be buffered)
	UART1_TX::connect(Usart1::Id);
	UART1_RX::connect(Usart1::Id);
	Usart1::initialize(115200);

	// Initialize Usart2
	UART2_TX::connect(Usart2::Id);
	UART2_RX::connect(Usart2::Id);
	Usart2::initialize(115200);

	// If you want to output strings more comfortably
	// have a look at the iostream example.

	// Test Block
	const uint8_t block [] = { 'W', 'o', 'r', 'l', 'd', '!', '\n' };

	Usart1::write(block, )

	Usart2::writeBlocking('H');
	Usart2::writeBlocking('e');
	Usart2::writeBlocking('l');
	Usart2::writeBlocking('l');
	Usart2::writeBlocking('o');
	Usart2::writeBlocking('\n');

	// Write Block
	const uint8_t str [] = { 'W', 'o', 'r', 'l', 'd', '!', '\n' };
	Usart2::writeBlocking(str, sizeof(str));

	while (1)
	{
	}

	return 0;
}
