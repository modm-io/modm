
#include <xpcc/architecture.hpp>

GPIO__OUTPUT(Out, 3, 0);
GPIO__INPUT(In, 3, 1);
GPIO__IO(Io, 3, 2);

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
	
	// uart.write('x');
	
	while (1)
	{
	}
}
