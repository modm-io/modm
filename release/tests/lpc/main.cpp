
#include <xpcc/architecture.hpp>

GPIO__OUTPUT(Out, 0, 0);
GPIO__INPUT(In, 0, 1);
GPIO__IO(Io, 0, 2);

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

	while (1)
	{
	}
}
