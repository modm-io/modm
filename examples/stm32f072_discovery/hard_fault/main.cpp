#include "../stm32f072_discovery.hpp"
#include <xpcc/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart1, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);


MAIN_FUNCTION
{
	Board::initialize();

	// initialize Uart for XPCC_LOG
	GpioOutputA9::connect(Usart1::Tx);
	Usart1::initialize<Board::DefaultSystemClock, 115200>(12);

	XPCC_LOG_INFO << "Causing a Hardfault now!" << xpcc::endl;

	// simulate some stack usage
	asm volatile ("push {r0-r7}");
	asm volatile ("push {r0-r7}");
	asm volatile ("push {r0-r7}");
	asm volatile ("pop {r0-r7}");
	asm volatile ("pop {r0-r7}");
	asm volatile ("pop {r0-r7}");

	// load some patterns into r0-r7
	asm volatile ("ldr r0, =0xA000000A");
	asm volatile ("ldr r1, =0x0AAAAAA0");
	asm volatile ("ldr r2, =0x00A00A00");
	asm volatile ("ldr r3, =0x000AA000");
	asm volatile ("ldr r4, =0x04040404");
	asm volatile ("ldr r5, =0x05050505");
	asm volatile ("ldr r6, =0x06060606");
	asm volatile ("ldr r7, =0x07070707");

	// execute unused stack
	asm volatile ("ldr r4, =0x20000247");
	asm volatile ("bx r4");

	// undefined instruction
	asm volatile (".short 0xde00");

	// stack overflow
	while(1) asm volatile ("push {r0-r7}");

	while (1)
	{
		xpcc::delayMilliseconds(1000);
		Board::LedUp::toggle();
	}

	return 0;
}
