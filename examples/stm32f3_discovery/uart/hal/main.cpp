#include <xpcc/architecture.hpp>
#include "../../stm32f3_discovery.hpp"

MAIN_FUNCTION
{
	defaultSystemClock::enable();

	// Initialize Usart Hal
	GpioOutputA2::connect(UsartHal2::Tx);
	GpioInputA3::connect(UsartHal2::Rx, Gpio::InputType::PullUp);
//	UsartHal2::initialize<defaultSystemClock, 115200>(UsartHal2::Parity::Odd);
	UsartHal2::enableInterruptVector(true, 14);
	UsartHal2::enableInterrupt(UsartHal2::Interrupt::TxEmpty);
	UsartHal2::setTransmitterEnable(true);

	while (1)
	{
	}

	return 0;
}

// Interrupt Handler
extern "C" void
USART2_IRQHandler(void)
{
	UsartHal2::getInterruptFlags();
	if(UsartHal2::isTransmitRegisterEmpty()) {
		UsartHal2::write('A');
	}
}
