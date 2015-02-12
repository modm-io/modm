#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/processing.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/driver/color/tcs3414.hpp>

#include <xpcc/io/iostream.hpp>

#include <xpcc/architecture/interface/gpio.hpp>

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > device;
xpcc::IOStream stream(device);

/**
 * Example to demonstrate a XPCC driver for colour sensor TCS3414
 *
 * This example uses I2cMaster2 of STM32F407
 *
 * SDA PB11
 * SCL PB10
 *
 * GND and +3V3 are connected to the colour sensor.
 *
 */

// typedef I2cMaster1 MyI2cMaster;
typedef I2cMaster2 MyI2cMaster;
// typedef xpcc::SoftwareI2cMaster<GpioB10, GpioB11> MyI2cMaster;

class ThreadOne : public xpcc::pt::Protothread
{
public:
	ThreadOne() :
		colorSensor()
	{
	}

	bool
	update()
	{
		PT_BEGIN();

		stream << "Ping the device from ThreadOne" << xpcc::endl;

		// ping the device until it responds
		while (true)
		{
			// we wait until the task started
			if (PT_CALL(colorSensor.ping())) {
			 	break;
			}
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		stream << "Device responded" << xpcc::endl;

		while (true)
		{
			if (PT_CALL(colorSensor.initialize())) {
				break;
			}
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		stream << "Device initialized" << xpcc::endl;

		while (true)
		{
			if (PT_CALL(colorSensor.configure(
					xpcc::tcs3414::Gain::X16,
					xpcc::tcs3414::Prescaler::DEFAULT,
					xpcc::tcs3414::IntegrationMode::INTERNAL,
					static_cast<uint8_t>(xpcc::tcs3414::NominalIntegrationTime::MSEC_100)))){
				break;
			}
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		stream << "Device configured" << xpcc::endl;

		while (true)
		{
			if (PT_CALL(colorSensor.refreshAllColors())) {
				auto colors = colorSensor.getOldColors();
				stream.printf("RGB: %5d %5d %5d", colors.red, colors.green, colors.blue);
				xpcc::color::HsvT<xpcc::tcs3414::UnderlyingType> hsv;
				colors.toHsv(&hsv);
				stream.printf("  %5d\n", hsv.hue);
			}
			this->timeout.restart(500);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
	xpcc::Tcs3414<MyI2cMaster> colorSensor;
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

	GpioB10::setOutput(Gpio::OutputType::OpenDrain);
	GpioB11::setOutput(Gpio::OutputType::OpenDrain);

	GpioB11::connect(I2cMaster2::Sda);
	GpioB10::connect(I2cMaster2::Scl);

	MyI2cMaster::initialize<defaultSystemClock, 100000>();

	stream << "\n\nWelcome to TCS3414 demo!\n\n";

	xpcc::ShortPeriodicTimer tmr(500);

	while (1)
	{
		one.update();
		if (tmr.execute()) {
			LedOrange::toggle();
		}
	}

	return 0;
}
