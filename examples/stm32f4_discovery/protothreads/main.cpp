#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/processing/timeout.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/driver/temperature/tmp102.hpp>


struct DeviceConfiguration
{
	// shared memory for both devices as a cheap way
	// to `tag` the current configuration of the peripheral
	// so we do not need to configure it unnecessarily.
	static char tag;

	/// 100kHz I2C configuration
	static void
	standard_configuration()
	{
		if (tag != 's') {
			I2cMaster1::initialize<defaultSystemClock, 100000>();
			tag = 's';
		}
	}

	/// 400kHz I2C configuration
	static void
	fast_configuration()
	{
		if (tag != 'f') {
			I2cMaster1::initialize<defaultSystemClock, 400000>();
			tag = 'f';
		}
	}
};
char DeviceConfiguration::tag = 0;


class ProtofaceTemperatureOne : public xpcc::pt::Protothread
{
public:
	ProtofaceTemperatureOne()
	:	temperature(temperatureData, 0x48)
	{
		// this i2c device will be driven with only 100kHz
		temperature.attachPeripheralConfiguration(DeviceConfiguration::standard_configuration);
	}

	bool
	update()
	{
		PT_BEGIN();

		// we need to configure the device before we can use it
		// so we wait until the task started
		PT_WAIT_UNTIL(temperature.startConfigure());
		// and we wait until it finished
		PT_WAIT_UNTIL(temperature.runConfigure());

		while (true)
		{
			// lets wait until we can read out the device
			PT_WAIT_UNTIL(temperature.startReadTemperature());
			// and until this task finishes
			PT_WAIT_UNTIL(temperature.runReadTemperature());
			// did we succeed with the readout?
			if (temperature.isSuccessful()) {
				tf = temperature.getTemperature();
			} else {
				tf = NAN;
			}

			this->timer.restart(200);
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


class ProtofaceTemperatureTwo : public xpcc::pt::Protothread
{
public:
	ProtofaceTemperatureTwo()
	:	temperature(temperatureData, 0x49)
	{
		// this device will be clocked at 400kHz
		temperature.attachPeripheralConfiguration(DeviceConfiguration::fast_configuration);
	}

	bool
	update()
	{
		PT_BEGIN();

		//
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

ProtofaceTemperatureOne one;
ProtofaceTemperatureTwo two;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	while (1)
	{
		one.update();
		two.update();
	}

	return 0;
}
