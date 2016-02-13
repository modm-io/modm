#include "../stm32f072_discovery.hpp"

// ----------------------------------------------------------------------------
/**
 * Very basic example of USART usage.
 * The ASCII sequence 'A', 'B', 'C, ... , 'Z', 'A', 'B', 'C', ...
 * is printed with 9600 baud, 8N1 at pin PA9.
 */
MAIN_FUNCTION
{
	Board::initialize();

	Board::LedUp::set();

	// Enable USART 1
	GpioOutputA9::connect(Usart1::Tx);
	GpioInputA10::connect(Usart1::Rx, Gpio::InputType::PullUp);
	Usart1::initialize<Board::systemClock, 9600>(12);

	while (1)
	{
		static uint8_t c = 'A';
		Board::LedUp::toggle();
		Board::LedDown::toggle();
		Usart1::write(c);
		++c;
		if (c > 'Z') {
			c = 'A';
		}
		xpcc::delayMilliseconds(500);
	}

	return 0;
}
