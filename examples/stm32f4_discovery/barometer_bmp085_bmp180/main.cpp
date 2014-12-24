#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/processing.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/driver/pressure/bmp085.hpp>

#include <xpcc/io/iostream.hpp>

#include <xpcc/architecture/peripheral/gpio.hpp>

xpcc::IODeviceWrapper< Usart2, xpcc::IODevice::BufferBehavior::BusyWait > device;
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

	bool
	update()
	{
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

		// Configure the device until it responds
		while(true)
		{
			// we wait until the task started
			if (PT_CALL(barometer.configure(this)))
				break;
			// otherwise, try again in 100ms
			this->timer.restart(100);
			PT_WAIT_UNTIL(this->timer.isExpired());
		}

		stream << "Device configured" << xpcc::endl;

		static xpcc::bmp085::Calibration &cal = data.getCalibration();

		stream << "Calibration data is: ";
		stream.printf(" ac1 %d\n", cal.ac1);
		stream.printf(" ac2 %d\n", cal.ac2);
		stream.printf(" ac3 %d\n", cal.ac3);
		stream.printf(" ac4 %d\n", cal.ac4);
		stream.printf(" ac5 %d\n", cal.ac5);
		stream.printf(" ac6 %d\n", cal.ac6);
		stream.printf(" b1 %d\n", cal.b1);
		stream.printf(" b2 %d\n", cal.b2);
		stream.printf(" mb %d\n", cal.mb);
		stream.printf(" mc %d\n", cal.mc);
		stream.printf(" md %d\n", cal.md);

		while (1)
		{
			static xpcc::PeriodicTimer<> timer(250);

			PT_WAIT_UNTIL(timer.isExpired());

			// Returns when new data was read from the sensor
			PT_CALL(barometer.readout(this));

			{
				int16_t temp  = data.getTemperature();
				int32_t press = data.getPressure();

				stream.printf("Calibrated temperature in 0.1 degree Celsius is: %d\n",   temp  );
				stream.printf("Calibrated pressure in Pa is                   : %d\n\n", press );
			}
		}

		PT_END();
	}

private:
	xpcc::Timeout<> timer;

	xpcc::bmp085::Data data;
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
    MyI2cMaster::initialize<defaultSystemClock, MyI2cMaster::Baudrate::Standard>();

	stream << "\n\nWelcome to BMP085 demo!\n\n";

	while (1)
	{
		one.update();
		LedOrange::toggle();
	}

	return 0;
}
