// coding: utf-8
/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/inertial/ixm42xxx.hpp>
#include <modm/math.hpp>
#include <modm/platform.hpp>
#include <modm/processing.hpp>

using SpiMaster = modm::platform::SpiMaster1;
using Mosi = GpioA7;
using Miso = GpioA6;
using Sck = GpioA5;
using Cs = GpioC5;

class ImuThread : public modm::pt::Protothread, public modm::ixm42xxx
{
    using Transport = modm::Ixm42xxxTransportSpi< SpiMaster, Cs >;

public:
    ImuThread() : imu(data), timer(std::chrono::milliseconds(500)) {}

    bool
    run()
    {
        PT_BEGIN();

        /// Initialize the IMU and verify that it is connected
        PT_CALL(imu.initialize());
        while (not PT_CALL(imu.ping()))
        {
            MODM_LOG_ERROR << "Cannot ping IXM42xxx" << modm::endl;
            PT_WAIT_UNTIL(timer.execute());
        }

        /// Configure data sensors
        PT_CALL(imu.updateRegister(Register::GYRO_CONFIG0, GyroFs_t(GyroFs::dps2000) | GyroOdr_t(GyroOdr::kHz1)));
        PT_CALL(imu.updateRegister(Register::ACCEL_CONFIG0, AccelFs_t(AccelFs::g16) | AccelOdr_t(AccelOdr::kHz1)));
        PT_CALL(imu.updateRegister(Register::PWR_MGMT0, GyroMode_t(GyroMode::LowNoise) | AccelMode_t(AccelMode::LowNoise)));

        while (true)
        {
            PT_WAIT_UNTIL(timer.execute());
            PT_CALL(imu.readSensorData());

            data.getTemp(&temp);
            data.getAccel(&accel);
            data.getGyro(&gyro);

            MODM_LOG_INFO.printf("Temp: %.3f\n", temp);
            MODM_LOG_INFO.printf("Accel: (%.3f, %.3f, %.3f)\n", accel.x, accel.y, accel.z);
            MODM_LOG_INFO.printf("Gyro: (%.3f, %.3f, %.3f)\n", gyro.x, gyro.y, gyro.z);
        }
        PT_END();
    }

private:
    float temp;
    modm::Vector3f accel;
    modm::Vector3f gyro;

    modm::ixm42xxxdata::Data data;
    modm::Ixm42xxx< Transport > imu;

    modm::PeriodicTimer timer;

} imuThread;

int
main()
{
    Board::initialize();

    Cs::setOutput(modm::Gpio::High);
    SpiMaster::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
    SpiMaster::initialize<Board::SystemClock, 21.5_MHz>();

    MODM_LOG_INFO       << "==========IXM-42xxx Test==========" << modm::endl;
    MODM_LOG_DEBUG      << "Debug logging here" << modm::endl;
    MODM_LOG_INFO       << "Info logging here" << modm::endl;
    MODM_LOG_WARNING    << "Warning logging here" << modm::endl;
    MODM_LOG_ERROR      << "Error logging here" << modm::endl;
    MODM_LOG_INFO       << "==================================" << modm::endl;

    while (true)
    {
        imuThread.run();
    }

    return 0;
}
