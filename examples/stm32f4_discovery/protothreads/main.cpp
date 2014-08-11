#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/processing/timeout.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/driver/temperature/tmp102.hpp>

class ProtofaceDriverTest : public xpcc::pt::Protothread
{
public:
	ProtofaceDriverTest()
	:	temperature(temperatureData)
	{
	}

	bool
	update()
	{
		PT_BEGIN();

		PT_WAIT_UNTIL(temperature.startConfigure());
		PT_WAIT_UNTIL(temperature.runConfigure());

		while (true)
		{
			PT_WAIT_UNTIL(temperature.startReadTemperature());
			PT_WAIT_UNTIL(temperature.runReadTemperature());
			if (temperature.isSuccessful()) {
				tf = temperature.getTemperature();
			} else {
				tf = NAN;
			}

			this->timer.restart(500);
			PT_WAIT_UNTIL(this->timer.isExpired());
		}

		PT_END();
	}

private:
	float tf;
	xpcc::Timeout<> timer;
	uint8_t temperatureData[2];
	xpcc::Tmp102<I2cMaster1> temperature;
};

ProtofaceDriverTest test;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	I2cMaster1::initialize<defaultSystemClock, 100000>();

	while (1)
	{
		test.update();
	}

	return 0;
}
