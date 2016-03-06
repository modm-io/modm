
#include <xpcc/architecture.hpp>

GPIO__OUTPUT(Out, A, 0);
GPIO__INPUT(In, A, 1);
GPIO__IO(Io, A, 2);

int
main()
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

	while (1)
	{
	}
}
