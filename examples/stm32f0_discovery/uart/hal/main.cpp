
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
	//	SystemClock<Pll<ExternalClock<MHz8>, MHz48>>::enable();

	// Initialize Usart Hal
	UART2_TX::connect(UsartHal2::Tx);
	UART2_RX::connect(UsartHal2::Rx);
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
