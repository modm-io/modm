
#include <xpcc/architecture.hpp>
#include <xpcc/processing/rtos.hpp>

using namespace xpcc::stm32;
using namespace xpcc::cortex;


// ----------------------------------------------------------------------------
template <typename GPIO>
class P: xpcc::rtos::Thread {
private:
	char c;
	int i;
	volatile float a = 1;
	int sleepTime;
public:
	P(int sleepTime, char c):
	Thread(2,1<<10),
	c(c),
	sleepTime(sleepTime)
	{

	}
	void run() {
		while (1) {
			sleep(sleepTime);

			GPIO::toggle();
			xpcc::stm32::Usart2::writeBlocking(i + c);

			i = (i+1)%10;
			a = a*10;
		}
	}
};

P<GpioD12> p1(260, '0');
P<GpioD13> p2(260+10, 'a');
P<GpioD14> p3(260+20, 'A');
P<GpioD15> p4(260+30, '!');


// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	typedef Pll<ExternalClock<MHz8>, MHz168, MHz48> clockSource;
	typedef SystemClock<clockSource> systemClock;
	systemClock::enable();

	GpioD12::setOutput();
	GpioD13::setOutput();
	GpioD14::setOutput();
	GpioD15::setOutput();

	GpioA2::connect(xpcc::stm32::Usart2::Tx);
	GpioA3::connect(xpcc::stm32::Usart2::Rx, Gpio::InputType::PullUp);
	xpcc::stm32::Usart2::initialize<systemClock, xpcc::stm32::Usart2::B115200>(6);

    while (1)
    {
    	xpcc::rtos::Scheduler::schedule();
    }

    // should not get here!
    return 0;
}
