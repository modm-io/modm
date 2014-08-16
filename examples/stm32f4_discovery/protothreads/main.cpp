#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/processing/timeout.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/driver/temperature/tmp102.hpp>

#include <xpcc/io/iostream.hpp>

xpcc::IODeviceWrapper<Usart2> device;
xpcc::IOStream stream(device);

// example of a debug transaction, which outputs the wrapped transactions choices.
class CustomI2cTransaction : public xpcc::I2cTransaction
{
	xpcc::I2cTransaction *transaction;

public:
	CustomI2cTransaction()
			:	transaction(nullptr)
	{}

	void
	setTransaction(xpcc::I2cTransaction *transaction)
	{
		this->transaction = transaction;
	}

protected:
	virtual bool
	attaching()
	{
		bool result = transaction->attaching();
		stream << "T: attaching: " << result << xpcc::endl;
		return result;
	}

	virtual Starting
	starting()
	{
		xpcc::I2cTransaction::Starting result = transaction->starting();
		stream << "T: starting: addr=" << result.address << " next=" << (int)result.next << xpcc::endl;
		return result;
	}

	virtual Writing
	writing()
	{
		xpcc::I2cTransaction::Writing result = transaction->writing();
		stream << "T: writing: length=" << result.length << " next=" << (int)result.next << xpcc::endl;
		return result;
	}

	virtual Reading
	reading()
	{
		xpcc::I2cTransaction::Reading result = transaction->reading();
		stream << "T: reading: length=" << result.length << " next=" << (int)result.next << xpcc::endl;
		return result;
	}

	virtual void
	detaching(DetachCause cause)
	{
		transaction->detaching(cause);
		stream << "T: detaching: cause=" << (int)cause << xpcc::endl;
	}
};
static CustomI2cTransaction myTransaction;


// this custom I2c Master wraps the original transaction into the above debug transaction.
class MyI2cMaster2 : public I2cMaster1
{
public:
	static bool
	start(xpcc::I2cTransaction *transaction, xpcc::I2c::Configuration_t configuration = nullptr)
	{
		myTransaction.setTransaction(transaction);
		stream << "st" << xpcc::endl;
		return I2cMaster1::start(&myTransaction, configuration);
	}

	static bool
	startBlocking(xpcc::I2cTransaction *transaction, xpcc::I2c::Configuration_t configuration = nullptr)
	{
		myTransaction.setTransaction(transaction);
		stream << "stBl" << xpcc::endl;
		return I2cMaster1::start(&myTransaction, configuration);
	}
};

typedef I2cMaster1 MyI2cMaster;

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
			stream << "config: 20kHz" << xpcc::endl;
			MyI2cMaster::initialize<defaultSystemClock,  20000>();
			tag = 's';
		}
	}

	/// 400kHz I2C configuration
	static void
	fast_configuration()
	{
		if (tag != 'f') {
			stream << "config: 100kHz" << xpcc::endl;
			MyI2cMaster::initialize<defaultSystemClock, 100000>();
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
		// this i2c device will be driven with only 20kHz
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
				stream << "t1: " << (int)tf << xpcc::endl;
			} else {
				tf = NAN;
				stream << "t1: NACK"<< xpcc::endl;
			}

			this->timer.restart(200);
			PT_WAIT_UNTIL(this->timer.isExpired());
			LedRed::toggle();
		}

		PT_END();
	}

private:
	float tf;
	xpcc::Timeout<> timer;
	uint8_t temperatureData[2];
	xpcc::Tmp102<MyI2cMaster> temperature;
};


class ProtofaceTemperatureTwo : public xpcc::pt::Protothread
{
public:
	ProtofaceTemperatureTwo()
	:	temperature(temperatureData, 0x48)
	{
		// this device will be clocked at 100kHz
		temperature.attachPeripheralConfiguration(DeviceConfiguration::fast_configuration);
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
				stream << "t2: " << (int)tf << xpcc::endl;
			} else {
				tf = NAN;
				stream << "t2: NACK"<< xpcc::endl;
			}

			this->timer.restart(500);
			PT_WAIT_UNTIL(this->timer.isExpired());
			LedGreen::toggle();
		}

		PT_END();
	}

private:
	float tf;
	xpcc::Timeout<> timer;
	uint8_t temperatureData[2];
	xpcc::Tmp102<MyI2cMaster> temperature;
};

ProtofaceTemperatureOne one;
ProtofaceTemperatureTwo two;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();
	xpcc::cortex::SysTickTimer::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<defaultSystemClock, xpcc::Uart::B38400>(10);

	GpioB7::connect(MyI2cMaster::Sda);
	GpioB8::connect(MyI2cMaster::Scl);
	GpioB7::configure(Gpio::InputType::PullUp);
	GpioB8::configure(Gpio::InputType::PullUp);

	stream << "\n\nRESTART\n\n";

	while (1)
	{
		one.update();
		two.update();
		LedOrange::toggle();
	}

	return 0;
}
