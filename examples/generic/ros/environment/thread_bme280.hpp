#ifndef THREAD_BME280
#define THREAD_BME280

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

#include <modm/driver/pressure/bme280.hpp>

#include "hardware.hpp"

class Bme280Thread: public modm::pt::Protothread
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
	modm::ShortTimeout timeout;

	modm::bme280::Data data;
	modm::Bme280<MyI2cMaster> barometer;

	bool start_measurement;
	bool new_data;
};

#endif
