
#include <xpcc/architecture.hpp>

using Out = xpcc::lpc::GpioOutput0_1;
using In  = xpcc::lpc::GpioInput0_2;
using Io  = xpcc::lpc::Gpio0_3;

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

	while (true)
	{
	}
}
