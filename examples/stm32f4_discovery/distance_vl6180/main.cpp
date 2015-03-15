#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/processing.hpp>
#include <xpcc/driver/position/vl6180.hpp>
#include <xpcc/io/iostream.hpp>
#include <xpcc/debug/logger.hpp>

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > device;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(device);
xpcc::log::Logger xpcc::log::info(device);
xpcc::log::Logger xpcc::log::warning(device);
xpcc::log::Logger xpcc::log::error(device);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

/**
 * Example to demonstrate a XPCC driver for distance sensor VL6180
 *
 * This example uses I2cMaster2 of STM32F407
 *
 * SDA		PB11
 * SCL		PB10
 * GPIO0	PB14
 *
 * GND and +3V are connected to the sensor.
 */

typedef I2cMaster2 MyI2cMaster;
// typedef xpcc::SoftwareI2cMaster<GpioB10, GpioB11> MyI2cMaster;
typedef GpioOutputB14 VlReset;

xpcc::vl6180::Data data;
xpcc::Vl6180<MyI2cMaster> distance(data);

class ThreadOne : public xpcc::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		VlReset::reset();
		this->timeout.restart(1);
		PT_WAIT_UNTIL(this->timeout.isExpired());

		// bring device out of reset
		VlReset::set();
		// wait 5ms
		this->timeout.restart(5);
		PT_WAIT_UNTIL(this->timeout.isExpired());

		XPCC_LOG_DEBUG << "Ping the device from ThreadOne" << xpcc::endl;

		// ping the device until it responds
		while (true)
		{
			// we wait until the task started
			if (PT_CALL(distance.ping())) {
			 	break;
			}
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		XPCC_LOG_DEBUG << "Device responded" << xpcc::endl;

		while (true)
		{
			if (PT_CALL(distance.initialize()))
				break;
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		XPCC_LOG_DEBUG << "Device initialized" << xpcc::endl;
		this->timeout.restart(1);

		PT_CALL(distance.setIntegrationTime(10));
		PT_CALL(distance.setAddress(100));

		while (true)
		{
			stamp = xpcc::Clock::now();

			if (PT_CALL(distance.readDistance()))
			{
				uint8_t mm = distance.data.getDistance();
				XPCC_LOG_DEBUG << "mm: " << mm;
				LedGreen::set(mm > 100);
				LedBlue::set(mm > 50);
				LedRed::set(mm > 10);
			}

			XPCC_LOG_DEBUG << "\tt=" << (xpcc::Clock::now() - stamp);
			stamp = xpcc::Clock::now();

			if (PT_CALL(distance.readAmbientLight()))
			{
				uint32_t lux = distance.data.getAmbientLight();
				XPCC_LOG_DEBUG << "\tLux: " << lux;
			}

			XPCC_LOG_DEBUG << " \tt=" << (xpcc::Clock::now() - stamp) << xpcc::endl;

			PT_WAIT_UNTIL(this->timeout.isExpired());
			this->timeout.restart(40);
		}

		PT_END();
	}

private:
	xpcc::Timeout timeout;
	xpcc::Timestamp stamp;
};

ThreadOne one;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();
	xpcc::cortex::SysTickTimer::enable();

	LedOrange::setOutput(xpcc::Gpio::Low);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::Low);
	LedBlue::setOutput(xpcc::Gpio::Low);
	VlReset::setOutput(Gpio::OutputType::OpenDrain);

	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<defaultSystemClock, xpcc::Uart::B115200>(10);

	GpioB10::setOutput(Gpio::OutputType::OpenDrain);
	GpioB11::setOutput(Gpio::OutputType::OpenDrain);

	GpioB11::connect(I2cMaster2::Sda);
	GpioB10::connect(I2cMaster2::Scl);

	MyI2cMaster::initialize<defaultSystemClock, 400000>();

	XPCC_LOG_INFO << "\n\nWelcome to VL6180X demo!\n\n";

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
