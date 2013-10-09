
#include <xpcc/architecture.hpp>

using namespace xpcc::stm32;

MAIN_FUNCTION
{
	StartupError err =
		SystemClock<Pll<ExternalClock<MHz8>, MHz72>>::enable();

	// Initialize Usart Hal
	GpioOutputA2::connect(UsartHal2::Tx);
	GpioInputA3::connect(UsartHal2::Rx);
	UsartHal2::initialize(115200, UsartHal2::Parity::Odd);
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
