
#include <xpcc/architecture.hpp>

xpcc::xmega::BufferedUartD0 uart(9600);

GPIO__OUTPUT(Out, A, 0);
GPIO__INPUT(In, A, 1);
GPIO__IO(Io, A, 2);

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
	
	uart.write('x');
	
	while (1)
	{
	}
}
