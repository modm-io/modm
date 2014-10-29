
#include <xpcc/architecture.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;

typedef Uart0 uart;

typedef GpioOutputB0 Out;
typedef GpioInputB1  In;
typedef GpioB2       Io;

int
main(void)
{
	Out::setOutput();
	Out::setOutput(true);
	Out::set();
	Out::reset();	
	Out::toggle();
	
	In::setInput();
	In::read();
	
	Io::setOutput();
	Io::set();
	Io::setInput();
	Io::read();
	
	GpioOutputD1::connect(Uart0::Tx);
	GpioInputD0::connect(Uart0::Rx);
	uart::initialize<clock, 9600>();
	uart::write('x');
	
	while (1)
	{
	}
}
