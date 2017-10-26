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
	getTemperature() {
		return data.getTemperature();
	}

	int32_t
	getPressure() {
		return data.getPressure();
	}

	int32_t
	getHumidity() {
		return data.getHumidity();
	}

private:
	xpcc::ShortTimeout timeout;

	xpcc::bme280::Data data;
	xpcc::Bme280<MyI2cMaster> barometer;

	bool start_measurement;
	bool new_data;
};

#endif
