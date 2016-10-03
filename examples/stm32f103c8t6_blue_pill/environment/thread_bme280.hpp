#ifndef THREAD_BME280
#define THREAD_BME280

#include <xpcc/processing/timer.hpp>
#include <xpcc/processing/protothread.hpp>

#include <xpcc/driver/pressure/bme280.hpp>

#include "hardware.hpp"

class Bme280Thread: public xpcc::pt::Protothread
{
public:
	Bme280Thread();

	bool
	update();

	bool
	startMeasurement();

	bool
	isNewDataAvailable() {
		return new_data;
	}

	int32_t
	getTemperatureA() {
		return dataA.getTemperature();
	}

	int32_t
	getPressureA() {
		return dataA.getPressure();
	}

	int32_t
	getHumidityA() {
		return dataA.getHumidity();
	}

	int32_t
	getTemperatureB() {
		return dataB.getTemperature();
	}

	int32_t
	getPressureB() {
		return dataB.getPressure();
	}

	int32_t
	getHumidityB() {
		return dataB.getHumidity();
	}

private:
	xpcc::ShortTimeout timeout;

	xpcc::bme280::Data dataA;
	xpcc::Bme280<SensorsAI2cMaster> barometerA;

	xpcc::bme280::Data dataB;
	xpcc::Bme280<SensorsBI2cMaster> barometerB;

	bool start_measurement;
	bool new_data;
};

#endif // THREAD_BME280
