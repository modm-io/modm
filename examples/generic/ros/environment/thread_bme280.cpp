#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>

#include "thread_bme280.hpp"

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DISABLED

// ----------------------------------------------------------------------------
Bme280Thread::Bme280Thread() :
	barometer(data, 0x76),
	start_measurement(false),
	new_data(false)
{
}

bool
Bme280Thread::startMeasurement()
{
	start_measurement = true;
	new_data = false;
	return true;
}

bool
Bme280Thread::update()
{
	PT_BEGIN();

	XPCC_LOG_DEBUG << XPCC_FILE_INFO;
	XPCC_LOG_DEBUG << "Ping the BME280" << xpcc::endl;

	// ping the device until it responds
	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometer.ping()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	// Configure the device until it responds
	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometer.initialize()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	XPCC_LOG_DEBUG << XPCC_FILE_INFO;
	XPCC_LOG_DEBUG << "BME280 configured" << xpcc::endl;

	{
		static xpcc::bme280::Calibration &cal = data.getCalibration();

		XPCC_LOG_DEBUG << XPCC_FILE_INFO;
		XPCC_LOG_DEBUG << "BME280 Calibration data is: " << xpcc::endl;
		XPCC_LOG_DEBUG.printf(" T1 %d\n", cal.T1);
		XPCC_LOG_DEBUG.printf(" T2 %d\n", cal.T2);
		XPCC_LOG_DEBUG.printf(" T3 %d\n", cal.T3);
		XPCC_LOG_DEBUG.printf(" P1 %d\n", cal.P1);
		XPCC_LOG_DEBUG.printf(" P2 %d\n", cal.P2);
		XPCC_LOG_DEBUG.printf(" P3 %d\n", cal.P3);
		XPCC_LOG_DEBUG.printf(" P4 %d\n", cal.P4);
		XPCC_LOG_DEBUG.printf(" P5 %d\n", cal.P5);
		XPCC_LOG_DEBUG.printf(" P6 %d\n", cal.P6);
		XPCC_LOG_DEBUG.printf(" P7 %d\n", cal.P7);
		XPCC_LOG_DEBUG.printf(" P8 %d\n", cal.P8);
		XPCC_LOG_DEBUG.printf(" P9 %d\n", cal.P9);
		XPCC_LOG_DEBUG.printf(" H1 %d\n", cal.H1);
		XPCC_LOG_DEBUG.printf(" H2 %d\n", cal.H2);
		XPCC_LOG_DEBUG.printf(" H3 %d\n", cal.H3);
		XPCC_LOG_DEBUG.printf(" H4 %d\n", cal.H4);
		XPCC_LOG_DEBUG.printf(" H5 %d\n", cal.H5);
		XPCC_LOG_DEBUG.printf(" H6 %d\n", cal.H6);
	}

	while (true)
	{
		PT_WAIT_UNTIL(start_measurement);

		// Returns when new data was read from the sensor
		PT_CALL(barometer.readout());
		new_data = true;

		{
			int32_t temp = data.getTemperature();
			int32_t press = data.getPressure();
			int32_t hum = data.getHumidity();
			XPCC_LOG_DEBUG << XPCC_FILE_INFO;
			XPCC_LOG_DEBUG.printf("BME280: Calibrated temperature in 0.01 degree Celsius is: %" PRId32 "\n", temp  );
			XPCC_LOG_DEBUG << XPCC_FILE_INFO;
			XPCC_LOG_DEBUG.printf("BME280: Calibrated pressure in mPa is                   : %" PRId32 "\n", press );
			XPCC_LOG_DEBUG << XPCC_FILE_INFO;
			XPCC_LOG_DEBUG.printf("BME280: Calibrated humidity in 0.001 %% is               : %" PRId32 "\n", hum );
		}

		start_measurement = false;
	}

	PT_END();
}
