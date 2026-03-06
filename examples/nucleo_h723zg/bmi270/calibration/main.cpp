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

#include <modm/board.hpp>
#include <modm/driver/inertial/bmi270.hpp>
#include <modm/processing.hpp>

using namespace Board;

using I2c = I2cMaster1;
using Scl = GpioB8;  // D15
using Sda = GpioB9;  // D14

using Transport = modm::Bmi270I2cTransport<I2c>;
using Imu = modm::Bmi270<Transport>;

Imu imu{static_cast<uint8_t>(0x68)};

namespace
{

struct CalibrationSnapshot
{
	Imu::GyroUserGain gyroUserGainOut;
};

bool
initializeImu()
{
	while (!imu.initialize())
	{
		MODM_LOG_ERROR << "Initialization failed, retrying..." << modm::endl;
		modm::this_fiber::sleep_for(500ms);
	}

	bool ok = true;
	ok &= imu.setAccRate(Imu::AccRate::Rate100Hz_Normal);
	ok &= imu.setAccRange(Imu::AccRange::Range4g);
	ok &= imu.setGyroRate(Imu::GyroRate::Rate100Hz_Normal);
	ok &= imu.setGyroRange(Imu::GyroRange::Range2000dps);
	ok &= imu.setPowerControl(Imu::PowerControl::Accelerometer | Imu::PowerControl::Gyroscope |
							  Imu::PowerControl::Temperature);
	return ok;
}

void
printGyroUserGain(const char* label, const Imu::GyroUserGain& gain)
{
	MODM_LOG_INFO << label << " x=" << int(gain.x) << " y=" << int(gain.y) << " z=" << int(gain.z)
				  << modm::endl;
}

bool
equalGyroUserGain(const Imu::GyroUserGain& lhs, const Imu::GyroUserGain& rhs)
{
	return (lhs.x == rhs.x) and (lhs.y == rhs.y) and (lhs.z == rhs.z);
}

}  // namespace

int
main()
{
	Board::initialize();
	Leds::setOutput();
	I2c::connect<Scl::Scl, Sda::Sda>(I2c::PullUps::Internal);
	I2c::initialize<Board::SystemClock, 1_MHz, 10_pct>();

	MODM_LOG_INFO << "BMI270 calibration example (CRT + gyro user gain restore)" << modm::endl;
	MODM_LOG_INFO << "Keep sensor stable while CRT runs" << modm::endl;

	if (!imu.reset()) { MODM_LOG_ERROR << "Initial soft reset command failed" << modm::endl; }
	if (!initializeImu()) { MODM_LOG_ERROR << "Initial IMU configuration failed" << modm::endl; }

	if (const auto userGain = imu.getGyroUserGain())
	{
		printGyroUserGain("Pre-CRT gyro user gain out", *userGain);
	}

	MODM_LOG_INFO << "Starting component retrim (CRT)..." << modm::endl;
	if (!imu.doCrt())
	{
		MODM_LOG_ERROR << "CRT failed" << modm::endl;
	} else
	{
		MODM_LOG_INFO << "CRT completed" << modm::endl;
	}

	const auto userGainOut = imu.getGyroUserGain();
	if (!userGainOut)
	{
		MODM_LOG_ERROR << "Reading user gain out after CRT failed" << modm::endl;
		return 1;
	}

	CalibrationSnapshot savedCalibration{};
	savedCalibration.gyroUserGainOut = *userGainOut;

	printGyroUserGain("Saved gyro user gain out", savedCalibration.gyroUserGainOut);

	MODM_LOG_INFO << "Soft resetting sensor..." << modm::endl;
	if (!imu.reset()) { MODM_LOG_ERROR << "Soft reset command failed" << modm::endl; }
	if (!initializeImu())
	{
		MODM_LOG_ERROR << "Re-initialization after reset failed" << modm::endl;
	}

	if (const auto userGainBeforeRestore = imu.getGyroUserGain())
	{
		printGyroUserGain("After reset, before restore gyro user gain out", *userGainBeforeRestore);
	}

	MODM_LOG_INFO << "Restoring saved calibration values..." << modm::endl;
	bool writeOk = true;
	writeOk &= imu.setGyroUserGain(savedCalibration.gyroUserGainOut);
	if (!writeOk) { MODM_LOG_ERROR << "Writing calibration values failed" << modm::endl; }

	const auto restoredUserGain = imu.getGyroUserGain();

	bool userGainMatch = false;
	if (restoredUserGain)
	{
		userGainMatch = equalGyroUserGain(savedCalibration.gyroUserGainOut, *restoredUserGain);
		printGyroUserGain("Restored gyro user gain out", *restoredUserGain);
	}

	MODM_LOG_INFO << "Verification: userGainOut=" << userGainMatch << modm::endl;

	const bool restoreOk = writeOk and userGainMatch;
	MODM_LOG_INFO << (restoreOk ? "Calibration restore PASSED" : "Calibration restore FAILED")
				  << modm::endl;

	static constexpr float LowPassAlpha{1e-3f};
	bool filterInitialized = false;
	modm::Vector3f filteredAcc{0.f, 0.f, 0.f};
	modm::Vector3f filteredGyro{0.f, 0.f, 0.f};

	modm::PeriodicTimer printTimer{1s};
	while (true)
	{
		if (const auto data = imu.readData())
		{
			const auto acc = data->acc.getFloat();
			const auto gyro = data->gyro.getFloat();

			if (!filterInitialized)
			{
				filteredAcc = acc;
				filteredGyro = gyro;
				filterInitialized = true;
			} else
			{
				filteredAcc[0] += LowPassAlpha * (acc[0] - filteredAcc[0]);
				filteredAcc[1] += LowPassAlpha * (acc[1] - filteredAcc[1]);
				filteredAcc[2] += LowPassAlpha * (acc[2] - filteredAcc[2]);
				filteredGyro[0] += LowPassAlpha * (gyro[0] - filteredGyro[0]);
				filteredGyro[1] += LowPassAlpha * (gyro[1] - filteredGyro[1]);
				filteredGyro[2] += LowPassAlpha * (gyro[2] - filteredGyro[2]);
			}
		}

		if (printTimer.execute())
		{
			if (filterInitialized)
			{
				MODM_LOG_INFO << "LPF (Bias) Acc [mg] x=" << filteredAcc[0]
							  << " y=" << filteredAcc[1] << " z=" << filteredAcc[2] << modm::endl;
				MODM_LOG_INFO << "LPF (Bias) Gyr [deg/s] x=" << filteredGyro[0]
							  << " y=" << filteredGyro[1] << " z=" << filteredGyro[2] << modm::endl;
			} else
			{
				MODM_LOG_ERROR << "No IMU sample received yet" << modm::endl;
			}

			if (restoreOk)
			{
				Board::LedGreen::toggle();
			} else
			{
				Board::LedRed::toggle();
			}
		}
	}

	return 0;
}
