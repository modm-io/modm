#include "../stm32f4_discovery.hpp"

// ----------------------------------------------------------------------------
/**
 * Very basic example of USART usage.
 * The ASCII sequence 'A', 'B', 'C, ... , 'Z', 'A', 'B', 'C', ...
 * is printed with 9600 baud, 8N1 at pin PA3.
 */
int
main()
{
	Board::initialize();

	Board::LedRed::set();

	// Enable USART 2
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 9600>(12);

	while (1)
	{
		static uint8_t c = 'A';
		Board::LedRed::toggle();
		Board::LedGreen::toggle();
		Usart2::write(c);
		++c;
		if (c > 'Z') {
			c = 'A';
		}
		xpcc::delayMilliseconds(500);
	}

	return 0;
}
