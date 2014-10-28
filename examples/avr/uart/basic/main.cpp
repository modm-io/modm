
#include <xpcc/architecture/platform.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;

// Create a new UART object and configure it to a baudrate of 115200
typedef Uart0 uart;

int
main()
{
    GpioOutputD1::connect(Uart0::Tx);
    GpioInputD0::connect(Uart0::Rx);
    uart::initialize<clock, 115200>();

    // Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Write some characters
    uart::write('H');
    uart::write('e');
    uart::write('l');
    uart::write('l');
    uart::write('o');
    uart::write('\n');
	
	while (1)
	{
	}
}
