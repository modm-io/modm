
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
using namespace xpcc::stm32;

// Usart 3 Pins
typedef GpioOutputA2 UART2_TX;
typedef GpioInputA3 UART2_RX;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	//StartupError err =
	//	SystemClock<Pll<ExternalOscillator<MHz8>, MHz48>>::enable();

	// Initialize Usart
	UART2_TX::connect(Usart2::Id);
	UART2_RX::connect(Usart2::Id);
	Usart2::initialize(115200);

	// If you want to output strings more comfortably
	// have a look at the logger example.
	// Write Single Bytes
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
