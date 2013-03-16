
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedNorth,     E,  9); // LD3
GPIO__OUTPUT(LedNorthEast, E, 10); // LD5
GPIO__OUTPUT(LedEast,      E, 11); // LD7
GPIO__OUTPUT(LedSouthEast, E, 12); // LD9
GPIO__OUTPUT(LedSouth,     E, 13); // LD10
GPIO__OUTPUT(LedSouthWest, E, 14); // LD8
GPIO__OUTPUT(LedWest,      E, 15); // LD6
GPIO__OUTPUT(LedNorthWest, E,  8); // LD4

GPIO__INPUT(Button, A, 0);

// Usart 3 Pins
GPIO__OUTPUT(UART3_TX, B, 10);
GPIO__INPUT(UART3_RX, B, 11);

using namespace xpcc::stm32;

static bool
initClock()
{
	// use external 8MHz clock from ST-LINK
	if (!Clock::enableHse(Clock::HseConfig::HSE_BYPASS)) {
		return false;
	}
	
	Clock::enablePll(Clock::PllSource::PLL_HSE, Clock::PllMul::MUL_9);
	return Clock::switchToPll();
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	initClock();

	LedNorth::setOutput(xpcc::gpio::LOW);
	LedNorthEast::setOutput(xpcc::gpio::LOW);
	LedEast::setOutput(xpcc::gpio::LOW);
	LedSouthEast::setOutput(xpcc::gpio::LOW);
	LedSouth::setOutput(xpcc::gpio::LOW);
	LedSouthWest::setOutput(xpcc::gpio::LOW);
	LedWest::setOutput(xpcc::gpio::LOW);
	LedNorthWest::setOutput(xpcc::gpio::LOW);

	// Initialize Usart Hal
	UART3_TX::setAlternateFunction(AF_USART3, xpcc::stm32::PUSH_PULL);
	UART3_RX::setAlternateFunction(AF_USART3);
	UsartHal3::initialize(115200, UsartHal3::Parity::Odd);
	UsartHal3::enableInterruptVector(true, 14);
	UsartHal3::enableInterrupt(UsartHal3::INTERRUPT_TX_EMPTY);
	UsartHal3::setTransmitterEnable(true);

	while (1)
	{
	}

	return 0;
}

// Interrupt Handler
extern "C" void
USART3_IRQHandler(void)
{
	UsartHal3::getInterruptFlags();
	if(UsartHal3::isTransmitRegisterEmpty()) {
		UsartHal3::write(0x46);
	}
}
