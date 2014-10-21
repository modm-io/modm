#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/processing/timeout.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/driver/pressure/bmp085.hpp>

#include <xpcc/io/iostream.hpp>

#include <xpcc/architecture/peripheral/gpio.hpp>

xpcc::IODeviceWrapper<Usart2> device;
xpcc::IOStream stream(device);

/**
 * Example to demonstrate a XPCC driver for barometer and
 * thermometer BMP085 and BMP180 (which are compatible).
 *
 * PB9	SDA
 * PB8	SCL
 *
 * GND and +3V3 are connected to the barometer module.
 *
 */

typedef I2cMaster1 MyI2cMaster;

class ThreadOne : public xpcc::pt::Protothread
{
public:
	ThreadOne() :
		barometer(data, 0x77)
	{
	}

	uint8_t data[5];

	bool
	update()
	{
		barometer.update();

		PT_BEGIN()

		stream << "Ping the device from ThreadOne" << xpcc::endl;

		// ping the device until it responds
		while(true)
		{
			// we wait until the task started
			if (PT_CALL(barometer.ping(this)))
				break;
			// otherwise, try again in 100ms
			this->timer.restart(100);
			PT_WAIT_UNTIL(this->timer.isExpired());
		}

		stream << "Device responded" << xpcc::endl;

//		PT_CALL(portExpander.writePort(this, 0xaa));

		PT_END();
	}

private:
	xpcc::Timeout<> timer;
	xpcc::Bmp085<MyI2cMaster> barometer;
};


ThreadOne one;

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
	Usart2::initialize<defaultSystemClock, xpcc::Uart::B115200>(10);

	GpioB9::connect(MyI2cMaster::Sda);
	GpioB8::connect(MyI2cMaster::Scl);
	GpioB9::configure(Gpio::InputType::PullUp);
	GpioB8::configure(Gpio::InputType::PullUp);
	MyI2cMaster::initialize<defaultSystemClock, 50000>();

	stream << "\n\nWelcome to BMP085 demo!\n\n";

	while (1)
	{
		one.update();
		LedOrange::toggle();
	}

	return 0;
}
