/*
 * Copyright (c) 2026, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <cstdint>
#include <modm/board.hpp>
#include <modm/driver/inertial/bmi270.hpp>
#include <modm/processing/timer.hpp>

using namespace Board;

using I2c = I2cMaster1;
using Scl = GpioB8;  // D15
using Sda = GpioB9;  // D14

using Int1 = GpioD15;  // D8
using Int2 = GpioF3;   // D9

using Transport = modm::Bmi270I2cTransport<I2c>;
using Imu = modm::Bmi270<Transport>;

Imu imu{Transport::I2cAddress::SdoLow};

bool
configureDriver()
{
	while (!imu.initialize())
	{
		MODM_LOG_ERROR << "Initialization failed, retrying ..." << modm::endl;
		modm::this_fiber::sleep_for(500ms);
	}

	bool ok = true;
	ok &= imu.setAccRate(Imu::AccRate::Rate50Hz_Normal);
	ok &= imu.setAccRange(Imu::AccRange::Range2g);
	ok &= imu.setGyroRate(Imu::GyroRate::Rate1600Hz_Normal);
	ok &= imu.setGyroRange(Imu::GyroRange::Range2000dps);

	Imu::InterruptIoControl int1{};
	int1.level = Imu::InterruptOutputLevel::ActiveHigh;
	int1.outputType = Imu::InterruptOutputType::PushPull;
	int1.outputEnable = true;
	int1.inputEnable = false;
	ok &= imu.setInt1IoControl(int1);

	Imu::InterruptIoControl int2{};
	int2.level = Imu::InterruptOutputLevel::ActiveHigh;
	int2.outputType = Imu::InterruptOutputType::PushPull;
	int2.outputEnable = true;
	int2.inputEnable = false;
	ok &= imu.setInt2IoControl(int2);

	ok &= imu.setInterruptLatch(Imu::InterruptLatch::None);

	Imu::InterruptMapData intMap{};
	intMap.int1DataReady = true;
	intMap.int2DataReady = true;
	ok &= imu.setInterruptMapData(intMap);

	ok &= imu.setPowerControl(Imu::PowerControl::Accelerometer | Imu::PowerControl::Gyroscope |
							  Imu::PowerControl::Temperature);
	return ok;
}

int
main()
{
	Board::initialize();
	Leds::setOutput();
	I2c::connect<Scl::Scl, Sda::Sda>(I2c::PullUps::External);
	I2c::initialize<Board::SystemClock, 1_MHz, 10_pct>();
	Int1::setInput(Int1::InputType::PullDown);
	Int2::setInput(Int2::InputType::PullDown);

	MODM_LOG_INFO << "BMI270 I2C polling example" << modm::endl;

	if (!configureDriver()) { MODM_LOG_ERROR << "Configuration failed!" << modm::endl; }

	uint32_t accSampleCount{0};
	uint32_t gyroSampleCount{0};
	modm::PeriodicTimer printTimer{1s};

	Imu::AccData latestAccData{};
	latestAccData.raw = modm::Vector3i(0, 0, 0);
	latestAccData.range = Imu::AccRange::Range2g;

	Imu::GyroData latestGyroData{};
	latestGyroData.raw = modm::Vector3i(0, 0, 0);
	latestGyroData.range = Imu::GyroRange::Range2000dps;

	std::optional<Imu::SensorStatus> latestStatus;

	while (true)
	{
		if (Int1::read())
		{
			if (const auto status = imu.getStatus())
			{
				latestStatus = status;

				if (status->accDataReady)
				{
					if (const auto accData = imu.readAccData())
					{
						latestAccData = *accData;
						++accSampleCount;
					}
				}

				if (status->gyroDataReady)
				{
					if (const auto gyroData = imu.readGyroData())
					{
						latestGyroData = *gyroData;
						++gyroSampleCount;
					}
				}
			}
		}

		if (printTimer.execute())
		{
			const modm::Vector3f acc = latestAccData.getFloat();
			const modm::Vector3f gyro = latestGyroData.getFloat();
			const std::optional<Imu::Temperature> temperature = imu.getTemperature();

			MODM_LOG_INFO << "Data-ready events in last 1s: acc=" << accSampleCount
						  << " gyro=" << gyroSampleCount << modm::endl;

			accSampleCount = 0;
			gyroSampleCount = 0;

			MODM_LOG_INFO << "Latest Values Acc  [mg]    x: " << acc[0] << " y: " << acc[1]
						  << " z: " << acc[2] << modm::endl;
			MODM_LOG_INFO << "Latest Values Gyro [deg/s] x: " << gyro[0] << " y: " << gyro[1]
						  << " z: " << gyro[2] << modm::endl;

			if (temperature and temperature->valid)
			{
				MODM_LOG_INFO << "Current Temperature [C]: " << temperature->celsius << modm::endl;
			} else
			{
				MODM_LOG_INFO << "Temperature: invalid" << modm::endl;
			}

			if (latestStatus)
			{
				MODM_LOG_INFO << "Current Status: acc=" << latestStatus->accDataReady
							  << " gyro=" << latestStatus->gyroDataReady
							  << " aux=" << latestStatus->auxDataReady
							  << " cmd=" << latestStatus->commandReady
							  << " auxBusy=" << latestStatus->auxBusy << modm::endl;
			} else
			{
				MODM_LOG_INFO << "Status: unavailable" << modm::endl;
			}
			MODM_LOG_INFO << modm::endl;

			Board::LedGreen::toggle();
		}

		modm::this_fiber::yield();
	}

	return 0;
}
