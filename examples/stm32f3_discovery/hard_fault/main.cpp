#include "../stm32f3_discovery.hpp"
#include <xpcc/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	Board::initialize();

	// initialize Uart2 for XPCC_LOG_
	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<Board::systemClock, 115200>(12);

	XPCC_LOG_INFO << "Causing a Hardfault now!" << xpcc::endl;

	// simulate some stack usage
	asm volatile ("push {r0-r12}");
	asm volatile ("push {r0-r12}");
	asm volatile ("push {r0-r12}");
	asm volatile ("pop {r0-r12}");
	asm volatile ("pop {r0-r12}");
	asm volatile ("pop {r0-r12}");

	// execute unused stack
	asm volatile ("ldr pc, =0x20000247");

	// divide by zero
	volatile uint8_t number = 42;
	volatile uint8_t divisor = 0;
	number /= divisor;

	// undefined instruction
	asm volatile (".short 0xde00");

	// stack overflow
	while(1) asm volatile ("push {r0-r12}");

	while (1)
	{
		xpcc::delayMilliseconds(250);
		Board::LedSouth::toggle();
	}

	return 0;
}
