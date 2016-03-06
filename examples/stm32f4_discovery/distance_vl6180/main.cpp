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
 *
 * GND and +3V are connected to the sensor.
 */

typedef I2cMaster2 MyI2cMaster;
// typedef xpcc::SoftwareI2cMaster<GpioB10, GpioB11> MyI2cMaster;

xpcc::vl6180::Data data;
xpcc::Vl6180<MyI2cMaster> distance(data);

class ThreadOne : public xpcc::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

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

		while (true)
		{
			stamp = xpcc::Clock::now();

			if (PT_CALL(distance.readDistance()))
			{
				xpcc::vl6180::RangeErrorCode error = distance.getRangeError();
				if (error == distance.RangeErrorCode::NoError)
				{
					uint8_t mm = distance.getData().getDistance();
					XPCC_LOG_DEBUG << "mm: " << mm;
					Board::LedGreen::set(mm > 160);
					Board::LedBlue::set(mm > 110);
					Board::LedRed::set(mm > 25);
				}
				else {
					XPCC_LOG_DEBUG << "Error: " << (uint8_t(error) >> 4);
					Board::LedGreen::set();
					Board::LedBlue::set();
					Board::LedRed::set();
				}
			}

			XPCC_LOG_DEBUG << "\tt=" << (xpcc::Clock::now() - stamp);
			stamp = xpcc::Clock::now();

			if (PT_CALL(distance.readAmbientLight()))
			{
				xpcc::vl6180::ALS_ErrorCode error = distance.getALS_Error();
				if (error == distance.ALS_ErrorCode::NoError)
				{
					uint32_t lux = distance.getData().getAmbientLight();
					XPCC_LOG_DEBUG << "\tLux: " << lux;
				}
				else {
					XPCC_LOG_DEBUG << "\tError: " << (uint8_t(error) >> 4);
				}
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
int
main()
{
	Board::initialize();

	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<Board::systemClock, xpcc::Uart::B115200>(10);

	GpioB11::connect(I2cMaster2::Sda);
	GpioB10::connect(I2cMaster2::Scl);

	MyI2cMaster::initialize<Board::systemClock, 400000>();

	XPCC_LOG_INFO << "\n\nWelcome to VL6180X demo!\n\n";

	xpcc::ShortPeriodicTimer tmr(500);

	while (1)
	{
		one.update();
		if (tmr.execute()) {
			Board::LedOrange::toggle();
		}
	}

	return 0;
}
