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

#include <array>
#include <atomic>
#include <cstdint>
#include <modm/board.hpp>
#include <modm/driver/inertial/bmi270.hpp>

using namespace Board;

using I2c = I2cMaster1;
using Scl = GpioB8;  // D15
using Sda = GpioB9;  // D14

using Int1 = GpioD15;  // D9
using Int2 = GpioF3;   // D8

using Transport = modm::Bmi270I2cTransport<I2c>;
using Imu = modm::Bmi270<Transport>;

Imu imu{Transport::I2cAddress::SdoLow};

constexpr uint16_t FifoWatermarkBytes = 384;
constexpr std::size_t FifoBufferSize = 2048;
constexpr std::size_t FifoFrameSizeAccGyro = 12;
constexpr std::size_t FifoGyroOffset = 0;
constexpr std::size_t FifoAccOffset = 6;

std::atomic_bool fifoWatermarkInterrupt{false};

void
onFifoWatermarkInterrupt()
{
	fifoWatermarkInterrupt.store(true, std::memory_order_release);
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
	ok &= imu.setAccRate(Imu::AccRate::Rate25Hz_Normal);
	ok &= imu.setAccRange(Imu::AccRange::Range2g);
	ok &= imu.setGyroRate(Imu::GyroRate::Rate25Hz_Normal);
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
	intMap.int1FifoWatermark = true;
	intMap.int2FifoWatermark = true;
	ok &= imu.setInterruptMapData(intMap);

	ok &= imu.setPowerControl(Imu::PowerControl::Accelerometer | Imu::PowerControl::Gyroscope |
							  Imu::PowerControl::Temperature);

	const auto powerConfiguration = imu.getPowerConfiguration();
	if (!powerConfiguration) { return false; }

	Imu::PowerConfiguration_t powerConf = *powerConfiguration;
	powerConf.value &= ~uint8_t(Imu::PowerConfiguration::AdvancedPowerSave);
	ok &= imu.setPowerConfiguration(powerConf);

	Imu::FifoConfiguration fifoConfiguration{};
	fifoConfiguration.stopOnFull = false;
	fifoConfiguration.timeEnable = false;
	fifoConfiguration.tagInt1 = Imu::FifoTagInterrupt::Edge;
	fifoConfiguration.tagInt2 = Imu::FifoTagInterrupt::Edge;
	fifoConfiguration.headerEnable = false;
	fifoConfiguration.auxEnable = false;
	fifoConfiguration.accEnable = false;
	fifoConfiguration.gyroEnable = false;
	ok &= imu.setFifoConfiguration(fifoConfiguration);

	fifoConfiguration.accEnable = true;
	fifoConfiguration.gyroEnable = true;
	ok &= imu.setFifoConfiguration(fifoConfiguration);

	Imu::FifoDownsampling fifoDownsampling{};
	fifoDownsampling.gyroDownsampling = 0;
	fifoDownsampling.gyroFilterData = Imu::FifoFilterData::Filtered;
	fifoDownsampling.accDownsampling = 0;
	fifoDownsampling.accFilterData = Imu::FifoFilterData::Filtered;
	ok &= imu.setFifoDownsampling(fifoDownsampling);

	ok &= imu.setFifoWatermark(FifoWatermarkBytes);
	ok &= imu.flushFifo();

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

	MODM_LOG_INFO << "BMI270 I2C FIFO watermark interrupt example" << modm::endl;

	if (!configureDriver()) { MODM_LOG_ERROR << "Configuration failed!" << modm::endl; }

	std::array<uint8_t, FifoBufferSize> fifoBuffer{};

	while (true)
	{
		if (!Int1::read())
		{
			modm::this_fiber::sleep_for(5ms);
			continue;
		}

		const auto fifoLength = imu.readFifo(std::span{fifoBuffer});
		if (!fifoLength)
		{
			MODM_LOG_ERROR << "FIFO read failed" << modm::endl;
			continue;
		}

		const uint16_t bytesRead = *fifoLength;
		const std::size_t frameCount = bytesRead / FifoFrameSizeAccGyro;
		if (frameCount == 0)
		{
			MODM_LOG_WARNING << "FIFO contained no complete accel+gyro frame. bytes=" << bytesRead
							 << modm::endl;
			continue;
		}

		auto readAxis = [&](std::size_t index) {
			return static_cast<int16_t>(uint16_t(fifoBuffer[index]) |
										(uint16_t(fifoBuffer[index + 1]) << 8));
		};

		MODM_LOG_INFO << "FIFO bytes read: " << bytesRead << " (frames=" << frameCount << ")"
					  << modm::endl;

		MODM_LOG_INFO << "ACC samples [mg]:" << modm::endl;
		for (std::size_t frameIndex = 0; frameIndex < frameCount; ++frameIndex)
		{
			const std::size_t frameOffset = frameIndex * FifoFrameSizeAccGyro;
			Imu::AccData accData{};
			accData.raw = modm::Vector3i(readAxis(frameOffset + FifoAccOffset + 0),
										 readAxis(frameOffset + FifoAccOffset + 2),
										 readAxis(frameOffset + FifoAccOffset + 4));
			accData.range = Imu::AccRange::Range2g;
			const modm::Vector3f acc = accData.getFloat();
			MODM_LOG_INFO << "  [" << frameIndex << "] x: " << acc[0] << " y: " << acc[1]
						  << " z: " << acc[2] << modm::endl;
		}

		MODM_LOG_INFO << "GYRO samples [deg/s]:" << modm::endl;
		for (std::size_t frameIndex = 0; frameIndex < frameCount; ++frameIndex)
		{
			const std::size_t frameOffset = frameIndex * FifoFrameSizeAccGyro;
			Imu::GyroData gyroData{};
			gyroData.raw = modm::Vector3i(readAxis(frameOffset + FifoGyroOffset + 0),
										  readAxis(frameOffset + FifoGyroOffset + 2),
										  readAxis(frameOffset + FifoGyroOffset + 4));
			gyroData.range = Imu::GyroRange::Range2000dps;
			const modm::Vector3f gyro = gyroData.getFloat();
			MODM_LOG_INFO << "  [" << frameIndex << "] x: " << gyro[0] << " y: " << gyro[1]
						  << " z: " << gyro[2] << modm::endl;
		}

		Board::LedGreen::toggle();
	}

	return 0;
}
