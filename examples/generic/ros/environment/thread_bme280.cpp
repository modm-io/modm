#include <modm/platform/platform.hpp>
#include <modm/debug/logger.hpp>

#include "thread_bme280.hpp"

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DISABLED

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

	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "Ping the BME280" << modm::endl;

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

	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "BME280 configured" << modm::endl;

	{
		static modm::bme280::Calibration &cal = data.getCalibration();

		MODM_LOG_DEBUG << MODM_FILE_INFO;
		MODM_LOG_DEBUG << "BME280 Calibration data is: " << modm::endl;
		MODM_LOG_DEBUG.printf(" T1 %d\n", cal.T1);
		MODM_LOG_DEBUG.printf(" T2 %d\n", cal.T2);
		MODM_LOG_DEBUG.printf(" T3 %d\n", cal.T3);
		MODM_LOG_DEBUG.printf(" P1 %d\n", cal.P1);
		MODM_LOG_DEBUG.printf(" P2 %d\n", cal.P2);
		MODM_LOG_DEBUG.printf(" P3 %d\n", cal.P3);
		MODM_LOG_DEBUG.printf(" P4 %d\n", cal.P4);
		MODM_LOG_DEBUG.printf(" P5 %d\n", cal.P5);
		MODM_LOG_DEBUG.printf(" P6 %d\n", cal.P6);
		MODM_LOG_DEBUG.printf(" P7 %d\n", cal.P7);
		MODM_LOG_DEBUG.printf(" P8 %d\n", cal.P8);
		MODM_LOG_DEBUG.printf(" P9 %d\n", cal.P9);
		MODM_LOG_DEBUG.printf(" H1 %d\n", cal.H1);
		MODM_LOG_DEBUG.printf(" H2 %d\n", cal.H2);
		MODM_LOG_DEBUG.printf(" H3 %d\n", cal.H3);
		MODM_LOG_DEBUG.printf(" H4 %d\n", cal.H4);
		MODM_LOG_DEBUG.printf(" H5 %d\n", cal.H5);
		MODM_LOG_DEBUG.printf(" H6 %d\n", cal.H6);
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
			MODM_LOG_DEBUG << MODM_FILE_INFO;
			MODM_LOG_DEBUG.printf("BME280: Calibrated temperature in 0.01 degree Celsius is: %d\n", temp  );
			MODM_LOG_DEBUG << MODM_FILE_INFO;
			MODM_LOG_DEBUG.printf("BME280: Calibrated pressure in mPa is                   : %d\n", press );
			MODM_LOG_DEBUG << MODM_FILE_INFO;
			MODM_LOG_DEBUG.printf("BME280: Calibrated humidity in 0.001 %% is               : %d\n", hum );
		}

		start_measurement = false;
	}

	PT_END();
}
