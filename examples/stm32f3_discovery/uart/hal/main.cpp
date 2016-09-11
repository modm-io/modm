#include <xpcc/architecture/platform.hpp>

int
main()
{
	Board::initialize();

	// Initialize Usart Hal
	GpioOutputA2::connect(UsartHal2::Tx);
	GpioInputA3::connect(UsartHal2::Rx, Gpio::InputType::PullUp);
//	UsartHal2::initialize<Board::systemClock, 115200>(UsartHal2::Parity::Odd);
	UsartHal2::enableInterruptVector(true, 14);
	UsartHal2::enableInterrupt(UsartHal2::Interrupt::TxEmpty);
	UsartHal2::setTransmitterEnable(true);

	while (1)
	{
	}

	return 0;
}

// Interrupt Handler
XPCC_ISR(USART2)
{
	UsartHal2::getInterruptFlags();
	if(UsartHal2::isTransmitRegisterEmpty()) {
		UsartHal2::write('A');
	}
}
