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

#include <atomic>
#include <cstdint>
#include <modm/board.hpp>
#include <modm/driver/inertial/bmi270.hpp>

using namespace Board;

using I2c = I2cMaster1;
using Scl = GpioB8;  // D15
using Sda = GpioB9;  // D14

using Int1 = GpioD15;  // D8
using Int2 = GpioF3;   // D9

using Transport = modm::Bmi270I2cTransport<I2c>;
using Imu = modm::Bmi270<Transport>;

Imu imu{static_cast<uint8_t>(0x68)};

std::atomic<uint32_t> accInterruptCount{0};
std::atomic<uint32_t> gyroInterruptCount{0};

std::atomic<int32_t> accX{0};
std::atomic<int32_t> accY{0};
std::atomic<int32_t> accZ{0};

std::atomic<int32_t> gyroX{0};
std::atomic<int32_t> gyroY{0};
std::atomic<int32_t> gyroZ{0};

std::atomic_flag imuBusLock = ATOMIC_FLAG_INIT;

bool
tryLockImuBus()
{
	return !imuBusLock.test_and_set(std::memory_order_acquire);
}

void
unlockImuBus()
{
	imuBusLock.clear(std::memory_order_release);
}

void
onDataReadyInterrupt()
{
	if (!tryLockImuBus()) { return; }

	const auto status = imu.getStatus();
	if (!status)
	{
		unlockImuBus();
		return;
	}

	if (status->accDataReady)
	{
		const auto data = imu.readAccData();
		if (data)
		{
			accInterruptCount.fetch_add(1, std::memory_order_relaxed);
			accX.store(data->raw[0], std::memory_order_relaxed);
			accY.store(data->raw[1], std::memory_order_relaxed);
			accZ.store(data->raw[2], std::memory_order_relaxed);
		}
	}

	if (status->gyroDataReady)
	{
		const auto data = imu.readGyroData();
		if (data)
		{
			gyroInterruptCount.fetch_add(1, std::memory_order_relaxed);
			gyroX.store(data->raw[0], std::memory_order_relaxed);
			gyroY.store(data->raw[1], std::memory_order_relaxed);
			gyroZ.store(data->raw[2], std::memory_order_relaxed);
		}
	}

	unlockImuBus();
}

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
	int2.outputEnable = false;
	int2.inputEnable = false;
	ok &= imu.setInt2IoControl(int2);

	ok &= imu.setInterruptLatch(Imu::InterruptLatch::None);

	Imu::InterruptMapData intMap{};
	intMap.int1DataReady = true;
	intMap.int2DataReady = false;
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
	I2c::connect<Scl::Scl, Sda::Sda>(I2c::PullUps::Internal);
	I2c::initialize<Board::SystemClock, 1_MHz, 10_pct>();
	Int1::setInput(Int1::InputType::PullDown);
	Int2::setInput(Int2::InputType::PullDown);

	MODM_LOG_INFO << "BMI270 I2C interrupt example" << modm::endl;

	if (!configureDriver()) { MODM_LOG_ERROR << "Configuration failed!" << modm::endl; }

	Exti::connect<Int1>(Exti::Trigger::RisingEdge, [](auto) { onDataReadyInterrupt(); });

	while (true)
	{
		modm::this_fiber::sleep_for(1s);

		const uint32_t accCount = accInterruptCount.exchange(0, std::memory_order_relaxed);
		const uint32_t gyroCount = gyroInterruptCount.exchange(0, std::memory_order_relaxed);

		Imu::AccData accData{};
		accData.raw = modm::Vector3i(accX.load(std::memory_order_relaxed),
									 accY.load(std::memory_order_relaxed),
									 accZ.load(std::memory_order_relaxed));
		accData.range = Imu::AccRange::Range2g;

		Imu::GyroData gyroData{};
		gyroData.raw = modm::Vector3i(gyroX.load(std::memory_order_relaxed),
									  gyroY.load(std::memory_order_relaxed),
									  gyroZ.load(std::memory_order_relaxed));
		gyroData.range = Imu::GyroRange::Range2000dps;

		const modm::Vector3f acc = accData.getFloat();
		const modm::Vector3f gyro = gyroData.getFloat();

		std::optional<Imu::Temperature> temperature;
		std::optional<Imu::SensorStatus> status;
		while (!tryLockImuBus()) { modm::this_fiber::sleep_for(50us); }
		temperature = imu.getTemperature();
		status = imu.getStatus();
		unlockImuBus();

		MODM_LOG_INFO << "Interrupts in last 1s: acc=" << accCount << " gyro=" << gyroCount
					  << modm::endl;

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

		if (status)
		{
			MODM_LOG_INFO << "Current Status: acc=" << status->accDataReady
						  << " gyro=" << status->gyroDataReady << " aux=" << status->auxDataReady
						  << " cmd=" << status->commandReady << " auxBusy=" << status->auxBusy
						  << modm::endl;
		} else
		{
			MODM_LOG_INFO << "Status: unavailable" << modm::endl;
		}
		MODM_LOG_INFO << modm::endl;

		Board::LedGreen::toggle();
	}

	return 0;
}
